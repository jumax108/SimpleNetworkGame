
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

#include <process.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <Windows.h>

#include <vector>
#include <unordered_set>
#include <unordered_map>

#include "protocolBuffer.h"
#include "common.h"
#include "log.h"
#include "network.h"
#include "user.h" 
#include "proxy.h"
#include "sector.h"
#include "stub.h"

CLogger* logger;
CNetwork* network;
CUserList* userList;
CProxy* proxy;
CStub* packetMake;
CSectorList* sectorList;

void logic();
void enterNewUser(SOCKET sock);

int logicCnt = 0;

void printFps(void*) {

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);
	freq.QuadPart /= 1000000;

	LARGE_INTEGER startTime;
	LARGE_INTEGER endTime;
	LARGE_INTEGER oldStartTime;

	QueryPerformanceCounter(&startTime);
	startTime.QuadPart /= freq.QuadPart;
	oldStartTime = startTime;

	__int64 delay = 0;

	while (1) {

		printf("fps: %d\n", logicCnt);
		logicCnt = 0;

		QueryPerformanceCounter(&endTime);
		QueryPerformanceCounter(&startTime);

		endTime.QuadPart /= freq.QuadPart;
		startTime.QuadPart /= freq.QuadPart;
		
		delay += (1000 * 1000) - (endTime.QuadPart - oldStartTime.QuadPart);
		oldStartTime = startTime;

		if (delay > 0) {
			int delayTime = delay / 1000;
			delay %= 1000;
			Sleep(delayTime);
			oldStartTime.QuadPart += (long long)delayTime * 1000;
		}

	}
	
}

int main() {

	HANDLE threadHandle = (HANDLE)_beginthread(printFps, 0, nullptr);

	LARGE_INTEGER freq;
	QueryPerformanceFrequency(&freq);

	freq.QuadPart /= 1000000; // us�� ���

	__int64 delay = 0;

	LARGE_INTEGER startTime;
	LARGE_INTEGER oldStartTime;
	LARGE_INTEGER endTime;

	logger = new CLogger(LOG_ALL);
	network = new CNetwork();
	userList = new CUserList();
	proxy = new CProxy();
	packetMake = new CStub();
	sectorList = new CSectorList(MAP_WIDTH, MAP_HEIGHT, SECTOR_WIDTH, SECTOR_HEIGHT);

	bool networkInitResult = network->init();
	if (networkInitResult == false) {
		logger->Logging(L"[NETWORK] network init fail", LOG_CRITICAL);
		return -1;
	}
	logger->Logging(L"[NETWORK] network init success", LOG_NOTICE);

	timeBeginPeriod(1);

	QueryPerformanceCounter(&startTime);
	startTime.QuadPart /= freq.QuadPart;
	oldStartTime = startTime;
	while (1) {

		{
			// ���� �õ� ���� üũ
			SOCKET sock = network->acceptUser();
			if (sock != -1) {
				// ���� Ȯ�� ��, ���� ���� ó��
				enterNewUser(sock);
			}
		}

		{
			// ���� ��Ŷ �ޱ�
			network->recvPacket();
		}

		{
			// ���� ����
			logic();
			logicCnt += 1;
		}

		{
			// ���� ��Ŷ ������
			network->sendPacket();
		}

		{
			// ���� ���� ���� ó��

			for (std::unordered_map<int, stUser*>::iterator userIter = userList->idListBegin(); userIter != userList->idListEnd(); ++userIter){
				stUser* user = userIter->second;
				if (user->beDisconnect == true) {
					network->disconnectUser(user);
				}
			}

		}

		{
			// ������ ����
			//Sleep(20);
			//printf("delay: %lld\n", delay);

			QueryPerformanceCounter(&endTime);
			QueryPerformanceCounter(&startTime);
			endTime.QuadPart /= freq.QuadPart;
			startTime.QuadPart /= freq.QuadPart;
			delay += (20000 - (endTime.QuadPart - oldStartTime.QuadPart));
			oldStartTime = startTime;
			if (delay > 0) {
				int delayTime = delay / 1000;
				delay %= 1000;
				Sleep(delayTime);
				oldStartTime.QuadPart += (long long)delayTime * 1000;
			}
		}

	}

	return 0;

}

