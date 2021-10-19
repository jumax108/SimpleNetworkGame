#pragma once

class CScreenDib
{

private: 

	static CScreenDib* instance;

public:

	static void setInstance(int width, int height, int colorBit) {
		instance = new CScreenDib(width, height, colorBit);
	}

	static CScreenDib* getInstance() {
		return instance;
	}

	void flip(HWND hWnd);

	inline BYTE* getDibBuffer() {
		return _bmpBuf;
	}
	inline int getWidth() {
		return _width;
	}
	inline int getHeight() {
		return _height;
	}
	inline int getPitch() {
		return _pitch;
	}
	inline int getBufSize() {
		return _bufferSize;
	}

private:

	CScreenDib(int width, int height, int colorBit);
	~CScreenDib();

private:

	BITMAPINFO _bmpInfo;
	BYTE* _bmpBuf;

	int _width;
	int _height;
	int _pitch;
	int _colorBit;
	int _bufferSize;

};
