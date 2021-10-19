#include <unordered_map>
#include <vector>
#include <unordered_set>

#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

#include "ringBuffer.h"
#include "log.h"
#include "common.h"
#include "protocolBuffer.h"
#include "sector.h"
#include "user.h"
#include "network.h"
#include "stub.h"

#include "proxy.h"

constexpr int strLen = 1000;
static wchar_t str[strLen];

bool CProxy::CS_MoveStartProxy(stUser* user, CProtocolBuffer* protocolBuffer) {
	
	user->dir = (MOVE_DIR)0;
	protocolBuffer->popData(1, (unsigned char*)(&user->dir));

	int userX;
	int userY;

	*protocolBuffer >> userX;
	*protocolBuffer >> userY;

	{
		// 오차가 크면 싱크를 맞추자
		//syncProc(user, userX, userY);
	}

	userX = user->x;
	userY = user->y;

	{
		// 주변에 이동 시작 뿌리기

		int sectorY;
		int sectorX;
		sectorList->coordinateToIndex(userY, userX, &sectorY, &sectorX);

		std::vector<CSingleSector*>* nearSectorList = sectorList->nearSector(sectorY, sectorX);

		CProtocolBuffer packet(50);
		packetMake->SC_MoveStartStub(&packet, user->id, (char)user->dir, user->x, user->y);

		for (std::vector<CSingleSector*>::iterator sectorIter = nearSectorList->begin(); sectorIter != nearSectorList->end(); ++sectorIter) {
			CSingleSector* sector = *sectorIter;
			network->broadCast<std::unordered_set<int>::iterator, CUserList>(sector->userListBegin(), sector->userListEnd(), nullptr, &packet, *userList);
		}
		
		CSingleSector* userSector = sectorList->getSector(sectorY, sectorX);
		network->broadCast<std::unordered_set<int>::iterator, CUserList>(userSector->userListBegin(), userSector->userListEnd(), user, &packet, *userList);

		if (userViewer != nullptr) {
			network->uniCast(userViewer, &packet);
		}
	}

	swprintf_s(str, strLen, L"[RECV] Move Start, id: %d, x: %d, y: %d", user->id, user->x, user->y);
	logger->Logging(str, LOG_ALL);
	return true;
}

bool CProxy::CS_MoveStopProxy(stUser* user, CProtocolBuffer* protocolBuffer) {

	user->dir = MOVE_DIR::NONE;
	protocolBuffer->moveFront(1);
	//*protocolBuffer >> user->x;
	//*protocolBuffer >> user->y;
	
	{
		// 주변에 이동 종료 뿌리기
		int userX = user->x;
		int userY = user->y;

		int sectorY;
		int sectorX;
		sectorList->coordinateToIndex(userY, userX, &sectorY, &sectorX);

		std::vector<CSingleSector*>* nearSectorList = sectorList->nearSector(sectorY, sectorX);

		CProtocolBuffer packet(50);
		packetMake->SC_MoveStopStub(&packet, user->id, !user->seeLeft * 4, user->x, user->y);

		for (std::vector<CSingleSector*>::iterator sectorIter = nearSectorList->begin(); sectorIter != nearSectorList->end(); ++sectorIter) {
			CSingleSector* sector = *sectorIter;
			network->broadCast<std::unordered_set<int>::iterator, CUserList>(sector->userListBegin(), sector->userListEnd(), nullptr, &packet, *userList);
		}

		CSingleSector* userSector = sectorList->getSector(sectorY, sectorX);
		network->broadCast<std::unordered_set<int>::iterator, CUserList>(userSector->userListBegin(), userSector->userListEnd(), user, &packet, *userList);

		if (userViewer != nullptr) {
			network->uniCast(userViewer, &packet);
		}
	}

	swprintf_s(str, strLen, L"[RECV] Move Stop, id: %d, x: %d, y: %d", user->id, user->x, user->y);
	logger->Logging(str, LOG_ALL);
	return true;
}

bool CProxy::CS_Attack1Proxy(stUser* user, CProtocolBuffer* protocolBuffer) {

	
	user->dir = MOVE_DIR::NONE;

	int userX = user->x;
	int userY = user->y;

	attackProc(CStub::SC_Attack1Stub, ATTACK1_RANGE_X, ATTACK1_RANGE_Y, ATTACK1_DAMAGE, user);

	swprintf_s(str, strLen, L"[RECV] Attack 1, id: %d", user->id);
	logger->Logging(str, LOG_ALL);
	return true;
}

bool CProxy::CS_Attack2Proxy(stUser* user, CProtocolBuffer* protocolBuffer) {

	user->dir = MOVE_DIR::NONE;

	int userX = user->x;
	int userY = user->y;

	attackProc(CStub::SC_Attack2Stub, ATTACK2_RANGE_X, ATTACK2_RANGE_Y, ATTACK2_DAMAGE, user);

	swprintf_s(str, strLen, L"[RECV] Attack 2, id: %d", user->id);
	logger->Logging(str, LOG_ALL);
	return true;
}

