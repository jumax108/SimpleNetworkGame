#include "framework.h"
#include "ScreenDib.h"

CScreenDib* CScreenDib::instance = nullptr;
CScreenDib::CScreenDib(int width, int height, int colorBit) {
	
	memset(&_bmpInfo, 0, sizeof(BITMAPINFO));

	BITMAPINFOHEADER* infoHeader = &_bmpInfo.bmiHeader;
	infoHeader->biSize = sizeof(BITMAPINFOHEADER);
	infoHeader->biWidth = width;
	infoHeader->biHeight = -height;
	infoHeader->biPlanes = 1;
	infoHeader->biBitCount = colorBit;

	_width = width;
	_height = height;
	_pitch = (width * colorBit / 8 + 3) & ~3;
	_bufferSize = width * height * (colorBit / 8);

	_bmpBuf = (BYTE*)malloc(_bufferSize);

}

CScreenDib::~CScreenDib() {
	free(_bmpBuf);
}

void CScreenDib::flip(HWND hWnd) {
	HDC hdc = GetDC(hWnd);
	SetDIBitsToDevice(hdc, 0, 0, _width, _height,
		0, 0, 0, _height, _bmpBuf, &_bmpInfo, DIB_RGB_COLORS);
	ReleaseDC(hWnd, hdc);
}