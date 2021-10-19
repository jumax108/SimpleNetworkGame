#include "framework.h"
#include "ringBuffer.h"
#include "network.h"
#include "networkMessage.h"
#include "protocolBuffer.h"
#include "common.h"
#include "recv.h"
#include "send.h"

void printWsaErrorW(const WCHAR* errorMsg, int* errorCode, const WCHAR* file, int line) {

	*errorCode = WSAGetLastError();

	wprintf(L"%s\n",errorMsg);
	wprintf(L"file: %s\n", file);
	wprintf(L"line: %d\n", line);
	wprintf(L"error: %d\n", *errorCode);

}

void msgBoxWsaErrorW(const WCHAR* errorMsg, int* errorCode, const WCHAR* file, int line) {

	WCHAR errorText[500];
	size_t index = 0;

	*errorCode = WSAGetLastError();

	index += swprintf_s(errorText		 , 500        , L"msg : %s\n", errorMsg);
	index += swprintf_s(errorText + index, 500 - index, L"file : %s\n", file);
	index += swprintf_s(errorText + index, 500 - index, L"line : %d\n", line);
	index += swprintf_s(errorText + index, 500 - index, L"error : %d\n", *errorCode);

	MessageBox(NULL, errorText, L"error", MB_OK);
}


CNetwork::CNetwork() {

	sock = NULL;
	new (&recvBuffer) CRingBuffer(NETWORK_BUFFER_SIZE);
	new (&sendBuffer) CRingBuffer(NETWORK_BUFFER_SIZE);

	ableSendPacket = false;

}

bool CNetwork::networkInit(HWND hWnd) {

	WSAData wsaData;
	int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	int wsaStartupError;
	if (wsaStartupResult != NULL) {
		errorOutputFunc(L"startup error", &wsaStartupError);
		return false;
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	int socketError;
	if (sock == INVALID_SOCKET) {
		errorOutputFunc(L"socket error", &socketError);
		return false;
	}

	ULONG setNonBlockingSocket = 1;
	int ioctlResult = ioctlsocket(sock, FIONBIO, &setNonBlockingSocket);
	int ioctlError;
	if (ioctlResult == SOCKET_ERROR) {
		errorOutputFunc(L"ioctl error", &ioctlError);
		return false;
	}

	WSAAsyncSelect(sock, hWnd, UM_SOCKET, FD_READ | FD_WRITE);

	SOCKADDR_IN sockAddr;
	sockAddr.sin_family = AF_INET;
	InetPtonW(AF_INET, SERVER_IP, &sockAddr.sin_addr.S_un.S_addr);
	sockAddr.sin_port = htons(SERVER_PORT);
	int connectResult = connect(sock, (SOCKADDR*)&sockAddr, sizeof(sockAddr));
	int connectError;
	if (connectResult == SOCKET_ERROR) {
		errorOutputFunc(L"connect error", &connectError);
		if (connectError != WSAEWOULDBLOCK) {
			return false;
		}
	}

	long setNagleOff = 1;
	setsockopt(sock, IPPROTO_TCP, TCP_NODELAY, (const char*)&setNagleOff, sizeof(setNagleOff));

	return true;

}


bool CNetwork::socketMessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {

	USHORT selectResult = WSAGETSELECTERROR(lParam);
	int selectError;
	if (selectResult != 0) {
		errorOutputFunc(L"select Error", &selectError);
		return false;
	}

	switch (WSAGETSELECTEVENT(lParam)) {
	case FD_READ:
		recvPacket();
		break;
	case FD_WRITE:
		ableSendPacket = true;
		sendPacket();
		break;
	}

	return true;
}


bool CNetwork::recvPacket() {

	while (1) {
		short readSize = recvBuffer.getDirectFreeSize();
		int recvResult = recv(sock, recvBuffer.getDirectPush(), readSize, 0);
		int recvError;
		if (recvResult == SOCKET_ERROR) {
			errorOutputFunc(L"recv error", &recvError);
			if (recvError == WSAEWOULDBLOCK) {
				break;
			}
			int k = 1;
			return false;
		}
		recvBuffer.moveRear(recvResult);
	}

	unsigned int usedSize = recvBuffer.getUsedSize();
	CProtocolBuffer* protocolBuffer = nullptr;
	while (usedSize >= sizeof(stHeader)) {
		stHeader header;
		recvBuffer.front(sizeof(header), (char*)&header);
		if (usedSize < sizeof(stHeader) + header.payloadSize) {
			return false;
		}
		recvBuffer.pop(sizeof(header));
		delete(protocolBuffer);
		protocolBuffer = new CProtocolBuffer(header.payloadSize);
		recvBuffer.front(header.payloadSize, protocolBuffer->getRearPtr());
		protocolBuffer->moveRear(header.payloadSize);
		recvBuffer.pop(header.payloadSize);
		proxy->packetProc(&header, protocolBuffer, proxy);
		usedSize = recvBuffer.getUsedSize();
	}

	delete(protocolBuffer);

	return true;

}

bool CNetwork::sendPacket() {

	while (1) {
		UINT sendBufUsedSize = sendBuffer.getUsedSize();
		if (sendBufUsedSize == 0) {
			break;
		}

		UINT sendSize = min(sendBuffer.getDirectUsedSize(), sendBufUsedSize);

		//char* buf = sendBuffer.getDirectFront();

		int sendResult = send(sock, sendBuffer.getDirectFront(), sendSize, 0);
		int sendError;
		if (sendResult == SOCKET_ERROR) {
			errorOutputFunc(L"send error", &sendError);
			if (sendError != WSAEWOULDBLOCK) {
				return false;
			}
			else {
				ableSendPacket = false;
				break;
			}
		}

		sendBuffer.moveFront(sendResult);
	}

	return true;

}