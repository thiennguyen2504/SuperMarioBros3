#pragma once
#include "GameObject.h"
#include "Sprites.h"

#define EFFECT_VY -0.05f 
#define EFFECT_DURATION	500   

class CEffect : public CGameObject
{
protected:
	int point;
	ULONGLONG startTime;

public:
	CEffect(float x, float y, int point);
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
	virtual void Render();
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override {}
};

typedef CEffect* LPEFFECT;