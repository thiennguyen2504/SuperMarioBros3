#pragma once
#include "Enemy.h"

#define RED_KOOPA_WALKING_SPEED 0.03f
#define RED_KOOPA_SHELL_RUNNING_SPEED 0.1f
#define RED_KOOPA_BBOX_WIDTH 16
#define RED_KOOPA_BBOX_HEIGHT 26
#define RED_KOOPA_SHELL_BBOX_HEIGHT 16
#define RED_KOOPA_SHELL_IDLE_TIMEOUT 5000 // 5 seconds before reviving
#define RED_KOOPA_GRAVITY 0.002f
#define RED_KOOPA_BOUNCE_COOLDOWN 100 // 100ms cooldown after bouncing

#define RED_KOOPA_STATE_WALKING 100
#define RED_KOOPA_STATE_SHELL_IDLE 200
#define RED_KOOPA_STATE_SHELL_RUNNING 300
#define RED_KOOPA_STATE_CARRIED 400

class RedKoopaTroopa : public Enemy
{
protected:
    float startX;
    int direction; // 1 for right, -1 for left
    ULONGLONG shellIdleStart; // Time when shell becomes idle or carried
    bool isOnPlatform;
    bool isPositionFixed; // Flag to indicate if position should be fixed
    float fixedY; // Store the fixed Y position when in SHELL_IDLE
    bool hasBounced; // Flag to prevent multiple bounces in a single frame
    ULONGLONG bounceCooldownStart; // Time when the last bounce occurred

    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() override;

    void OnCollisionWithBrick(LPCOLLISIONEVENT e);
    void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
    void OnCollisionWithVenusFire(LPCOLLISIONEVENT e);
    void OnCollisionWith(LPCOLLISIONEVENT e);

public:
    RedKoopaTroopa(float x, float y);
    virtual void SetState(int state) override;

    virtual int IsCollidable() { return 1; }
    virtual int IsBlocking() { return state == RED_KOOPA_STATE_SHELL_RUNNING; } // Only block when running

    void StartShellIdleTimer() { shellIdleStart = GetTickCount64(); }
    void ResetShellIdleTimer() { shellIdleStart = 0; }
    bool IsShellIdleTimeout() { return shellIdleStart > 0 && (GetTickCount64() - shellIdleStart) > RED_KOOPA_SHELL_IDLE_TIMEOUT; }
    int GetDirection() const { return direction; }
    void SetDirection(int dir) { direction = dir; }
    void ReverseDirection() { direction = -direction; vx = direction * abs(vx); }
    bool CanBounce() { return bounceCooldownStart == 0 || (GetTickCount64() - bounceCooldownStart) > RED_KOOPA_BOUNCE_COOLDOWN; }
};