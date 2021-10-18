#pragma once

#define errorOutputFunc printWsaError
#define printWsaError(errorMsg, errorCode) printWsaErrorW(errorMsg, errorCode, __FILEW__, __LINE__);
#define msgBoxWsaError(errorMsg, errorCode) msgBoxWsaErrorW(errorMsg, errorCode, __FILEW__, __LINE__);
#define UM_SOCKET (WM_USER+1)

constexpr UINT NETWORK_BUFFER_SIZE = 100;

class CSprite;
class CSpriteData;
class CImage;
class CAnimation;
template<typename T>
class CQueue;
class CProxyFuncBase;
class CStubFunc;

extern CSprite* user[50];
extern CQueue<USHORT>* userIndex;
extern CSpriteData* spriteData;
extern CImage* _shadow;
extern CImage* _hpBar;
extern CAnimation* _effect;
extern CSprite* mySprite;
extern CProxyFuncBase* proxy;
extern CStubFunc* stub;

void userUpdate(CSprite* sprite, void* argv);
void otherUserUpdate(CSprite* sprite, void* argv);

class CNetwork {
public:

	CNetwork();

	bool networkInit(HWND);

	bool socketMessageProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

	CRingBuffer recvBuffer;
	CRingBuffer sendBuffer;

	bool recvPacket();
	bool sendPacket();
	bool ableSendPacket;
private:

	SOCKET sock;
	const WCHAR* SERVER_IP = L"127.0.0.1";
	const USHORT SERVER_PORT = 20000;

};

void printWsaErrorW(const WCHAR* errorMsg, int* errorCode, const WCHAR* file, int line);
void msgBoxWsaErrorW(const WCHAR* errorMsg, int* errorCode, const WCHAR* file, int line);