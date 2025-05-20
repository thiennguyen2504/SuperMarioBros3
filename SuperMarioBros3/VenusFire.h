#pragma once
#include "Enemy.h"

#define VENUS_FIRE_STATE_HIDDEN 0
#define VENUS_FIRE_STATE_EMERGING 1
#define VENUS_FIRE_STATE_WAITING 2
#define VENUS_FIRE_STATE_RETREATING 3

#define VENUS_FIRE_EMERGE_TIME 1000   
#define VENUS_FIRE_WAIT_TIME 2000   
#define VENUS_FIRE_RETREAT_DELAY 2000  
#define VENUS_FIRE_HIDDEN_TIME 2000   
#define VENUS_FIRE_ATTACK_RANGE 300.0f

#define VENUS_FIRE_EMERGE_SPEED 0.015f 
#define VENUS_FIRE_RETREAT_SPEED 0.03f
#define VENUS_FIRE_EMERGE_HEIGHT 38.0f 

class VenusFire : public Enemy
{
protected:
    int color;
    int spriteIdLeftDown, spriteIdLeftUp, spriteIdRightDown, spriteIdRightUp;
    bool isFacingRight;
    bool isMarioAbove;
    bool hasFired; 
    int state;
    ULONGLONG moveTimer;
    ULONGLONG waitTimer;
    ULONGLONG cycleTimer;
    float baseY;
    float targetY;
    float maxEmergeY;

    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() override;

public:
    VenusFire(float x, float y, int color,
        int spriteIdLeftDown, int spriteIdLeftUp, int spriteIdRightDown, int spriteIdRightUp);
    virtual void SetState(int state) override;
};