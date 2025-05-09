#pragma once
#include "GameObject.h"

#define FIREBALL_SPEED 0.1f
#define FIREBALL_BBOX_WIDTH 8
#define FIREBALL_BBOX_HEIGHT 9

#define ID_ANI_FIREBALL 10001

class Fireball : public CGameObject
{
protected:
    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() override;

public:
    Fireball(float x, float y, float targetX, float targetY);
    virtual int IsCollidable() { return 1; }
    virtual int IsBlocking() { return 0; }
};