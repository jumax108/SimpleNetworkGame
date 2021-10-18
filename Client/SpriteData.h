#pragma once

class CAnimation;

class CSpriteData {

public:

	CSpriteData() {
		_animationNum = 0;
		_animation = nullptr;
	}
	CSpriteData(DWORD animationNum, DWORD* imgNumOfAnimations);
	~CSpriteData();

	inline CAnimation* operator[](DWORD index) {
		return &_animation[index];
	}

	inline void operator=(CSpriteData& data) {
		_animationNum = data._animationNum;
		_animation = new CAnimation[_animationNum];
		for (UINT aniCnt = 0; aniCnt < _animationNum; ++aniCnt) {
			_animation[aniCnt] = data._animation[aniCnt];
		}
	}

private:

	DWORD _animationNum;
	CAnimation* _animation;


};