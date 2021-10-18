#pragma once

enum class INPUT_MESSAGE {
	NONE = -1,
	MOVE_LL,
	MOVE_LU,
	MOVE_UU,
	MOVE_RU,
	MOVE_RR,
	MOVE_RD,
	MOVE_DD,
	MOVE_LD,
	ATTACK1,
	ATTACK2,
	ATTACK3
};

extern RECT rtCamera;

class CSpriteData;

class CSprite {

public:

	CSprite() {}
	CSprite(CSpriteData* aniData, void (*updateFunc)(CSprite* sprite, void* argv));
	~CSprite();

	void update(void* argv);
	void draw(float redRatio = 1, float blueRatio = 1, float greenRatio = 1);

	void create(int x, int y);
	void die();
	inline bool isLive() {
		return _isLive;
	}

	inline void setAnimation(DWORD index) {
		_currentAnimationIndex = index;
		(*_aniData)[_currentAnimationIndex + (int)_seeRight]->_currentImageIndex = 0;
		(*_aniData)[_currentAnimationIndex + (int)_seeRight]->_doneSingleTime = false;
	}

	inline bool isAnimationLoopOnce() {
		return (*_aniData)[_currentAnimationIndex + (int)_seeRight]->_doneSingleTime;
	}

	CSpriteData* _aniData;

	CImage* _shadow;
	CImage* _hpBar;

	CAnimation _effect;

	int _x;
	int _y;
	bool _seeRight;
	bool _changeAnimation;
	DWORD _currentAnimationIndex;
	bool _isLive;
	USHORT _nowHp;
	USHORT _maxHp;

	INPUT_MESSAGE _oldMsg;
	INPUT_MESSAGE _msg;

	UINT _id;

private:

	DWORD _oldAnimationIndex;

	void (*_update)(CSprite* sprite, void* argv);

};