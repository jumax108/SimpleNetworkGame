#pragma once

extern CLogger* logger;
class CRingBuffer;

struct stUser {

	SOCKET sock;

	unsigned short x;
	unsigned short y;
	int id;
	char hp;

	bool seeLeft;

	int sectorY;
	int sectorX;

	CRingBuffer* sendBuffer;
	CRingBuffer* recvBuffer;

	MOVE_DIR dir;

	bool beDisconnect;

	stUser();

};

class CUserList {

public:

	CUserList();

	stUser* addUser(SOCKET sock);
	void eraseUser(int id);
	void eraseUser(SOCKET sock);

	stUser* getUser(SOCKET sock);
	stUser* getUser(int id);

	inline stUser* operator()(std::unordered_set<int>::iterator& iter) {
		return getUser(*iter);
	}

	std::unordered_map<SOCKET, stUser*>::iterator socketListBegin() { return _userListBySocket.begin(); }
	std::unordered_map<SOCKET, stUser*>::iterator socketListEnd() { return _userListBySocket.end(); }

	std::unordered_map<int, stUser*>::iterator idListBegin() { return _userListById.begin(); }
	std::unordered_map<int, stUser*>::iterator idListEnd() { return _userListById.end(); }

private:

	int _idBase;

	std::unordered_map<int, stUser*> _userListById;
	std::unordered_map<SOCKET, stUser*> _userListBySocket;

};