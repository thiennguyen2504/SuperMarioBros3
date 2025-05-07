#pragma once
#include "Enemy.h"

#define VENUS_FIRE_STATE_HIDDEN 0
#define VENUS_FIRE_STATE_EMERGING 1
#define VENUS_FIRE_STATE_ATTACKING 2
#define VENUS_FIRE_STATE_RETREATING 3

#define VENUS_FIRE_EMERGE_TIME 1000 // Thời gian thò ra (ms)
#define VENUS_FIRE_ATTACK_TIME 500  // Thời gian dừng để bắn (ms)
#define VENUS_FIRE_RETREAT_TIME 1000 // Thời gian rút vào (ms)
#define VENUS_FIRE_FIREBALL_COOLDOWN 1500 // Thời gian giữa các lần bắn (ms)
#define VENUS_FIRE_ATTACK_RANGE 500 // Khoảng cách để kích hoạt tấn công (pixel)

class VenusFire : public Enemy
{
protected:
    int color; // 0: đỏ, 1: xanh
    int spriteIdLeftDown, spriteIdLeftUp, spriteIdRightDown, spriteIdRightUp;
    bool isFacingRight; // Hướng thay đổi động dựa trên vị trí Mario (trục X)
    bool isMarioAbove;  // Hướng thay đổi động dựa trên vị trí Mario (trục Y)
    int state;
    ULONGLONG moveTimer;
    ULONGLONG fireballTimer;
    float baseY; // Vị trí y ban đầu (ẩn trong ống)
    float targetY; // Vị trí y mục tiêu khi di chuyển

    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() override;

    virtual void OnCollisionWith(LPCOLLISIONEVENT e) override;

public:
    VenusFire(float x, float y, int color,
        int spriteIdLeftDown, int spriteIdLeftUp, int spriteIdRightDown, int spriteIdRightUp);
    virtual void SetState(int state) override;
};