#include <stdio.h>
#include <conio.h>
#include <string.h>

#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib, "ws2_32")

#include <locale>

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

bool recvEnd;

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
		case SC_UserList: {
			int num;
			*payload >> num;

			int* idArr = new int[num];
			char* dirArr = new char[num];
			short* xArr = new short[num];
			short* yArr = new short[num];

			for (int idx = 0; idx < num; ++idx) {
				*payload >> idArr[idx];
				*payload >> dirArr[idx];
				*payload >> xArr[idx];
				*payload >> yArr[idx];
			}

			wprintf(L"-------------------------\n");
			for (int idx = 0; idx < num; ++idx) {
				wprintf(L"id: %d, dir: %d, x: %d, y: %d\n", idArr[idx], dirArr[idx], xArr[idx], yArr[idx]);
			}
			wprintf(L"-------------------------\n");

			delete[] idArr;
			delete[] dirArr;
			delete[] xArr;
			delete[] yArr;

			recvEnd = true;

			break;
		}
	}
}

void recvLogic() {
	
	int num = recv(sock, recvBuffer->getDirectPush(), recvBuffer->getDirectFreeSize(), 0);
	recvBuffer->moveRear(num);

	while (recvBuffer->getUsedSize() > 3) {

		stHeader header;
		recvBuffer->front(sizeof(stHeader), (char*)&header);

		if (recvBuffer->getUsedSize() < sizeof(stHeader) + header.payloadSize) {
			break;
		}

		recvBuffer->moveFront(sizeof(stHeader));

		CProtocolBuffer payload(header.payloadSize);
		recvBuffer->front(header.payloadSize, payload.getRearPtr());
		payload.moveRear(header.payloadSize);
		recvBuffer->moveFront(header.payloadSize);

		packetProc(&header, &payload);

	}

}

int main() {

	setlocale(LC_ALL, "");

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

			if (cmdWord->size() < 4) {
				wprintf(L"-------------------------\n");
				wprintf(L"tp [id] [x] [y]\n");
				wprintf(L"-------------------------\n");
				continue;
			}

			int id = _wcstoi64(cmdWord->at(1), nullptr, 10);

			if (id == 0) {
				continue;
			}

			short x = _wcstoi64(cmdWord->at(2), nullptr, 10);
			short y = _wcstoi64(cmdWord->at(3), nullptr, 10);

			CProtocolBuffer packet(50);
			stub.CS_TpStub(&packet, id, x, y);

			send(sock, packet.getFrontPtr(), packet.getUsedSize(), 0);
			wprintf(L"%d\n", WSAGetLastError());

		}
		else if (wcscmp(cmdWord->at(0), L"userlist") == 0) {
			if (cmdWord->size() < 7) {
				wprintf(L"-------------------------\n");
				wprintf(L"userlist [id] [direction] [left] [top] [right] [bottom]\n");
				wprintf(L"각 조건에 해당하는 유저 리스트가 출력됨\n-100 입력하면 해당 조건은 무시함\n");
				wprintf(L"-------------------------\n");
				continue;
			}

			int id = _wcstoi64(cmdWord->at(1), nullptr, 10);
			char direction = _wcstoi64(cmdWord->at(2), nullptr, 10);
			short left = _wcstoi64(cmdWord->at(3), nullptr, 10);
			short top = _wcstoi64(cmdWord->at(4), nullptr, 10);
			short right = _wcstoi64(cmdWord->at(5), nullptr, 10);
			short bottom = _wcstoi64(cmdWord->at(6), nullptr, 10);

			CProtocolBuffer packet(50);
			stub.CS_UserListStub(&packet, id, direction, left, top, right, bottom);

			send(sock, packet.getFrontPtr(), packet.getUsedSize(), 0);
			wprintf(L"%d\n", WSAGetLastError());

			recvEnd = false;
			while (recvEnd == false) {
				recvLogic();
			}

		}
		else if (wcscmp(cmdWord->at(0), L"clear") == 0) {
			system("cls");
		}

		for (std::vector<wchar_t*>::iterator iter = cmdWord->begin(); iter != cmdWord->end(); ++iter) {
			delete* iter;
		}
		cmdWord->clear();

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