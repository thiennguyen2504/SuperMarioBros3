#pragma once
#include "GameObject.h"

#define P_BUTTON_BBOX_WIDTH 16
#define P_BUTTON_BBOX_HEIGHT 16

#define P_BUTTON_STATE_NORMAL 100
#define P_BUTTON_STATE_PRESSED 200

#define ID_ANI_P_BUTTON_NORMAL 9997
#define ID_ANI_P_BUTTON_PRESSED 9996

class PButton : public CGameObject
{
protected:
	virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
	virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
	virtual void Render() override;

public:
	PButton(float x, float y);
	virtual void SetState(int state) override;
	virtual int IsCollidable() { return 1; }
	virtual int IsBlocking() { return 0; }
};