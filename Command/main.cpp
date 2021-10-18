#include <stdio.h>
#include <conio.h>
#include <string.h>

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

#include <vector>

#include "common.h"
#include "ringBuffer.h"
#include "protocolBuffer.h"

#include "stub.h"
const wchar_t* SERVER_IP = L"127.0.0.1";
constexpr int SERVER_PORT = 20000;

SOCKET sock;

void networkInit();

CRingBuffer* recvBuffer;
CStub stub;

std::vector<wchar_t*>* split(wchar_t* str) {

	std::vector<wchar_t*>* vt = new std::vector<wchar_t*>();

	wchar_t* wordStart = str;

	int strLen = wcslen(str);
	wchar_t* strIter = str;
	wchar_t* strEnd = str + strLen;

	for (; strIter != strEnd; ++strIter) {

		if (*strIter == ' ' || *strIter == '\n') {

			do {
				if (strIter == wordStart) {
					break;
				}

				int wordLen = strIter - wordStart;
				wchar_t* word = new wchar_t[wordLen + 1];
				wcsncpy_s(word, wordLen + 1, wordStart, wordLen);
				word[wordLen] = '\0';
				vt->push_back(word);

			} while (false);

			wordStart = strIter + 1;

			if (*strIter == '\n') {
				break;
			}
		}
	}

	return vt;
}

void packetProc(stHeader* header, CProtocolBuffer* payload) {

	switch (header->payloadType) {
		case SC_CreateMyCharacter: {
			CProtocolBuffer packet(50);
			int id;
			*payload >> id;
			stub.CS_KillStub(&packet, id);
			send(sock, packet.getFrontPtr(), packet.getUsedSize(), 0);
			break;
		}
	}
}

void recvLogic() {
	
	int num = recv(sock, recvBuffer->getDirectPush(), recvBuffer->getDirectFreeSize(), 0);

	while (recvBuffer->getUsedSize() > 3) {

		stHeader header;
		recvBuffer->front(sizeof(stHeader), (char*)&header);

		if (recvBuffer->getUsedSize() < sizeof(stHeader) + header.payloadSize) {
			break;
		}

		recvBuffer->moveFront(sizeof(stHeader));

		CProtocolBuffer payload(50);
		recvBuffer->front(header.payloadSize, payload.getRearPtr());
		payload.moveRear(header.payloadSize);
		recvBuffer->moveFront(header.payloadSize);

		packetProc(&header, &payload);

	}

}

int main() {

	recvBuffer = new CRingBuffer(5000);

	networkInit();

	recvLogic();

	std::vector<wchar_t*>* cmdWord;

	while (1) {
		
		wchar_t cmd[100];
		fgetws(cmd, 100, stdin);	

		cmdWord = split(cmd);

		if (wcscmp(cmdWord->at(0), L"exit") == 0) {
			break;
		}
		else if (wcscmp(cmdWord->at(0), L"tp") == 0) {

			int id = _wcstoi64(cmdWord->at(1), nullptr, 10);
			short x = _wcstoi64(cmdWord->at(2), nullptr, 10);
			short y = _wcstoi64(cmdWord->at(3), nullptr, 10);

			CProtocolBuffer packet(50);
			stub.CS_TpStub(&packet, id, x, y);

			send(sock, packet.getFrontPtr(), packet.getUsedSize(), 0);
			wprintf(L"%d\n", WSAGetLastError());

		}


	}

	return 0;

}

void networkInit() {

	WSADATA wsaData;
	int wsaError;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		wsaError = WSAGetLastError();
		wprintf(L"wsa startup error: %d", wsaError);
		return;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);



	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	InetPtonW(AF_INET, SERVER_IP, &addr.sin_addr.S_un.S_addr);
	addr.sin_port = htons(SERVER_PORT);

	connect(sock, (SOCKADDR*)&addr, sizeof(addr));
	int connectError;
	if (sock == SOCKET_ERROR) {
		connectError = WSAGetLastError();
		wprintf(L"wsa connect error: %d", connectError);
		return;
	}
	

}