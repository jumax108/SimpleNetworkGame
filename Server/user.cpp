#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

#include "ringBuffer.h"
#include "log.h"
#include "common.h"
#include "user.h"

constexpr int strLen = 1000;
static wchar_t str[strLen];

constexpr int BUFFER_SIZE = 5000;

stUser::stUser() {

	sock = 0;
	id = 0;
	x = 0;
	y = 0;

	dir = MOVE_DIR::NONE;
	hp = 0;

	seeLeft = false;
	beDisconnect = false;

	recvBuffer = new CRingBuffer(BUFFER_SIZE);
	sendBuffer = new CRingBuffer(BUFFER_SIZE);

}

CUserList::CUserList() {
	_idBase = 1;

}

stUser* CUserList::addUser(SOCKET sock) {

	stUser* newUser = new stUser();
	newUser->sock = sock;
	newUser->id = _idBase++;
	newUser->x = rand() % MAP_WIDTH;
	newUser->y = rand() % MAP_HEIGHT;
	newUser->hp = 100;

	_userListById.insert(std::pair<int, stUser*>(newUser->id, newUser));
	_userListBySocket.insert(std::pair<SOCKET, stUser*>(sock, newUser));

	swprintf_s(str, strLen, L"[USER] add user, socket: %d, id: %d, x: %d, y: %d", (int)sock, newUser->id, newUser->x, newUser->y);
	logger->Logging(str, LOG_NOTICE);

	return newUser;
}

void CUserList::eraseUser(int id) {

	std::unordered_map<int, stUser*>::iterator userIdIter = _userListById.find(id);

	stUser* user = (*userIdIter).second;

	std::unordered_map<SOCKET, stUser*>::iterator userSocketIter = _userListBySocket.find(user->sock);

	swprintf_s(str, strLen, L"[USER] erase user, id: %d", user->id);

	user->id = 0;
	delete(user);

	_userListById.erase(userIdIter);
	_userListBySocket.erase(userSocketIter);

	logger->Logging(str, LOG_NOTICE);

}

void CUserList::eraseUser(SOCKET sock) {

	std::unordered_map<SOCKET, stUser*>::iterator userSocketIter = _userListBySocket.find(sock);

	stUser* user = (*userSocketIter).second;

	std::unordered_map<int, stUser*>::iterator userIdIter = _userListById.find(user->id);

	swprintf_s(str, strLen, L"[USER] erase user, id: %d", user->id);

	user->id = 0;
	delete(user);

	_userListById.erase(userIdIter);
	_userListBySocket.erase(userSocketIter);

	logger->Logging(str, LOG_NOTICE);

}

stUser* CUserList::getUser(SOCKET sock) {

	std::unordered_map<SOCKET, stUser*>::iterator userSocketIter = _userListBySocket.find(sock);
	stUser* user = (*userSocketIter).second;

	if (user->id == 0) {
		return nullptr;
	}

	return user;
}

stUser* CUserList::getUser(int id) {

	std::unordered_map<int, stUser*>::iterator userIdIter = _userListById.find(id);
	stUser* user = (*userIdIter).second;

	if (user->id == 0) {
		return nullptr;
	}

	return user;

}
