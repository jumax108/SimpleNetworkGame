#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

#include <list>
#include <unordered_map>
#include <unordered_set>
#include <stdio.h>
#include <Windows.h>
#include <time.h>
#include <string.h>

#include "common.h"
#include "ringBuffer.h"
#include "protocolBuffer.h"
#include "log.h" 
#include "user.h"
#include "proxy.h"
#include "network.h"
#include "stub.h"

constexpr int strLen = 1000;
static wchar_t str[strLen];

void CStub::SC_CreateMyCharacterStub(CProtocolBuffer* buffer, int id, char direction, short x, short y, char hp)
{

	*buffer << (char)0x89;
	*buffer << (char)sizeof(stSC_CreateMyCharacter);
	*buffer << (char)SC_CreateMyCharacter;

	*buffer << id;
	*buffer << direction;
	*buffer << x;
	*buffer << y;
	*buffer << hp;

}

void CStub::SC_CreateOtherCharacterStub(CProtocolBuffer* buffer, int id, char direction, short x, short y, char hp)
{

	*buffer << (char)0x89;
	*buffer << (char)sizeof(stSC_CreateOtherCharacter);
	*buffer << (char)SC_CreateOtherCharacter;

	*buffer << id;
	*buffer << direction;
	*buffer << x;
	*buffer << y;
	*buffer << hp;

}

void CStub::SC_DeleteCharacterStub(CProtocolBuffer* buffer, int id)
{
	*buffer << (char)0x89;
	*buffer << (char)sizeof(stSC_DeleteCharacter);
	*buffer << (char)SC_DeleteCharacter;

	*buffer << id;

}

void CStub::SC_MoveStartStub(CProtocolBuffer* buffer, int id, char direction, short x, short y)
{

	*buffer << (char)0x89;
	*buffer << (char)sizeof(stSC_MoveStart);
	*buffer << (char)SC_MoveStart;

	*buffer << id;
	*buffer << direction;
	*buffer << x;
	*buffer << y;

}

void CStub::SC_MoveStopStub(CProtocolBuffer* buffer, int id, char direction, short x, short y)
{

	*buffer << (char)0x89;
	*buffer << (char)sizeof(stSC_MoveStop);
	*buffer << (char)SC_MoveStop;

	*buffer << id;
	*buffer << direction;
	*buffer << x;
	*buffer << y;

}

void CStub::SC_Attack1Stub(CProtocolBuffer* buffer, int id, char direction, short x, short y)
{

	*buffer << (char)0x89;
	*buffer << (char)sizeof(stSC_Attack1);
	*buffer << (char)SC_Attack1;

	*buffer << id;
	*buffer << direction;
	*buffer << x;
	*buffer << y;

}

void CStub::SC_Attack2Stub(CProtocolBuffer* buffer, int id, char direction, short x, short y)
{

	*buffer << (char)0x89;
	*buffer << (char)sizeof(stSC_Attack2);
	*buffer << (char)SC_Attack2;

	*buffer << id;
	*buffer << direction;
	*buffer << x;
	*buffer << y;

}

void CStub::SC_Attack3Stub(CProtocolBuffer* buffer, int id, char direction, short x, short y)
{

	*buffer << (char)0x89;
	*buffer << (char)sizeof(stSC_Attack3);
	*buffer << (char)SC_Attack3;

	*buffer << id;
	*buffer << direction;
	*buffer << x;
	*buffer << y;

}

void CStub::SC_DamageStub(CProtocolBuffer* buffer, int attackId, int damageId, char damageHp)
{

	*buffer << (char)0x89;
	*buffer << (char)sizeof(stSC_Damage);
	*buffer << (char)SC_Damage;

	*buffer << attackId;
	*buffer << damageId;
	*buffer << damageHp;

}

void CStub::SC_SyncStub(CProtocolBuffer* buffer, int id, short x, short y){

	*buffer << (char)0x89;
	*buffer << (char)sizeof(stSC_Sync);
	*buffer << (char)SC_Sync;

	*buffer << id;
	*buffer << x;
	*buffer << y;

}

void CStub::SC_UserListStub(CProtocolBuffer* buffer, int num, int* idArr, char* dirArr, short* xArr, short* yArr) {

	*buffer << (char)0x89;
	*buffer << (char)(sizeof(int) + (sizeof(int) + sizeof(char) + sizeof(short) + sizeof(short)) * num);
	*buffer << (char)SC_UserList;

	*buffer << num;
	for (int idx = 0; idx < num; ++idx) {
		*buffer << idArr[idx];
		*buffer << dirArr[idx];
		*buffer << xArr[idx];
		*buffer << yArr[idx];
	}

}