bool CProxy::CS_Attack3Proxy(stUser* user, CProtocolBuffer* protocolBuffer) {

	user->dir = MOVE_DIR::NONE;

	int userX = user->x;
	int userY = user->y;

	attackProc(CStub::SC_Attack3Stub, ATTACK3_RANGE_X, ATTACK3_RANGE_Y, ATTACK3_DAMAGE, user);

	swprintf_s(str, strLen, L"[RECV] Attack 3, id: %d", user->id);
	logger->Logging(str, LOG_ALL);

	return true;
}

void CProxy::attackProc(void (*attackStub)(CProtocolBuffer*, int, char, short, short), const int attackRangeX, const int attackRangeY, const int damage, stUser* user) {

	int userY = user->y;
	int userX = user->x;

	int rangeX[4] = { userX, userX, userX, userX };
	int rangeY[4] = { userY - attackRangeY / 2, userY - attackRangeY / 2, userY + attackRangeY / 2, userY + attackRangeY / 2 };

	if (user->seeLeft == true) {
		rangeX[0] -= attackRangeX;
		rangeX[3] -= attackRangeX;
	}
	else {
		rangeX[0] += attackRangeX;
		rangeX[3] += attackRangeX;
	}

	int* rangeXIter = rangeX;
	int* rangeYIter = rangeY;
	int* rangeXEnd = rangeX + 4;

	std::unordered_set<CSingleSector*> attackSectorList;
	for (; rangeXIter != rangeXEnd; ++rangeXIter, ++rangeYIter) {

		*rangeXIter = max(min(*rangeXIter, MAP_WIDTH), 0);
		*rangeYIter = max(min(*rangeYIter, MAP_HEIGHT), 0);

		int sectorX;
		int sectorY;
		sectorList->coordinateToIndex(*rangeYIter, *rangeXIter, &sectorY, &sectorX);
		attackSectorList.insert(sectorList->getSector(sectorY, sectorX));
	}

	{
		// 주변에 공격 시작 알림
		CProtocolBuffer packet(50);
		attackStub(&packet, user->id, (char)!user->seeLeft * 4, user->x, user->y);

		std::vector<CSingleSector*>* nearSectorList = sectorList->nearSector(user->sectorY, user->sectorX);
		for (std::vector<CSingleSector*>::iterator sectorIter = nearSectorList->begin(); sectorIter != nearSectorList->end(); ++sectorIter) {
			CSingleSector* sector = *sectorIter;
			network->broadCast<std::unordered_set<int>::iterator, CUserList>(sector->userListBegin(), sector->userListEnd(), nullptr, &packet, *userList);
		}

		CSingleSector* sector = sectorList->getSector(user->sectorY, user->sectorX);
		network->broadCast<std::unordered_set<int>::iterator, CUserList>(sector->userListBegin(), sector->userListEnd(), user, &packet, *userList);

		if (userViewer != nullptr) {
			network->uniCast(userViewer, &packet);
		}
	}

	int left;
	int top;
	int right;
	int bottom;

	if (user->seeLeft == true) {
		left = rangeX[0];
		top = rangeY[0];
		right = rangeX[2];
		bottom = rangeY[2];
	}
	else {
		left = rangeX[1];
		top = rangeY[1];
		right = rangeX[3];
		bottom = rangeY[3];
	}

	std::vector<stUser*> damagedUserList;
	{
		// 공격 받을 유저 검색
		for (std::unordered_set<CSingleSector*>::iterator sectorIter = attackSectorList.begin(); sectorIter != attackSectorList.end(); ++sectorIter) {
			CSingleSector* sector = *sectorIter;
			for (std::unordered_set<int>::iterator userIdIter = sector->userListBegin(); userIdIter != sector->userListEnd(); ++userIdIter) {
				int id = *userIdIter;
				stUser* attackedUser = userList->getUser(id);

				if (user == attackedUser) {
					continue;
				}

				int x = attackedUser->x;
				int y = attackedUser->y;
				if (left <= x && x <= right && top <= y && y <= bottom) {
					damagedUserList.push_back(attackedUser);
				}
			}
		}
	}

	{
		// 피해자 주변 유저에게 대미지 판정 전달
		for (std::vector<stUser*>::iterator userIter = damagedUserList.begin(); userIter != damagedUserList.end(); ++userIter) {

			stUser* damagedUser = *userIter;
			damagedUser->hp -= ATTACK1_DAMAGE;

			CProtocolBuffer packet(50);
			packetMake->SC_DamageStub(&packet, user->id, damagedUser->id, damagedUser->hp);


			std::vector<CSingleSector*>* dmgUserNearSectorList = sectorList->nearSector(damagedUser->sectorY, damagedUser->sectorX);
			for (std::vector<CSingleSector*>::iterator sectorIter = dmgUserNearSectorList->begin(); sectorIter != dmgUserNearSectorList->end(); ++sectorIter) {
				CSingleSector* sector = *sectorIter;
				network->broadCast<std::unordered_set<int>::iterator, CUserList>(sector->userListBegin(), sector->userListEnd(), nullptr, &packet, *userList);
			}

			CSingleSector* sector = sectorList->getSector(damagedUser->sectorY, damagedUser->sectorX);
			network->broadCast<std::unordered_set<int>::iterator, CUserList>(sector->userListBegin(), sector->userListEnd(), nullptr, &packet, *userList);

			if (damagedUser->hp <= 0) {
				damagedUser->beDisconnect = true;
			}

			if (userViewer != nullptr) {
				network->uniCast(userViewer, &packet);
			}
		}
	}

}

