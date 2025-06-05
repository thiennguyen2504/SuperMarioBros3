#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_ANI_COIN 11000

#define COIN_WIDTH 10
#define COIN_BBOX_WIDTH 10
#define COIN_BBOX_HEIGHT 16

#define COIN_BOUNCE_SPEED -0.5f
#define COIN_GRAVITY 0.002f
#define COIN_LIFETIME 500

#define COIN_STATE_ACTIVE 100

class CCoin : public CGameObject {
protected:
    float vy;
    float ay;
    ULONGLONG spawnTime;
    BOOLEAN isFromQuestionBlock;
    float startX, startY; // V? trí ban ??u

public:
    CCoin(float x, float y, BOOLEAN fromQuestionBlock = false);
    void Render();
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
    void GetBoundingBox(float& l, float& t, float& r, float& b);
    void OnNoCollision(DWORD dt);
    void OnCollisionWith(LPCOLLISIONEVENT e);
    int IsBlocking() { return 0; }
    int IsCollidable() { return 1; }
};