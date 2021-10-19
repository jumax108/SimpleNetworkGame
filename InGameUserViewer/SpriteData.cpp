#include <Windows.h>
#include <malloc.h>
#include <new>

#include "Animation.h"

#include "SpriteData.h"

CSpriteData::CSpriteData(DWORD animationNum, DWORD* imgNumOfAnimations) {

	_animationNum = animationNum;
	_animation = (CAnimation*)malloc(sizeof(CAnimation) * animationNum);
	for (DWORD animationCnt = 0; animationCnt < animationNum; ++animationCnt) {
		new(&_animation[animationCnt]) CAnimation(imgNumOfAnimations[animationCnt]);
	}

}

CSpriteData::~CSpriteData() {
	free(_animation);
}