void CProxy::syncProc(stUser* user, short x, short y) {

	int userX = user->x;
	int userY = user->y;
	if (abs(userX - x) < COORDINATE_ERROR_RANGE && abs(userY - y) < COORDINATE_ERROR_RANGE) {
		return;
	}

	int userId = user->id;

	CProtocolBuffer packet(50);
	packetMake->SC_SyncStub(&packet, userId, userX, userY);
	network->uniCast(user, &packet);

	if (userViewer != nullptr) {
		network->uniCast(userViewer, &packet);
	}

}

bool CProxy::CS_TpProxy(stUser* user, CProtocolBuffer* protocolBuffer) {

	int id;
	short x;
	short y;

	*protocolBuffer >> id;
	*protocolBuffer >> x;
	*protocolBuffer >> y;

	CProtocolBuffer packet(50);
	packetMake->SC_SyncStub(&packet, id, x, y);

	stUser* destUser = userList->getUser(id);
	destUser->y = y;
	destUser->x = x;
	network->uniCast(destUser, &packet);

	if (userViewer != nullptr) {
		network->uniCast(userViewer, &packet);
	}

	// 강제 이동해서 섹터 변경될 수 있음
	sectorLogic(destUser);

	return true;
}

bool CProxy::CS_KillUserProxy(stUser* user, CProtocolBuffer* protocolBuffer) {
	int id;
	*protocolBuffer >> id;
	return true;
}

bool CProxy::CS_UserListProxy(stUser* user, CProtocolBuffer* protocolBuffer) {

	int id;
	char direction;
	short left;
	short top;
	short right;
	short bottom;

	*protocolBuffer >> id;
	*protocolBuffer >> direction;
	*protocolBuffer >> left;
	*protocolBuffer >> top;
	*protocolBuffer >> right;
	*protocolBuffer >> bottom;

	std::vector<stUser*> correctUser;

	for (std::unordered_map<int, stUser*>::iterator userIter = userList->idListBegin(); userIter != userList->idListEnd(); ++userIter) {
		stUser* user = userIter->second;

		int conditionNum = 0;

		if (id == -100 || id == user->id) {
			conditionNum += 1;
		}

		if (direction == -100 || direction == (char)user->dir) {
			conditionNum += 1;
		}

		if (left == -100 || (left <= user->x && user->x < right && top <= user->y && user->y < bottom) ) {
			conditionNum += 1;
		}

		if (conditionNum == 3) {
			correctUser.push_back(user);
		}

	}

	int arrSize = correctUser.size();
	int* idArr = new int[arrSize];
	char* dirArr = new char[arrSize];
	short* xArr = new short[arrSize];
	short* yArr = new short[arrSize];

	int idx = 0;
	for (std::vector<stUser*>::iterator userIter = correctUser.begin(); userIter != correctUser.end(); ++userIter) {
		stUser* user = *userIter;
		idArr[idx] = user->id;
		dirArr[idx] = (char)user->dir;
		xArr[idx] = user->x;
		yArr[idx] = user->y;
		++idx;
	}

	CProtocolBuffer packet(1000);
	packetMake->SC_UserListStub(&packet, arrSize, idArr, dirArr, xArr, yArr);
	network->uniCast(user, &packet);

	delete[] idArr;
	delete[] dirArr;
	delete[] xArr;
	delete[] yArr;

	return true;

}

bool CProxy::CS_RegistUserViewerProxy(stUser* user, CProtocolBuffer* protocolBuffer) {

	userViewer = user;

	// 모든 캐릭터 재생성
	for (std::unordered_map<int, stUser*>::iterator userIter = userList->idListBegin(); userIter != userList->idListEnd(); ++userIter) {

		int id = userIter->first;
		stUser* user = userIter->second;

		{
			CProtocolBuffer packet(50);
			packetMake->SC_DeleteCharacterStub(&packet, id);
			network->uniCast(userViewer, &packet);
		}

		{
			
			CProtocolBuffer packet(50);
			packetMake->SC_CreateOtherCharacterStub(&packet, id, (char)user->dir, user->x, user->y, user->hp);
			network->uniCast(userViewer, &packet);

		}
	}


	// 이후에는 모든 변경 메시지를 전달
	return true;
}