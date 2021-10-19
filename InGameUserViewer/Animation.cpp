#include <Windows.h>
#include <new>

#include "Image.h"

#include "Animation.h"


CAnimation::CAnimation(int imageNum) {

	_imageNum = imageNum;
	_images = (CImage*)malloc(sizeof(CImage) * imageNum);
	_delayFrameNum = (DWORD*)malloc(sizeof(DWORD) * imageNum);

	_currentImageIndex = 0;
	_currentDelayFrame = 0;

	_doneSingleTime = false;
	_playOnce = false;
}

CAnimation::~CAnimation() {

	for (int imageCnt = 0; imageCnt < _imageNum; ++imageCnt) {
		_images[imageCnt].~CImage();
	}
	free(_images);
	free(_delayFrameNum);

}

void CAnimation::update() {

	_currentDelayFrame += 1;

	if (_currentDelayFrame >= _delayFrameNum[_currentImageIndex]) {
		_currentDelayFrame = 0;
		_currentImageIndex += 1;
		if (_currentImageIndex >= _imageNum) {
			if (_playOnce == true) {
				_currentImageIndex = _imageNum - 1;
			}
			else {
				_currentImageIndex = 0;
			}
			_doneSingleTime = true;
		}
	}

}

void CAnimation::draw(int x, int y, float redRatio, float blueRatio, float greenRatio) {

	_images[_currentImageIndex].draw(x, y, 1, redRatio, blueRatio, greenRatio);

}

void CAnimation::loadImage(DWORD index, const WCHAR* fileName, DWORD delayFrame, int centerX, int centerY, DWORD alphaColor) {


	new(&_images[index]) CImage(fileName, centerX, centerY, alphaColor);

	CImage* img = &_images[index];

	_delayFrameNum[index] = delayFrame;

}