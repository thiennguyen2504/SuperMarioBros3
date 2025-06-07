#pragma once
#include "Enemy.h"

#define KOOPA_WALKING_SPEED 0.01f
#define KOOPA_SHELL_RUNNING_SPEED 0.08f
#define KOOPA_BBOX_WIDTH 16
#define KOOPA_BBOX_HEIGHT 26
#define KOOPA_SHELL_BBOX_HEIGHT 16
#define KOOPA_SHELL_IDLE_TIMEOUT 5000
#define KOOPA_GRAVITY 0.0005f
#define KOOPA_BOUNCE_COOLDOWN 100
#define KOOPA_BLINK_INTERVAL 100
#define KOOPA_BLINK_DURATION 1000
#define KOOPA_KICK_COOLDOWN 10
#define KOOPA_IDLE_COOLDOWN 200
#define KOOPA_HEADSHOT_JUMP_SPEED -0.12f
#define HEADSHOT_GRAVITY 0.0001f

#define KOOPA_STATE_WALKING 100
#define KOOPA_STATE_SHELL_IDLE 200
#define KOOPA_STATE_SHELL_RUNNING 300
#define KOOPA_STATE_CARRIED 400
#define KOOPA_STATE_HEADSHOT 500

class KoopaTroopa : public Enemy
{
protected:
    float startX, startY; 
    int direction;
    ULONGLONG shellIdleStart;
    bool isOnPlatform;
    bool isPositionFixed;
    float fixedY;
    bool hasBounced;
    ULONGLONG bounceCooldownStart;
    ULONGLONG kickCooldownStart;
    ULONGLONG idleCooldownStart;
    bool isFlipped;
    bool shouldJumpOnHeadshot;

    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() = 0;

    virtual void OnCollisionWith(LPCOLLISIONEVENT e);
    virtual void OnCollisionWithKoopaTroopa(LPCOLLISIONEVENT e);

public:
    KoopaTroopa(float x, float y);
    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void SetState(int state) override;

    virtual int IsCollidable() override;
    virtual int IsBlocking() override { return state == KOOPA_STATE_SHELL_RUNNING || state == KOOPA_STATE_HEADSHOT; }

    void StartShellIdleTimer() { shellIdleStart = GetTickCount64(); }
    void ResetShellIdleTimer() { shellIdleStart = 0; }
    bool IsShellIdleTimeout() { return shellIdleStart > 0 && (GetTickCount64() - shellIdleStart) > KOOPA_SHELL_IDLE_TIMEOUT; }
    int GetDirection() const { return direction; }
    void SetDirection(int dir) { direction = dir; }
    void ReverseDirection() { direction = -direction; vx = direction * abs(vx); }
    bool CanBounce() { return bounceCooldownStart == 0 || (GetTickCount64() - bounceCooldownStart) > KOOPA_BOUNCE_COOLDOWN; }
    bool IsKickCooldownActive() { return kickCooldownStart > 0 && (GetTickCount64() - kickCooldownStart) <= KOOPA_KICK_COOLDOWN; }
    bool IsIdleCooldownActive() { return idleCooldownStart > 0 && (GetTickCount64() - idleCooldownStart) <= KOOPA_IDLE_COOLDOWN; }
    void SetFlipped(bool flipped) { isFlipped = flipped; }
    bool IsFlipped() const { return isFlipped; }
    void SetShouldJumpOnHeadshot(bool jump) { shouldJumpOnHeadshot = jump; }
    float GetStartX() const { return startX; }
    float GetStartY() const { return startY; }
};