void logic() {

	for (std::unordered_map<int, stUser*>::iterator userIter = userList->idListBegin(); userIter != userList->idListEnd(); ++userIter) {

		stUser* user = userIter->second;

		//printf("y: %d, x: %d, dir: %d\n", user->y, user->x, user->dir);
		switch (user->dir) {
			case MOVE_DIR::NONE: {
				break;
			}
			case MOVE_DIR::MOVE_DIR_LL: {
				if (user->x >= MOVE_X_SPEED) {
					user->x -= MOVE_X_SPEED;
					user->seeLeft = true;
				}
				break;
			}
			case MOVE_DIR::MOVE_DIR_LU: {
				if (user->x >= MOVE_X_SPEED && user->y >= MOVE_Y_SPEED) {
					user->x -= MOVE_X_SPEED;
					user->y -= MOVE_Y_SPEED;
					user->seeLeft = true;
				}
				break;
			}
			case MOVE_DIR::MOVE_DIR_UU: {
				if (user->y >= MOVE_Y_SPEED) {
					user->y -= MOVE_Y_SPEED;
				}
				break;
			}
			case MOVE_DIR::MOVE_DIR_RU: {
				if (user->y >= MOVE_Y_SPEED && user->x < MAP_WIDTH - MOVE_X_SPEED) {
					user->y -= MOVE_Y_SPEED;
					user->x += MOVE_X_SPEED;
					user->seeLeft = false;
				}
				break;
			}
			case MOVE_DIR::MOVE_DIR_RR: {
				if (user->x < MAP_WIDTH - MOVE_X_SPEED) {
					user->x += MOVE_X_SPEED;
					user->seeLeft = false;
				}
				break;
			}
			case MOVE_DIR::MOVE_DIR_RD: {
				if (user->x < MAP_WIDTH - MOVE_X_SPEED && user->y < MAP_HEIGHT - MOVE_Y_SPEED) {
					user->x += MOVE_X_SPEED;
					user->y += MOVE_Y_SPEED;
					user->seeLeft = false;
				}
				break;
			}
			case MOVE_DIR::MOVE_DIR_DD: {
				if (user->y < MAP_HEIGHT - MOVE_Y_SPEED) {
					user->y += MOVE_Y_SPEED;
				}
				break;
			}
			case MOVE_DIR::MOVE_DIR_LD: {
				if (user->y < MAP_HEIGHT - MOVE_Y_SPEED && user->x >= MOVE_X_SPEED) {
					user->y += MOVE_Y_SPEED;
					user->x -= MOVE_X_SPEED;
					user->seeLeft = true;
				}
				break;
			}
		}

		sectorLogic(user);

	}

}

