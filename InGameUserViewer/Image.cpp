#include <Windows.h>
#include <stdio.h>

#include "ScreenDib.h"

#include "Image.h"

CImage::CImage(const WCHAR* fileName, int centerX, int centerY, DWORD alphaColor) {

	FILE* bmpFile;
	_wfopen_s(&bmpFile, fileName, L"rb");

	BITMAPFILEHEADER fileHeader;
	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, bmpFile);

	_infoHeader = (BITMAPINFOHEADER*)malloc(sizeof(BITMAPINFOHEADER));
	fread(_infoHeader, sizeof(BITMAPINFOHEADER), 1, bmpFile);

	int width = _infoHeader->biWidth;
	int height = _infoHeader->biHeight;
	_colorByte = _infoHeader->biBitCount / 8;
	_pitch = (width * _colorByte + _colorByte) & (~_colorByte);
	_size = _pitch * height;

	BYTE* reverseBuf = (BYTE*)malloc(_size);
	_buf = (BYTE*)malloc(_size);

	fread(reverseBuf, _size, 1, bmpFile);

	for (int heightCnt = 0; heightCnt < height; ++heightCnt) {

		memcpy(_buf + _pitch * heightCnt, reverseBuf + _pitch * (height - heightCnt - 1), _pitch);

	}

	_centerX = centerX;
	_centerY = centerY;

	_alphaG = *(BYTE*)&alphaColor;
	_alphaB = *(((BYTE*)&alphaColor) + 1);
	_alphaR = *(((BYTE*)&alphaColor) + 2);

}

CImage::~CImage() {
	free(_infoHeader);
	free(_buf);
}

void CImage::draw(int x, int y, float widthRatio, float redRatio, float blueRatio, float greenRatio) {


	x -= _centerX;
	y -= _centerY;
	int srcWidth = _infoHeader->biWidth * widthRatio;
	int srcHeight = _infoHeader->biHeight;
	BYTE* srcBuf = _buf;

	if (x < 0) {

		srcWidth -= -x;
		srcBuf += (-x) * _colorByte;
		x = 0;
	}

	if (y < 0) {

		srcHeight -= -y;
		srcBuf += (-y) * _pitch;
		y = 0;

	}

	if (x + srcWidth > CLIENT_WIDTH) {

		srcWidth -= x + srcWidth - CLIENT_WIDTH;

	}

	if (y + srcHeight > CLIENT_HEIGHT) {

		srcHeight -= y + srcHeight - CLIENT_HEIGHT;

	}

	CScreenDib* screen = CScreenDib::getInstance();
	int destPitch = screen->getPitch();
	BYTE* destBuf = screen->getDibBuffer() + (x * _colorByte) + (y * destPitch);

	for (int heightCnt = 0; heightCnt < srcHeight; ++heightCnt) {

		BYTE* destBufLine = destBuf;
		BYTE* srcBufLine = srcBuf;

		for (int widthCnt = 0; widthCnt < srcWidth; ++widthCnt) {

			BYTE* srcG = srcBufLine;
			BYTE* srcB = srcBufLine + 1;
			BYTE* srcR = srcBufLine + 2;

			if (!(*srcG == _alphaG && *srcB == _alphaB && *srcR == _alphaR)) {

				BYTE* destG = destBufLine;
				BYTE* destB = destBufLine + 1;
				BYTE* destR = destBufLine + 2;

				*destG = min(*srcG * greenRatio, 255);
				*destB = min(*srcB * blueRatio,255);
				*destR = min(*srcR * redRatio,255);

			}

			srcBufLine += _colorByte;
			destBufLine += _colorByte;


		}

		destBuf += destPitch;
		srcBuf += _pitch;

	}

}