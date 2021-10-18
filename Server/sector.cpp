#include <unordered_set>
#include <vector>
#include <unordered_map>

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

#include "ringBuffer.h"
#include "log.h"
#include "common.h"
#include "user.h"
#include "sector.h"

void CSingleSector::addUser(int id) {

	userList.insert(id);

}

void CSingleSector::eraseUser(int id) {

	std::unordered_set<int>::iterator uerIter = userList.find(id);
	userList.erase(id);

}

CSectorList::CSectorList(int mapWidth, int mapHeight, int sectorWidth, int sectorHeight) {

	_sectorHeight = sectorHeight;
	_sectorWidth = sectorWidth;

	_mapHeight = mapHeight;
	_mapWidth = mapWidth;

	_widthNum = mapWidth / sectorWidth + (mapWidth % sectorWidth > 0);
	_heightNum = mapHeight / sectorHeight + (mapHeight % sectorHeight > 0);

	_sector = new CSingleSector * [_heightNum];
	for (int heightCnt = 0; heightCnt < _heightNum; ++heightCnt) {
		_sector[heightCnt] = new CSingleSector[_widthNum];
	}

}

CSectorList::~CSectorList() {

	for (int heightCnt = 0; heightCnt < _heightNum; ++heightCnt) {
		delete[] _sector[heightCnt];
	}
	delete[] _sector;

}

void CSectorList::addUser(int y, int x, int id) {

	_sector[y][x].addUser(id);

}

void CSectorList::eraseUser(int y, int x, int id) {

	_sector[y][x].eraseUser(id);

}

void CSectorList::coordinateToIndex(int coordinateY, int coordinateX, int* indexY, int* indexX) {

	*indexY = coordinateY / _sectorHeight;
	*indexX = coordinateX / _sectorWidth;

}

std::vector<CSingleSector*>* CSectorList::nearSector(int y, int x) {

	std::vector<CSingleSector*>* nearSectorList;

	int cnt = 0;
	cnt += y == 0;
	cnt += y == _widthNum - 1;
	cnt += x == 0;
	cnt += x == _heightNum - 1;

	if (cnt == 2) {
		// ÁÂÇ¥°¡ ²ÀÁöÁ¡
		nearSectorList = new std::vector<CSingleSector*>(3);
	}
	else if (cnt == 1) {
		// ÁÂÇ¥°¡ º¯
		nearSectorList = new std::vector<CSingleSector*>(5);
	}
	else {
		// ÁÂÇ¥°¡ ¸Ê ³»ºÎ
		nearSectorList = new std::vector<CSingleSector*>(8);
	}

	int dy[8] = { -1,-1,-1,0,0,1,1,1 };
	int dx[8] = { -1,0,1,-1,1,-1,0,1 };

	int sectorCnt = 0;

	int* dyIter = dy;
	int* dxIter = dx;
	int* dyEnd = dy + 8;

	for (; dyIter != dyEnd; ++dyIter, ++dxIter) {

		int ty = y + *dyIter;
		int tx = x + *dxIter;

		if (cnt == 0 || (ty >= 0 && tx >= 0 && ty < _heightNum && tx < _widthNum)) {

			nearSectorList->at(sectorCnt) = &_sector[ty][tx];
			sectorCnt += 1;

		}

	}

	return nearSectorList;
}

void CSectorList::eraseDuplicatedSector(std::vector<CSingleSector*>* list1, std::vector<CSingleSector*>* list2) {

	for (std::vector<CSingleSector*>::iterator iter = list1->begin(); iter != list1->end();) {

		std::vector<CSingleSector*>::iterator list1Iter = iter;

		std::vector<CSingleSector*>::iterator list2Iter = list2->begin();
		for (; list2Iter != list2->end(); ++list2Iter) {
			if (*list1Iter == *list2Iter) {
				break;
			}
		}

		if (list2Iter == list2->end()) {
			++iter;
			continue;
		}

		list1->erase(list1Iter);
		list2->erase(list2Iter);

	}

}