#pragma once
#include "Enemy.h"
#include "Mario.h"
#include "Goomba.h"

#define RED_PARAGOOMBA_GRAVITY 0.0003f
#define RED_PARAGOOMBA_WALKING_SPEED 0.01f
#define RED_PARAGOOMBA_LOW_JUMP_SPEED -0.08f
#define RED_PARAGOOMBA_HIGH_JUMP_SPEED -0.14f
#define HEADSHOT_JUMP_SPEED 0.1f

#define RED_PARAGOOMBA_BBOX_WIDTH 20
#define RED_PARAGOOMBA_BBOX_HEIGHT 19
#define RED_PARAGOOMBA_BBOX_HEIGHT_FLY 24
#define RED_PARAGOOMBA_BBOX_HEIGHT_DIE 10

#define RED_PARAGOOMBA_HOP_TIMEOUT 800
#define RED_PARAGOOMBA_REST_TIMEOUT 2000

#define RED_PARAGOOMBA_STATE_WALKING 100
#define RED_PARAGOOMBA_STATE_DIE 200
#define RED_PARAGOOMBA_STATE_HOPPING 300
#define RED_PARAGOOMBA_STATE_HEADSHOT 400

#define ID_ANI_RED_PARAGOOMBA_WALK 6001
#define ID_ANI_RED_PARAGOOMBA_FLY 6002
#define ID_ANI_RED_PARAGOOMBA_DIE 6000

class RedParaGoomba : public Enemy
{
protected:
    float ax;
    float ay;

    CMario* target;
    int jumpCount;
    bool isOnPlatform;
    bool isHighJumping;
    ULONGLONG hop_start;
    ULONGLONG rest_start;
    ULONGLONG die_start;

    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() override;

    virtual int IsCollidable() { return state != RED_PARAGOOMBA_STATE_DIE && state != RED_PARAGOOMBA_STATE_HEADSHOT; }
    virtual int IsBlocking() { return 0; }
    virtual void OnNoCollision(DWORD dt) override;
    virtual void OnCollisionWith(LPCOLLISIONEVENT e) override;

public:
    RedParaGoomba(float x, float y);
    virtual void SetState(int state) override;
    void SetTarget(CMario* mario) { target = mario; }
};