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
#include <vector>

#include "common.h"
#include "protocolBuffer.h"
#include "ringBuffer.h"
#include "log.h" 
#include "user.h"
#include "proxy.h"
#include "sector.h"
#include "network.h"

constexpr int strLen = 1000;
static wchar_t str[strLen];

bool CNetwork::init() {

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		swprintf(str, strLen, L"[NETWORK] WSA Start Up Error, errorCode: %d", WSAGetLastError());
		logger->Logging(str, LOG_CRITICAL);
		return false;
	}
	logger->Logging(L"[NETWORK] WSA Start Up Success", LOG_NOTICE);

	listenSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (listenSocket == INVALID_SOCKET) {

		swprintf_s(str, strLen, L"[NETWORK] Listen Socket Init Error, errorCode: %d", WSAGetLastError());
		logger->Logging(str, LOG_CRITICAL);
		return false;
	}
	logger->Logging(L"[NETWORK] Listen Socket Init", LOG_NOTICE);

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	InetPtonW(AF_INET, SERVER_IP, &addr.sin_addr.S_un.S_addr);
	addr.sin_port = htons(SERVER_PORT);

	int bindResult = bind(listenSocket, (SOCKADDR*)&addr, sizeof(SOCKADDR_IN));
	if (bindResult == SOCKET_ERROR) {
		swprintf_s(str, strLen, L"[NETWORK] Bind Error, errorCode: %d", WSAGetLastError());
		logger->Logging(str, LOG_CRITICAL);
		return false;
	}
	logger->Logging(L"[NETWORK] Bind Complete", LOG_NOTICE);

	int listenResult = listen(listenSocket, SOMAXCONN);
	if (listenResult == SOCKET_ERROR) {
		swprintf_s(str, strLen, L"[NETWORK] Listen Error, errorCode: %d", WSAGetLastError());
		logger->Logging(str, LOG_CRITICAL);
		return false;
	}
	logger->Logging(L"[NETWORK] Listen Success", LOG_NOTICE);

	unsigned long on = 1;
	int ioctlError = ioctlsocket(listenSocket, FIONBIO, &on);
	if (ioctlError == SOCKET_ERROR) {
		swprintf_s(str, strLen, L"[NETWORK] ioctl Error, errorCode: %d", WSAGetLastError());
		logger->Logging(str, LOG_CRITICAL);
		return false;
	}
	logger->Logging(L"[NETWORK] Set I/O Mode", LOG_NOTICE);

	linger rst;
	rst.l_linger = 0;
	rst.l_onoff = 1;
	int lingerResult = setsockopt(listenSocket, SOL_SOCKET, SO_LINGER, (const char*)&rst, sizeof(linger));
	if (lingerResult == SOCKET_ERROR) {
		swprintf_s(str, strLen, L"[NETWORK] linger Error, errorCode: %d", WSAGetLastError());
		logger->Logging(str, LOG_CRITICAL);
		return false;
	}
	logger->Logging(L"[NETWORK] Set Linger", LOG_NOTICE);

	logger->Logging(L"[NETWORK] Init Success", LOG_NOTICE);

	return true;
}

SOCKET CNetwork::acceptUser() {

	SOCKADDR_IN addr;
	int addrLen = sizeof(SOCKADDR_IN);
	SOCKET sock = accept(listenSocket, (SOCKADDR*)&addr, &addrLen);
	if (sock == SOCKET_ERROR) {
		int error = WSAGetLastError();
		if (error != WSAEWOULDBLOCK) {
			swprintf_s(str, strLen, L"[NETWORK] Accept Error, errorCode: %d", error);
			logger->Logging(str, LOG_CRITICAL);
		}
		return -1;
	}

	constexpr int ipBufLen = 20;
	wchar_t ipBuf[ipBufLen] = {0,};
	InetNtopW(AF_INET, &addr.sin_addr.S_un.S_addr, ipBuf, 20);
	swprintf_s(str, strLen, L"[NETWORK] Accept User, socket: %d, ip: %s, port: %d", (int)sock, ipBuf, addr.sin_port);
	logger->Logging(str, LOG_NOTICE);

	return sock;
}

timeval delayZero;

void CNetwork::sendPacket() {

	FD_SET set;
	FD_ZERO(&set);

	for (std::unordered_map<SOCKET, stUser*>::iterator socketIter = userList->socketListBegin(); socketIter != userList->socketListEnd(); ) {

		stUser* user = socketIter->second;
		++socketIter;

		if (user->sendBuffer->getUsedSize() != 0) {
			FD_SET(user->sock, &set);
		}

		if (set.fd_count < 64 && socketIter != userList->socketListEnd()) {
			continue;
		}

		int socketNum = select(0, nullptr, &set, nullptr, &delayZero);

		for (int socketCnt = 0; socketCnt < socketNum; ++socketCnt) {
			SOCKET sock = set.fd_array[socketCnt];
			stUser* user = userList->getUser(sock);
			CRingBuffer* sendBuffer = user->sendBuffer;

			int sendError;
			int sendResult;

			do {
				sendResult = send(sock, sendBuffer->getDirectFront(), sendBuffer->getDirectUsedSize(), 0);
				if (sendResult == SOCKET_ERROR) {
					sendError = WSAGetLastError();
					if (sendError != WSAEWOULDBLOCK) {
						// disconnect
						swprintf_s(str, strLen, L"[NETWORK] Send Error, errorCode: %d, id: %d, socket: %d", sendError, user->id, (int)sock);
						logger->Logging(str, LOG_NOTICE);
					}
					break;
				}
				sendBuffer->moveFront(sendResult);
			} while (sendBuffer->getUsedSize() > 0);
		}

		FD_ZERO(&set);
	}

}

