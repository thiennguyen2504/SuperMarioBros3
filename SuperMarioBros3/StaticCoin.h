#pragma once
#include "GameObject.h"

#define STATIC_COIN_BBOX_WIDTH 14
#define STATIC_COIN_BBOX_HEIGHT 16

class StaticCoin : public CGameObject
{
protected:
    int spriteId;
public:
    StaticCoin(float x, float y, int spriteId);
    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() override;
    virtual int IsCollidable() { return 1; }
    virtual int IsBlocking() { return 0; }
};