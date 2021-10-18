#pragma once

struct stUser;
class CUserList;
class CProxy;
class CSectorList;

extern CLogger* logger;
extern CUserList* userList;
extern CProxy* proxy;
extern CSectorList* sectorList;

class CNetwork {

public:

	bool init();

	SOCKET acceptUser();

	void recvPacket();
	void sendPacket();

	void packetProcess(stUser* user);

	void uniCast(stUser* user, CProtocolBuffer* buffer);

	template<typename ITERATOR_TYPE, typename GET_USER_FUNCTOR_TYPE>
	__declspec(noinline) void broadCast(ITERATOR_TYPE beginIter, ITERATOR_TYPE endIter, stUser* exceptUser, CProtocolBuffer* buffer, GET_USER_FUNCTOR_TYPE& getUserFunctor) {

		for (ITERATOR_TYPE iter = beginIter; iter != endIter; ++iter) {

			stUser* user = getUserFunctor(iter);
			if (user == exceptUser) {
				continue;
			}

			uniCast(user, buffer);

		}

	}

	void disconnectUser(stUser* user);

private:

	SOCKET listenSocket;

	const wchar_t* SERVER_IP = L"0.0.0.0";
	const unsigned short SERVER_PORT = 20000;

};