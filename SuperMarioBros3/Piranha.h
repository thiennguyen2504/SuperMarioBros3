#pragma once
#include "Enemy.h"

#define PIRANHA_STATE_HIDDEN 0
#define PIRANHA_STATE_EMERGING 1
#define PIRANHA_STATE_WAITING 2
#define PIRANHA_STATE_RETREATING 3
#define PIRANHA_STATE_DIE 4 

#define PIRANHA_EMERGE_TIME 1000   
#define PIRANHA_WAIT_TIME 2000   
#define PIRANHA_RETREAT_DELAY 2000  
#define PIRANHA_HIDDEN_TIME 2000   
#define PIRANHA_ATTACK_RANGE 300.0f
#define PIRANHA_DIE_TIME 300 

#define PIRANHA_EMERGE_SPEED 0.015f 
#define PIRANHA_RETREAT_SPEED 0.015f
#define PIRANHA_EMERGE_HEIGHT 28.0f 

#define PIRANHA_BBOX_WIDTH 16
#define PIRANHA_BBOX_HEIGHT 24

#define ID_ANI_PIRANHA 10002
#define ID_ANI_PIRANHA_DIE 10003 

class Piranha : public Enemy
{
protected:
    int state;
    ULONGLONG waitTimer;
    ULONGLONG cycleTimer;
    ULONGLONG dieTimer; 
    float baseY;
    float maxEmergeY;

    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() override;
    virtual void OnCollisionWith(LPCOLLISIONEVENT e); 

public:
    Piranha(float x, float y);
    virtual void SetState(int state) override;
};