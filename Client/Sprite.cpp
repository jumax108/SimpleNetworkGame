#include <Windows.h>

#include "Image.h"
#include "Animation.h"
#include "SpriteData.h"

#include "Sprite.h"

CSprite::CSprite(CSpriteData* aniData, void (*updateFunc)(CSprite* sprite, void* argv)) {

	_aniData = new CSpriteData();
	*_aniData = *aniData;

	_x = -1;
	_y = -1;

	_isLive = false;

	_update = updateFunc;
	_currentAnimationIndex = 0;
	_oldAnimationIndex = 0;
	_seeRight = false;
	_changeAnimation = false;

	_shadow = nullptr;

	_msg = INPUT_MESSAGE::NONE;
	_oldMsg = INPUT_MESSAGE::NONE;

	_effect._doneSingleTime = true;
}

CSprite::~CSprite() {

	_isLive = false;
	_id = 0;
	_x = -1;
	_y = -1;
	_update = nullptr;
	_currentAnimationIndex = 0;
	_oldAnimationIndex = 0;

}

void CSprite::update(void* argv) {

	if(_update != nullptr)
		_update(this, argv);

	(*_aniData)[_currentAnimationIndex + (int)_seeRight]->update();
	if (_oldAnimationIndex == _currentAnimationIndex) {
		_changeAnimation = false;
	}
	else {
		_oldAnimationIndex = _currentAnimationIndex;
		_changeAnimation = true;
		(*_aniData)[_currentAnimationIndex + (int)_seeRight]->_doneSingleTime = false;
	}

	if (_effect._doneSingleTime == false) {
		_effect.update();
	}

}

void CSprite::create(int x, int y) {

	_x = x;
	_y = y;
	_isLive = true;

}

void CSprite::draw(float redRatio, float blueRatio, float greenRatio) {

	int printX = _x - rtCamera.left;
	int printY = _y - rtCamera.top;

	if (_shadow != nullptr) {
		_shadow->draw(printX, printY);
	}
	if (_hpBar != nullptr) {
		_hpBar->draw(printX, printY, (float)_nowHp / _maxHp);
	}
	if (_effect._doneSingleTime == false) {
		_effect.draw(printX, printY);
	}
	(*_aniData)[_currentAnimationIndex + (int)_seeRight]->draw(printX, printY, redRatio, blueRatio, greenRatio);


}

void CSprite::die() {
	_isLive = false;
}