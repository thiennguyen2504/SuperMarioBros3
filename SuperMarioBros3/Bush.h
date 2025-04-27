#pragma once
#include "GameObject.h"

class CBush : public CGameObject {
    float scale;
public:
    CBush(float x, float y, float scale = 0.5f);
    void Render() override;
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    void GetBoundingBox(float& l, float& t, float& r, float& b) override;
    int IsCollidable() override { return 0; }
    int IsBlocking() override { return 0; }
};