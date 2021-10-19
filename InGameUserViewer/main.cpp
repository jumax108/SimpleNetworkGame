// SpriteSample.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "resource.h"
#include "ringBuffer.h"
#include "ScreenDib.h"
#include "Image.h"
#include "Animation.h"
#include "SpriteData.h"
#include "Sprite.h"
#include "networkMessage.h"
#include "network.h"
#include "queue.h"
#include "protocolBuffer.h"
#include "common.h"
#include "send.h"
#include "recv.h"

#define MIN(a,b) a>b?b:a
#define MAX(a,b) a>b?a:b

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

const int TILE_SIZE = 64;
const int MAP_HEIGHT = 6400;
const int MAP_WIDTH = 6400;

const int CLIENT_WIDTH = 800;
const int CLIENT_HEIGHT = 800;

HDC hMemDC;
HBITMAP hBufBmp;
RECT rtClient;
RECT rtCamera;

CSpriteData* spriteData;
CSprite* user[50];
CSprite* mySprite;
CQueue<USHORT>* userIndex;

CImage* _shadow;
CImage* _hpBar;
CAnimation* _effect;

LARGE_INTEGER mouseMoveTime;
bool mouseMoveNow;
LARGE_INTEGER freq;

int logicCnt = 0;
int renderCnt = 0;

WCHAR logicFrame[100] = { 0, };
WCHAR renderFrame[100] = { 0, };

CNetwork* network;

CProxyFuncBase* proxy;
CStubFunc* stub;

void otherUserUpdate(CSprite* sprite, void* argv) {

    int userCurAniIdx = sprite->_currentAnimationIndex;
    if (userCurAniIdx >= 4 && sprite->isAnimationLoopOnce() == true) {
        sprite->_msg = INPUT_MESSAGE::NONE;
    }
    do {
        switch (*(INPUT_MESSAGE*)argv) {
        case INPUT_MESSAGE::MOVE_DD:
            if (sprite->_y + 2 < 470)
                sprite->_y += 2;
            if (sprite->_oldMsg != sprite->_msg) {
                sprite->setAnimation(2);
            }
            break;
        case INPUT_MESSAGE::MOVE_LD:
            if (sprite->_y + 2 < 470 && sprite->_x - 3 > 10) {
                sprite->_y += 2;
                sprite->_x -= 3;
            }
            sprite->_seeRight = false;
            if (sprite->_oldMsg != sprite->_msg) {
                sprite->setAnimation(2);
            }
            break;
        case INPUT_MESSAGE::MOVE_LL:
            if (sprite->_x - 3 > 10)
                sprite->_x -= 3;
            sprite->_seeRight = false;
            if (sprite->_oldMsg != sprite->_msg) {
                sprite->setAnimation(2);
            }
            break;
        case INPUT_MESSAGE::MOVE_LU:
            if (sprite->_x - 3 > 10 && sprite->_y - 2 > 50) {
                sprite->_x -= 3;
                sprite->_y -= 2;
            }
            sprite->_seeRight = false;
            if (sprite->_oldMsg != sprite->_msg) {
                sprite->setAnimation(2);
            }
            break;
        case INPUT_MESSAGE::MOVE_RD:
            if (sprite->_x + 3 < 630 && sprite->_y + 2 < 470) {
                sprite->_x += 3;
                sprite->_y += 2;
            }
            sprite->_seeRight = true;
            if (sprite->_oldMsg != sprite->_msg) {
                sprite->setAnimation(2);
            }
            break;
        case INPUT_MESSAGE::MOVE_RR:
            if (sprite->_x + 3 < 630)
                sprite->_x += 3;
            sprite->_seeRight = true;
            if (sprite->_oldMsg != sprite->_msg) {
                sprite->setAnimation(2);
            }
            break;
        case INPUT_MESSAGE::MOVE_RU:
            if (sprite->_x + 3 < 630 && sprite->_y - 2 > 50) {
                sprite->_x += 3;
                sprite->_y -= 2;
            }
            sprite->_seeRight = true;
            if (sprite->_oldMsg != sprite->_msg) {
                sprite->setAnimation(2);
            }
            break;
        case INPUT_MESSAGE::MOVE_UU:
            if (sprite->_y - 2 > 50)
                sprite->_y -= 2;
            if (sprite->_oldMsg != sprite->_msg) {
                sprite->setAnimation(2);
            }
            break;
        case INPUT_MESSAGE::NONE:
            if (userCurAniIdx != 0) {
                sprite->setAnimation(0);
            }
            break;
        case INPUT_MESSAGE::ATTACK1:
            if (sprite->_oldMsg != sprite->_msg) {
                sprite->setAnimation(4);
            }

            break;
        case INPUT_MESSAGE::ATTACK2:
            if (sprite->_oldMsg != sprite->_msg) {
                sprite->setAnimation(6);
            }

            break;
        case INPUT_MESSAGE::ATTACK3:
            if (sprite->_oldMsg != sprite->_msg) {
                sprite->setAnimation(8);
            }

            break;
        }
    } while (false);
    sprite->_oldMsg = sprite->_msg;
}

