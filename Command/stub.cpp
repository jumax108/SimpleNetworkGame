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
#include "proxy.h"
#include "stub.h"

void CStub::CS_TpStub(CProtocolBuffer* buffer, int id, short x, short y) {

	*buffer << (char)0x89;
	*buffer << (char)sizeof(stCS_Tp);
	*buffer << (char)CS_Tp;

	*buffer << id;
	*buffer << x;
	*buffer << y;
}

void CStub::CS_KillStub(CProtocolBuffer* buffer, int id) {

	*buffer << (char)0x89;
	*buffer << (char)sizeof(stCS_KillUser);
	*buffer << (char)CS_KillUser;

	*buffer << id;
}

void CStub::CS_UserListStub(CProtocolBuffer* buffer, int id, char direction, short left, short top, short right, short bottom) {

	*buffer << (char)0x89;
	*buffer << (char)sizeof(stCS_UserList);
	*buffer << (char)CS_UserList;

	*buffer << id;
	*buffer << direction;
	*buffer << left;
	*buffer << top;
	*buffer << right;
	*buffer << bottom;

}