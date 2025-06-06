#pragma once
#include "GameObject.h"

#define GOLD_BRICK_BBOX_WIDTH 16
#define GOLD_BRICK_BBOX_HEIGHT 16

#define GOLD_BRICK_STATE_ACTIVE 100
#define GOLD_BRICK_STATE_DESTROYED 200

#define ID_ANI_GOLD_BRICK 9999

#define GOLD_BRICK_TYPE_NORMAL 0
#define GOLD_BRICK_TYPE_SPECIAL 1

class GoldBrick : public CGameObject
{
protected:
    int type;
    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() override;
    virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
    GoldBrick(float x, float y, int type);
    virtual void SetState(int state) override;
    virtual int IsCollidable() { return state == GOLD_BRICK_STATE_ACTIVE; }
    virtual int IsBlocking(LPGAMEOBJECT obj = nullptr);
};