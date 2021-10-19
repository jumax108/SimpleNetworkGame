#pragma once

extern const int CLIENT_WIDTH;
extern const int CLIENT_HEIGHT;

class CImage {

public:
	CImage(const WCHAR* fileName, int centerX, int centerY, DWORD alphaColor);

	void draw(int x, int y, float widthRatio = 1.0f, float redRatio = 1, float blueRatio = 1, float greenRatio = 1);

	~CImage();

private:

	int _pitch;
	int _colorByte;
	int _size;

	int _centerX;
	int _centerY;

	BYTE _alphaG;
	BYTE _alphaB;
	BYTE _alphaR;

	BITMAPINFOHEADER* _infoHeader;
	BYTE* _buf;

};