static bool regist = false;
void update() {

    ++logicCnt;

    for (int userCnt = 0; userCnt < 50; userCnt++) {
        if (user[userCnt]->isLive() == true) {
            user[userCnt]->update(&user[userCnt]->_msg);
        }
    }

    for (int userCnt = 0; userCnt < 49; userCnt++) {
        for (int cmpUserCnt = userCnt + 1; cmpUserCnt < 50; cmpUserCnt++) {

            if (user[userCnt]->_y > user[cmpUserCnt]->_y) {
                void* temp = user[userCnt];
                user[userCnt] = user[cmpUserCnt];
                user[cmpUserCnt] = (CSprite*)temp;
            }

        }
    }

    if (mySprite == nullptr) {

        rtCamera.left = 0;
        rtCamera.top = 0;
        rtCamera.right = CLIENT_WIDTH;
        rtCamera.bottom = CLIENT_HEIGHT;

    }
    else if(regist == false) {

        stub->CS_RegistUserViewerStub(&network->sendBuffer, mySprite->_id);
        regist = true;
    }
}

void render(HWND hWnd) {

    renderCnt += 1;
    BYTE* bmpBuf;
    int bufWidth;
    int bufHeight;
    int bufPitch;
    int bufSize;

    {
        CScreenDib* screen = CScreenDib::getInstance();
        bmpBuf = screen->getDibBuffer();
        bufWidth = screen->getWidth();
        bufHeight = screen->getHeight();
        bufPitch = screen->getPitch();
        bufSize = screen->getBufSize();

        BYTE* bufIter = bmpBuf;
        BYTE* bufEnd = bmpBuf + bufSize;
        for (; bufIter != bufEnd; bufIter += 4) {
            *bufIter = 43;
            *(bufIter+1) = 43;
            *(bufIter+2) = 43;
        }
        //memset(bmpBuf, bufSize, 43);
    }

    for (int userCnt = 0; userCnt < 50; userCnt++) {
        if (user[userCnt]->isLive() == true) {
            if (user[userCnt] == mySprite) {
                user[userCnt]->draw(2, 1, 1);
            }
            else {
                user[userCnt]->draw();
            }
        }
    }

    CScreenDib::getInstance()->flip(hWnd);

}

BYTE* loadImage(const WCHAR* fileDir, int* width = nullptr, int* height = nullptr) {

    BYTE* buf;
    BYTE* reverseBuf;

    FILE* bmpFile;
    _wfopen_s(&bmpFile, fileDir, L"rb");

    BITMAPFILEHEADER bmpHead;
    BITMAPINFOHEADER bmpInfo;
    memset(&bmpInfo, 0, sizeof(BITMAPINFOHEADER));
    bmpInfo.biSize = sizeof(BITMAPINFOHEADER);
    fread(&bmpHead, sizeof(BITMAPFILEHEADER), 1, bmpFile);
    fread(&bmpInfo, sizeof(BITMAPINFOHEADER), 1, bmpFile);
    DWORD bufSize = sizeof(BYTE) * bmpInfo.biHeight * bmpInfo.biWidth * 4;
    buf = (BYTE*)malloc(bufSize);
    reverseBuf = (BYTE*)malloc(bufSize);
    DWORD pitch = (bmpInfo.biWidth * bmpInfo.biBitCount / 8 + 3) & ~3;
    fread(buf, sizeof(BYTE), bmpInfo.biHeight * bmpInfo.biWidth * 4, bmpFile);

    BYTE* pBuf = buf;
    BYTE* pReverseBuf = reverseBuf + bufSize - pitch;

    for (LONG heightCnt = 0; heightCnt < bmpInfo.biHeight; heightCnt++) {
        memcpy(pReverseBuf, pBuf, sizeof(BYTE) * bmpInfo.biWidth * 4);
        pReverseBuf -= pitch;
        pBuf += pitch;
    }

    if (width != nullptr) {
        *width = bmpInfo.biWidth;
        *height = bmpInfo.biHeight;
    }

    fclose(bmpFile);

    free(buf);
    return reverseBuf;
}



