#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"
 
#define ID_ANI_BRICK 10000
#define ID_ANI_HIDDEN_BRICK 20013
#define BRICK_WIDTH 16
#define BRICK_BBOX_WIDTH 16
#define BRICK_BBOX_HEIGHT 16
#define BRICK_TYPE_NORMAL 0
#define BRICK_TYPE_HIDDEN 1
class CBrick : public CGameObject {
	int type;
public:
	CBrick(float x, float y, int type) : CGameObject(x, y) {
		type = type;
	}
	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
};