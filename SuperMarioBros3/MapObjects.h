#pragma once
#include "GameObject.h"

#define OBJECT_SCALE 0.33f
class CMapObjects : public CGameObject
{
protected:
    int spriteId; 

public:
    CMapObjects(float x, float y, int spriteId) : CGameObject(x, y), spriteId(spriteId) {}
    void Render() override;
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override {}
    void GetBoundingBox(float& left, float& top, float& right, float& bottom) override {}
    int IsCollidable() override { return 0; }
    int IsBlocking() override { return 0; }
};