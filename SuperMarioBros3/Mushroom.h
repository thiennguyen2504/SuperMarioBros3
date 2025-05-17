#pragma once
#include "GameObject.h"

#define MUSHROOM_GRAVITY 0.002f
#define MUSHROOM_WALKING_SPEED 0.05f
#define MUSHROOM_EMERGING_SPEED -0.015f
#define MUSHROOM_BBOX_WIDTH 16
#define MUSHROOM_BBOX_HEIGHT 16
#define QUESTION_BLOCK_BBOX_HEIGHT 16

#define MUSHROOM_STATE_EMERGING 50
#define MUSHROOM_STATE_WALKING 100

#define ID_ANI_MUSHROOM 120002

class CMushroom : public CGameObject {
private:
    bool isMovingUp;
    float startY;
    float ax;
    float ay;

public:
    CMushroom(float x, float y);
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    void Render() override;
    void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    void SetState(int state) override;
    int IsCollidable() { return 1; }
    int IsBlocking() { return 0; }
    void OnNoCollision(DWORD dt) override;
    void OnCollisionWith(LPCOLLISIONEVENT e) override;
};