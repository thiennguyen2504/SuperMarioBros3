#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_ANI_LEAF 120003

#define LEAF_BBOX_WIDTH 17
#define LEAF_BBOX_HEIGHT 14

#define LEAF_BOUNCE_SPEED -0.045f
#define LEAF_FALL_SPEED 0.045f
#define LEAF_HORIZONTAL_SPEED 0.1f
#define LEAF_WAVE_FREQUENCY 0.005f
#define LEAF_BOUNCE_DURATION 1000

#define LEAF_STATE_ACTIVE 100

class CLeaf : public CGameObject {
protected:
    float vy;
    float ay;
    float vx;
    ULONGLONG spawnTime;

public:
    CLeaf(float x, float y);
    void Render();
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
    void GetBoundingBox(float& l, float& t, float& r, float& b);
    void OnNoCollision(DWORD dt);
    virtual void OnCollisionWith(LPCOLLISIONEVENT e) override;
    int IsBlocking() { return 0; }
    int IsCollidable() { return 1; }
    bool IsCollidableWith(LPGAMEOBJECT obj);
    virtual void SetState(int state) override;
};