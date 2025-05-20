#pragma once
#include "Enemy.h"

#define GOOMBA_WALKING_SPEED 0.01f
#define GOOMBA_GRAVITY 0.002f
#define HEADSHOT_JUMP_SPEED 0.3f

#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT 14
#define GOOMBA_BBOX_HEIGHT_DIE 7

#define GOOMBA_STATE_WALKING ENEMY_STATE_WALKING
#define GOOMBA_STATE_DIE ENEMY_STATE_DIE
#define GOOMBA_STATE_HEADSHOT 400

#define ID_ANI_GOOMBA_WALKING 5000
#define ID_ANI_GOOMBA_DIE 5001

#define ID_ANI_RED_GOOMBA_WALK 7000
#define ID_ANI_RED_GOOMBA_DIE 7001

#define GOOMBA_TYPE_NORMAL 0
#define GOOMBA_TYPE_RED 1

class CGoomba : public Enemy
{
protected:
    int type; // 0: Goomba, 1: Red Goomba
    bool isOnPlatform;
    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() override;
    virtual void OnNoCollision(DWORD dt) override;

public:
    CGoomba(float x, float y, int type = GOOMBA_TYPE_NORMAL);
    virtual void SetState(int state) override;
    virtual int IsCollidable() { return state != GOOMBA_STATE_DIE && state != GOOMBA_STATE_HEADSHOT; }
    virtual int IsBlocking() { return 0; }
    virtual void OnCollisionWith(LPCOLLISIONEVENT e) override;
};