void init(HWND hWnd) {

    QueryPerformanceFrequency(&freq);
    freq.QuadPart = freq.QuadPart / 1000000;
    CScreenDib::setInstance(CLIENT_WIDTH, MAP_HEIGHT, 32);

    DWORD aniNum = 10;
    DWORD* imgNums = (DWORD*)malloc(sizeof(DWORD) * aniNum);
    imgNums[0] = 5;
    imgNums[1] = 5;
    imgNums[2] = 12;
    imgNums[3] = 12;
    imgNums[4] = 4;
    imgNums[5] = 4;
    imgNums[6] = 4;
    imgNums[7] = 4;
    imgNums[8] = 6;
    imgNums[9] = 6;
    spriteData = new CSpriteData(aniNum, imgNums);
    // ----------------------------------------------------------------------
    // user idle 애니메이션 로드
    CAnimation* idleLeft = (*spriteData)[0];
    idleLeft->loadImage(0, L"Sprite_Data\\Stand_L_01.bmp", 5, 71, 90, 0xffffffff);
    idleLeft->loadImage(1, L"Sprite_Data\\Stand_L_02.bmp", 5, 71, 90, 0xffffffff);
    idleLeft->loadImage(2, L"Sprite_Data\\Stand_L_03.bmp", 5, 71, 90, 0xffffffff);
    idleLeft->loadImage(3, L"Sprite_Data\\Stand_L_02.bmp", 5, 71, 90, 0xffffffff);
    idleLeft->loadImage(4, L"Sprite_Data\\Stand_L_01.bmp", 5, 71, 90, 0xffffffff);

    CAnimation* idleRight = (*spriteData)[1];
    idleRight->loadImage(0, L"Sprite_Data\\Stand_R_01.bmp", 5, 71, 90, 0xffffffff);
    idleRight->loadImage(1, L"Sprite_Data\\Stand_R_02.bmp", 5, 71, 90, 0xffffffff);
    idleRight->loadImage(2, L"Sprite_Data\\Stand_R_03.bmp", 5, 71, 90, 0xffffffff);
    idleRight->loadImage(3, L"Sprite_Data\\Stand_R_02.bmp", 5, 71, 90, 0xffffffff);
    idleRight->loadImage(4, L"Sprite_Data\\Stand_R_01.bmp", 5, 71, 90, 0xffffffff);
    // ----------------------------------------------------------------------
    // 
    // ----------------------------------------------------------------------
    // user move 애니메이션 로드
    CAnimation* moveLeft = (*spriteData)[2];
    moveLeft->loadImage(0, L"Sprite_Data\\Move_L_01.bmp", 4, 71, 90, 0xffffffff);
    moveLeft->loadImage(1, L"Sprite_Data\\Move_L_02.bmp", 4, 71, 90, 0xffffffff);
    moveLeft->loadImage(2, L"Sprite_Data\\Move_L_03.bmp", 4, 71, 90, 0xffffffff);
    moveLeft->loadImage(3, L"Sprite_Data\\Move_L_04.bmp", 4, 71, 90, 0xffffffff);
    moveLeft->loadImage(4, L"Sprite_Data\\Move_L_05.bmp", 4, 71, 90, 0xffffffff);
    moveLeft->loadImage(5, L"Sprite_Data\\Move_L_06.bmp", 4, 71, 90, 0xffffffff);
    moveLeft->loadImage(6, L"Sprite_Data\\Move_L_07.bmp", 4, 71, 90, 0xffffffff);
    moveLeft->loadImage(7, L"Sprite_Data\\Move_L_08.bmp", 4, 71, 90, 0xffffffff);
    moveLeft->loadImage(8, L"Sprite_Data\\Move_L_09.bmp", 4, 71, 90, 0xffffffff);
    moveLeft->loadImage(9, L"Sprite_Data\\Move_L_10.bmp", 4, 71, 90, 0xffffffff);
    moveLeft->loadImage(10, L"Sprite_Data\\Move_L_11.bmp", 4, 71, 90, 0xffffffff);
    moveLeft->loadImage(11, L"Sprite_Data\\Move_L_12.bmp", 4, 71, 90, 0xffffffff);

    CAnimation* moveRight = (*spriteData)[3];
    moveRight->loadImage(0, L"Sprite_Data\\Move_R_01.bmp", 4, 71, 90, 0xffffffff);
    moveRight->loadImage(1, L"Sprite_Data\\Move_R_02.bmp", 4, 71, 90, 0xffffffff);
    moveRight->loadImage(2, L"Sprite_Data\\Move_R_03.bmp", 4, 71, 90, 0xffffffff);
    moveRight->loadImage(3, L"Sprite_Data\\Move_R_04.bmp", 4, 71, 90, 0xffffffff);
    moveRight->loadImage(4, L"Sprite_Data\\Move_R_05.bmp", 4, 71, 90, 0xffffffff);
    moveRight->loadImage(5, L"Sprite_Data\\Move_R_06.bmp", 4, 71, 90, 0xffffffff);
    moveRight->loadImage(6, L"Sprite_Data\\Move_R_07.bmp", 4, 71, 90, 0xffffffff);
    moveRight->loadImage(7, L"Sprite_Data\\Move_R_08.bmp", 4, 71, 90, 0xffffffff);
    moveRight->loadImage(8, L"Sprite_Data\\Move_R_09.bmp", 4, 71, 90, 0xffffffff);
    moveRight->loadImage(9, L"Sprite_Data\\Move_R_10.bmp", 4, 71, 90, 0xffffffff);
    moveRight->loadImage(10, L"Sprite_Data\\Move_R_11.bmp", 4, 71, 90, 0xffffffff);
    moveRight->loadImage(11, L"Sprite_Data\\Move_R_12.bmp", 4, 71, 90, 0xffffffff);
    // ----------------------------------------------------------------------

    // ----------------------------------------------------------------------
    // user attack1 애니메이션 로드
    CAnimation* attack1Left = (*spriteData)[4];
    attack1Left->loadImage(0, L"Sprite_Data\\Attack1_L_01.bmp", 3, 71, 90, 0xffffffff);
    attack1Left->loadImage(1, L"Sprite_Data\\Attack1_L_02.bmp", 3, 71, 90, 0xffffffff);
    attack1Left->loadImage(2, L"Sprite_Data\\Attack1_L_03.bmp", 3, 71, 90, 0xffffffff);
    attack1Left->loadImage(3, L"Sprite_Data\\Attack1_L_04.bmp", 3, 71, 90, 0xffffffff);
    attack1Left->_playOnce = true;

    CAnimation* attack1Right = (*spriteData)[5];
    attack1Right->loadImage(0, L"Sprite_Data\\Attack1_R_01.bmp", 3, 71, 90, 0xffffffff);
    attack1Right->loadImage(1, L"Sprite_Data\\Attack1_R_02.bmp", 3, 71, 90, 0xffffffff);
    attack1Right->loadImage(2, L"Sprite_Data\\Attack1_R_03.bmp", 3, 71, 90, 0xffffffff);
    attack1Right->loadImage(3, L"Sprite_Data\\Attack1_R_04.bmp", 3, 71, 90, 0xffffffff);
    attack1Right->_playOnce = true;
    // ----------------------------------------------------------------------


    // ----------------------------------------------------------------------
    // user attack2 애니메이션 로드
    CAnimation* attack2Left = (*spriteData)[6];
    attack2Left->loadImage(0, L"Sprite_Data\\Attack2_L_01.bmp", 4, 71, 90, 0xffffffff);
    attack2Left->loadImage(1, L"Sprite_Data\\Attack2_L_02.bmp", 4, 71, 90, 0xffffffff);
    attack2Left->loadImage(2, L"Sprite_Data\\Attack2_L_03.bmp", 4, 71, 90, 0xffffffff);
    attack2Left->loadImage(3, L"Sprite_Data\\Attack2_L_04.bmp", 4, 71, 90, 0xffffffff);
    attack2Left->_playOnce = true;

    CAnimation* attack2Right = (*spriteData)[7];
    attack2Right->loadImage(0, L"Sprite_Data\\Attack2_R_01.bmp", 4, 71, 90, 0xffffffff);
    attack2Right->loadImage(1, L"Sprite_Data\\Attack2_R_02.bmp", 4, 71, 90, 0xffffffff);
    attack2Right->loadImage(2, L"Sprite_Data\\Attack2_R_03.bmp", 4, 71, 90, 0xffffffff);
    attack2Right->loadImage(3, L"Sprite_Data\\Attack2_R_04.bmp", 4, 71, 90, 0xffffffff);
    attack2Right->_playOnce = true;
    // ----------------------------------------------------------------------

    // ----------------------------------------------------------------------
    // user attack3 애니메이션 로드
    CAnimation* attack3Left = (*spriteData)[8];
    attack3Left->loadImage(0, L"Sprite_Data\\Attack3_L_01.bmp", 4, 71, 90, 0xffffffff);
    attack3Left->loadImage(1, L"Sprite_Data\\Attack3_L_02.bmp", 4, 71, 90, 0xffffffff);
    attack3Left->loadImage(2, L"Sprite_Data\\Attack3_L_03.bmp", 4, 71, 90, 0xffffffff);
    attack3Left->loadImage(3, L"Sprite_Data\\Attack3_L_04.bmp", 4, 71, 90, 0xffffffff);
    attack3Left->loadImage(4, L"Sprite_Data\\Attack3_L_05.bmp", 4, 71, 90, 0xffffffff);
    attack3Left->loadImage(5, L"Sprite_Data\\Attack3_L_06.bmp", 4, 71, 90, 0xffffffff);
    attack3Left->_playOnce = true;

    CAnimation* attack3Right = (*spriteData)[9];
    attack3Right->loadImage(0, L"Sprite_Data\\Attack3_R_01.bmp", 4, 71, 90, 0xffffffff);
    attack3Right->loadImage(1, L"Sprite_Data\\Attack3_R_02.bmp", 4, 71, 90, 0xffffffff);
    attack3Right->loadImage(2, L"Sprite_Data\\Attack3_R_03.bmp", 4, 71, 90, 0xffffffff);
    attack3Right->loadImage(3, L"Sprite_Data\\Attack3_R_04.bmp", 4, 71, 90, 0xffffffff);
    attack3Right->loadImage(4, L"Sprite_Data\\Attack3_R_05.bmp", 4, 71, 90, 0xffffffff);
    attack3Right->loadImage(5, L"Sprite_Data\\Attack3_R_06.bmp", 4, 71, 90, 0xffffffff);
    attack3Right->_playOnce = true;
    // ----------------------------------------------------------------------

    // ----------------------------------------------------------------------
    // 그림자 로드
    _shadow = new CImage(L"Sprite_Data\\Shadow.bmp", 32, 4, 0xffffffff);
    // ----------------------------------------------------------------------

    // ----------------------------------------------------------------------
    // 체력바 로드
    _hpBar = new CImage(L"Sprite_Data\\HPGuage.bmp", 35, -10, 0xffffffff);
    // ----------------------------------------------------------------------

    // ----------------------------------------------------------------------
    // 이펙트 애니메이션 로드
    _effect = new CAnimation(4);
    _effect->loadImage(0, L"Sprite_Data\\xSpark_1.bmp", 4, 70, 140, 0xffffffff);
    _effect->loadImage(1, L"Sprite_Data\\xSpark_2.bmp", 4, 70, 140, 0xffffffff);
    _effect->loadImage(2, L"Sprite_Data\\xSpark_3.bmp", 4, 70, 140, 0xffffffff);
    _effect->loadImage(3, L"Sprite_Data\\xSpark_4.bmp", 4, 70, 140, 0xffffffff);
    _effect->_playOnce = true;
    _effect->_doneSingleTime = true;
    // ----------------------------------------------------------------------

    userIndex = new CQueue<USHORT>(50);
    for (USHORT userCnt = 0; userCnt < 50; userCnt++) {
        userIndex->push(userCnt);
        user[userCnt] = new CSprite();
    }

    proxy = new CProxyFunc();
    stub = new CStubFunc();

    network = new CNetwork();


    return;
}