void CNetwork::recvPacket() {

	FD_SET set;
	FD_ZERO(&set);

	for (std::unordered_map<SOCKET, stUser*>::iterator socketIter = userList->socketListBegin(); socketIter != userList->socketListEnd(); ) {

		FD_SET(socketIter->first, &set);
		++socketIter;

		if (set.fd_count < 64 && socketIter != userList->socketListEnd()) {
			continue;
		}

		int socketNum = select(0, &set, nullptr, nullptr, &delayZero);

		for (int socketCnt = 0; socketCnt < socketNum; ++socketCnt) {
			SOCKET sock = set.fd_array[socketCnt];
			stUser* user = userList->getUser(sock);
			CRingBuffer* recvBuffer = user->recvBuffer;

			int recvError;
			int recvResult;

			do{
				recvResult = recv(sock, recvBuffer->getDirectPush(), recvBuffer->getDirectFreeSize(), 0);
				if (recvResult == SOCKET_ERROR) {
					recvError = WSAGetLastError();
					if (recvError != WSAEWOULDBLOCK) {
						// disconnect
						user->beDisconnect = true;
						swprintf_s(str, strLen, L"[NETWORK] Recv Error, errorCode: %d, id: %d, socket: %d", recvError, user->id, (int)sock);
						logger->Logging(str, LOG_NOTICE);
						// 어짜피 끊을 유저이기 때문에 패킷 처리하지 않기 위함
						goto PACKET_PROCESS_SKIP;
					}

					break;
				}
				else if (recvResult == 0) {
					// fin
					// disconnect
					user->beDisconnect = true;
					swprintf_s(str, strLen, L"[NETWORK] Recv Fin, id: %d, socket: %d", user->id, (int)sock);
					logger->Logging(str, LOG_NOTICE);
					// 어짜피 끊을 유저이기 때문에 패킷 처리하지 않기 위함
					goto PACKET_PROCESS_SKIP;

				}
				
				recvBuffer->moveRear(recvResult);
			} while (recvResult > 0);


			packetProcess(user);

			// 어짜피 끊을 유저이기 때문에 패킷 처리하지 않기 위함
			PACKET_PROCESS_SKIP: ;
		}


		FD_ZERO(&set);
	}

}

void CNetwork::packetProcess(stUser* user) {

	CRingBuffer* recvBuffer = user->recvBuffer;

	while (recvBuffer->getUsedSize() > sizeof(stHeader)) {

		stHeader header;
		recvBuffer->front(sizeof(stHeader), (char*)&header);

		if ((unsigned char)header.code != 0x89) {
			// disconnect
			swprintf_s(str, strLen, L"[NETWORK] Packet Header Error, recvHeader: 0x%x, userId: %d", header.code, user->id);
			logger->Logging(str, LOG_NOTICE);
			user->beDisconnect = true;
			return;
		}

		int payloadSize = header.payloadSize;

		if (recvBuffer->getUsedSize() < sizeof(stHeader) + payloadSize) {
			// 메시지가 아직 완성되어있지 않으면
			return;
		}

		recvBuffer->moveFront(sizeof(stHeader));

		CProtocolBuffer protocolBuffer(payloadSize);

		recvBuffer->front(payloadSize, protocolBuffer.getRearPtr());
		recvBuffer->moveFront(payloadSize);
		protocolBuffer.moveRear(payloadSize);

		switch (header.payloadType) {
		case CS_MoveStart:
			proxy->CS_MoveStartProxy(user, &protocolBuffer);
			break;
		case CS_MoveStop:
			proxy->CS_MoveStopProxy(user, &protocolBuffer);
			break;
		case CS_Attack1:
			proxy->CS_Attack1(user, &protocolBuffer);
			break;
		case CS_Attack2:
			proxy->CS_Attack2(user, &protocolBuffer);
			break;
		case CS_Attack3:
			proxy->CS_Attack3(user, &protocolBuffer);
			break;
		case CS_Tp:
			proxy->CS_Tp(user, &protocolBuffer);
			break;
		case CS_KillUser:
			proxy->CS_KillUser(user, &protocolBuffer);
			break;
		case CS_UserList:
			proxy->CS_UserList(user, &protocolBuffer);
			break;
		}

	}

}

void CNetwork::uniCast(stUser* user, CProtocolBuffer* buffer) {

	CRingBuffer* sendBuffer = user->sendBuffer;

	stHeader header;
	memcpy(&header, buffer->getFrontPtr(), sizeof(stHeader));

	swprintf_s(str, strLen, L"[NETWORK] send packet, id: %d, type: %d", user->id, header.payloadType);
	logger->Logging(str, LOG_NOTICE);

	int totalPacketSize = buffer->getUsedSize();

	while (buffer->getUsedSize() > 0) {

		int packetSize = buffer->getUsedSize();
		int directFreeSize = sendBuffer->getDirectFreeSize();

		if (directFreeSize > packetSize) {
			directFreeSize = packetSize;
		}

		memcpy(sendBuffer->getDirectPush(), buffer->getFrontPtr(), directFreeSize);
		sendBuffer->moveRear(directFreeSize);
		buffer->moveFront(directFreeSize);

	}

	buffer->moveFront(-totalPacketSize);
}

void CNetwork::disconnectUser(stUser* user) {

	closesocket(user->sock);

	userList->eraseUser(user->id);

	sectorList->getSector(user->sectorY, user->sectorX)->eraseUser(user->id);

}