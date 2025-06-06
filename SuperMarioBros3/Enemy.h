#pragma once
#include "GameObject.h"

#define ENEMY_GRAVITY 0.002f 
#define ENEMY_DIE_TIMEOUT 500 

#define ENEMY_STATE_WALKING 100
#define ENEMY_STATE_DIE 200

class Enemy : public CGameObject
{
protected:
    float ax;
    float ay;
    ULONGLONG die_start;

    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) = 0;
    virtual void Render() = 0;

    virtual int IsCollidable() { return 1; }
    virtual int IsBlocking() { return 0; }
    virtual void OnNoCollision(DWORD dt);
    virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
    Enemy(float x, float y);
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects);
    virtual void SetState(int state);
};