int sleep = 0;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        init(hWnd);
        bool networkInitResult = network->networkInit(hWnd);
        if (networkInitResult == false) {
            PostQuitMessage(0);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        ReleaseDC(hWnd, hMemDC);
        PostQuitMessage(0);
        break;

    case UM_SOCKET:
        network->socketMessageProc(hWnd, message, wParam, lParam);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_SPRITESAMPLE, szWindowClass, MAX_LOADSTRING);

    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SPRITESAMPLE));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    RegisterClassExW(&wcex);

    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_SYSMENU,
        0, 0, CLIENT_WIDTH, CLIENT_HEIGHT, nullptr, nullptr, hInstance, nullptr);

    RECT rt = { 0, };
    rt.right = CLIENT_WIDTH;
    rt.bottom = CLIENT_HEIGHT;

    AdjustWindowRectEx(&rt, GetWindowStyle(hWnd), GetMenu(hWnd) != NULL, GetWindowExStyle(hWnd));

    MoveWindow(hWnd, 0, 0, rt.right - rt.left, rt.bottom - rt.top, false);

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);


    MSG msg;

    AllocConsole();
    FILE* fp = nullptr;
    freopen_s(&fp, "CONOUT$", "w+", stdout);

    //QueryPerformanceFrequency
    // 기본 메시지 루프입니다:
    //QueryPerformanceCounter(&startTick);


    LARGE_INTEGER startTick = { 0, };
    LARGE_INTEGER endTick = { 0, };

    LARGE_INTEGER fpsStart = { 0, };
    LARGE_INTEGER fpsEnd = { 0, };
    __int64 fpsStartTime = fpsStart.QuadPart / freq.QuadPart;
    QueryPerformanceCounter(&fpsStart);


    timeBeginPeriod(1);

    __int64 startTime = 0;
    __int64 endTime;
    __int64 frameTime;
    int delayTime = 0;
    int intervalTime = 20 * 1000;

    __int64 remainDelayTime = 0;

    bool renderSkip = false;

    QueryPerformanceCounter(&fpsStart);
    fpsStartTime = fpsStart.QuadPart / freq.QuadPart;
    QueryPerformanceCounter(&startTick);
    startTime = startTick.QuadPart / freq.QuadPart;

    do {

        if (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessageW(&msg);
        }
        else {
            update();
            renderSkip = false;
            QueryPerformanceCounter(&endTick);
            QueryPerformanceCounter(&startTick);
            endTime = endTick.QuadPart / freq.QuadPart;
            frameTime = endTime - startTime;
            startTime = startTick.QuadPart / freq.QuadPart;
            remainDelayTime += intervalTime - frameTime;

            if (remainDelayTime > 0) {
                delayTime = (int)(remainDelayTime / 1000);
                remainDelayTime -= (long long)delayTime * 1000;
                Sleep(delayTime);
                startTime += (long long)delayTime * 1000;
            }
            else {
                renderSkip = true;
            }

            if (renderSkip == false) {
                render(hWnd);
            }

            QueryPerformanceCounter(&fpsEnd);
            __int64 fpsEndTime = fpsEnd.QuadPart / freq.QuadPart;
            if (fpsEndTime - fpsStartTime >= (__int64)1000 * 1000) {
                QueryPerformanceCounter(&fpsStart);
                fpsStartTime = fpsStart.QuadPart / freq.QuadPart;

                wsprintfW(logicFrame, L"Logic FPS: %d", logicCnt);
                wsprintfW(renderFrame, L"render FPS: %d", renderCnt);
                logicCnt = 0;
                renderCnt = 0;


            }
            HDC hdc = GetDC(hWnd);
            TextOutW(hdc, 0, 0, logicFrame, wcslen(logicFrame));
            TextOutW(hdc, 0, 20, renderFrame, wcslen(renderFrame));
            ReleaseDC(hWnd, hdc);


        }

    } while (msg.message != WM_QUIT);

    return (int)msg.wParam;
}