void sectorLogic(stUser* user) {
	int sectorY;
	int sectorX;

	sectorList->coordinateToIndex(user->y, user->x, &sectorY, &sectorX);

	int userSectorY = user->sectorY;
	int userSectorX = user->sectorX;

	if (sectorY != userSectorY || sectorX != userSectorX) {

		// ���� �����

		CSingleSector* oldUserSector = sectorList->getSector(userSectorY, userSectorX);
		CSingleSector* newUserSector = sectorList->getSector(sectorY, sectorX);

		oldUserSector->eraseUser(user->id);
		newUserSector->addUser(user->id);

		std::vector<CSingleSector*>* oldNearSectorList = sectorList->nearSector(userSectorY, userSectorX);
		std::vector<CSingleSector*>* newNearSectorList = sectorList->nearSector(sectorY, sectorX);

		oldNearSectorList->push_back(oldUserSector);
		newNearSectorList->push_back(newUserSector);

		CSectorList::eraseDuplicatedSector(oldNearSectorList, newNearSectorList);

		{
			// ���� ���� �������� ĳ���� ���� �޽��� ����

			CProtocolBuffer packet(50);
			packetMake->SC_DeleteCharacterStub(&packet, user->id);

			for (std::vector<CSingleSector*>::iterator sectorIter = oldNearSectorList->begin(); sectorIter != oldNearSectorList->end(); ++sectorIter) {
				CSingleSector* sector = *sectorIter;
				network->broadCast<std::unordered_set<int>::iterator, CUserList>(sector->userListBegin(), sector->userListEnd(), user, &packet, *userList);
			}
		}

		{
			// ���� ���� ������ ������ �����Լ� �����
			for (std::vector<CSingleSector*>::iterator sectorIter = oldNearSectorList->begin(); sectorIter != oldNearSectorList->end(); ++sectorIter) {
				CSingleSector* sector = *sectorIter;

				for (std::unordered_set<int>::iterator userIter = sector->userListBegin(); userIter != sector->userListEnd(); ++userIter) {
					int id = *userIter;
					stUser* otherUser = userList->getUser(id);

					CProtocolBuffer packet(50);
					packetMake->SC_DeleteCharacterStub(&packet, id);

					network->uniCast(user, &packet);

				}

			}
		}

		{
			// ���ο� ���� �������� ĳ���� ���� �޽��� ����

			CProtocolBuffer packet(50);
			packetMake->SC_CreateOtherCharacterStub(&packet, user->id, (char)user->dir, user->x, user->y, user->hp);

			for (std::vector<CSingleSector*>::iterator sectorIter = newNearSectorList->begin(); sectorIter != newNearSectorList->end(); ++sectorIter) {
				CSingleSector* sector = *sectorIter;
				network->broadCast<std::unordered_set<int>::iterator, CUserList>(sector->userListBegin(), sector->userListEnd(), user, &packet, *userList);
			}
		}

		{
			// ���ο� ���Ϳ� �ִ� ���� ���� ������, �������� �����ֱ�

			for (std::vector<CSingleSector*>::iterator sectorIter = newNearSectorList->begin(); sectorIter != newNearSectorList->end(); ++sectorIter) {
				CSingleSector* sector = *sectorIter;

				for (std::unordered_set<int>::iterator userIter = sector->userListBegin(); userIter != sector->userListEnd(); ++userIter) {
					int id = *userIter;

					if (id == user->id) {
						continue;
					}

					stUser* otherUser = userList->getUser(id);

					CProtocolBuffer packet(50);
					packetMake->SC_CreateOtherCharacterStub(&packet, id, (char)otherUser->dir, otherUser->x, otherUser->y, otherUser->hp);

					network->uniCast(user, &packet);

				}

			}
		}

		{
			// ���ο� ���Ϳ� �ִ� ���� ������ �����̰� ������ �����̰� �ϱ�
			for (std::vector<CSingleSector*>::iterator sectorIter = newNearSectorList->begin(); sectorIter != newNearSectorList->end(); ++sectorIter) {
				CSingleSector* sector = *sectorIter;

				for (std::unordered_set<int>::iterator userIter = sector->userListBegin(); userIter != sector->userListEnd(); ++userIter) {
					int id = *userIter;
					stUser* otherUser = userList->getUser(id);

					if (otherUser->dir == MOVE_DIR::NONE) {
						continue;
					}

					CProtocolBuffer packet(50);
					packetMake->SC_MoveStartStub(&packet, id, (char)otherUser->dir, otherUser->x, otherUser->y);

					network->uniCast(user, &packet);

				}

			}
		}

		do {
			// ���� �����̰� ������ ���ο� ���Ϳ� �ִ� �����鿡�Ե� �������� ���̰� �ϱ�

			if (user->dir == MOVE_DIR::NONE) {
				break;
			}

			CProtocolBuffer packet(50);
			packetMake->SC_MoveStartStub(&packet, user->id, (char)user->dir, user->x, user->y);

			for (std::vector<CSingleSector*>::iterator sectorIter = newNearSectorList->begin(); sectorIter != newNearSectorList->end(); ++sectorIter) {
				CSingleSector* sector = *sectorIter;
				network->broadCast<std::unordered_set<int>::iterator, CUserList>(sector->userListBegin(), sector->userListEnd(), user, &packet, *userList);
			}

		} while (false);

		user->sectorY = sectorY;
		user->sectorX = sectorX;
	}
}

void enterNewUser(SOCKET sock) {

	CProtocolBuffer createMyCharacterPacket(50);
	stUser* user = userList->addUser(sock);
	{
		// ����� �������� ĳ���� ���� �޽��� ����
		packetMake->SC_CreateMyCharacterStub(&createMyCharacterPacket, user->id, user->seeLeft * 4, user->x, user->y, user->hp);
		network->uniCast(user, &createMyCharacterPacket);
	}


	int sectorY, sectorX;
	{
		// ���Ϳ� �߰��ϱ�
		sectorList->coordinateToIndex(user->y, user->x, &sectorY, &sectorX);
		sectorList->addUser(sectorY, sectorX, user->id);
		user->sectorX = sectorX;
		user->sectorY = sectorY;
	}

	CProtocolBuffer createOtherCharacterPacket(50);
	std::vector<CSingleSector*>* nearSectorList = sectorList->nearSector(sectorY, sectorX);
	{
		// �ֺ� ���� �������� ĳ���� ���� �޽��� ����
		packetMake->SC_CreateOtherCharacterStub(&createOtherCharacterPacket, user->id, (char)user->dir, user->x, user->y, user->hp);

		for (std::vector<CSingleSector*>::iterator sectorIter = nearSectorList->begin(); sectorIter != nearSectorList->end(); ++sectorIter) {

			CSingleSector* sector = *sectorIter;
			network->broadCast<std::unordered_set<int>::iterator, CUserList>(sector->userListBegin(), sector->userListEnd(), nullptr, &createOtherCharacterPacket, *userList);

		}

	}

	{
		// �� ���� �������� ĳ���� ���� �޽��� ����
		CSingleSector* sector = sectorList->getSector(sectorY, sectorX);
		network->broadCast<std::unordered_set<int>::iterator, CUserList>(sector->userListBegin(), sector->userListEnd(), user, &createOtherCharacterPacket, *userList);
	}

}