#pragma once

class CImage;

struct CAnimation {

public:

	CAnimation() {}
	CAnimation(int imageNum);
	~CAnimation();

	void update();
	void draw(int x, int y, float redRatio = 1, float blueRatio = 1, float greenRatio = 1);

	void loadImage(DWORD index, const WCHAR* fileName, DWORD delayFrame, int centerX, int centerY, DWORD alphaColor);

	bool _doneSingleTime;
	bool _playOnce;
	int _currentImageIndex;

	inline void operator=(CAnimation& ani) {

		_doneSingleTime = ani._doneSingleTime;
		_playOnce = ani._playOnce;
		_currentImageIndex = ani._currentImageIndex;

		_imageNum = ani._imageNum;
		_images = ani._images;
		_delayFrameNum = ani._delayFrameNum;
		_currentDelayFrame = ani._currentDelayFrame;

	}

private:

	int _imageNum;
	CImage* _images;
	DWORD* _delayFrameNum;

	DWORD _currentDelayFrame;


};

