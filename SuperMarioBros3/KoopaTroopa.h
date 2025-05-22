#pragma once
#include "Enemy.h"

#define KOOPA_WALKING_SPEED 0.01f
#define KOOPA_SHELL_RUNNING_SPEED 0.08f
#define KOOPA_BBOX_WIDTH 16
#define KOOPA_BBOX_HEIGHT 26
#define KOOPA_SHELL_BBOX_HEIGHT 16
#define KOOPA_SHELL_IDLE_TIMEOUT 5000
#define KOOPA_GRAVITY 0.001f
#define KOOPA_BOUNCE_COOLDOWN 100
#define KOOPA_BLINK_INTERVAL 100
#define KOOPA_BLINK_DURATION 1000
#define KOOPA_KICK_COOLDOWN 10
#define KOOPA_IDLE_COOLDOWN 200

#define KOOPA_STATE_WALKING 100
#define KOOPA_STATE_SHELL_IDLE 200
#define KOOPA_STATE_SHELL_RUNNING 300
#define KOOPA_STATE_CARRIED 400

class KoopaTroopa : public Enemy
{
protected:
    float startX;
    int direction;
    ULONGLONG shellIdleStart;
    bool isOnPlatform;
    bool isPositionFixed;
    float fixedY;
    bool hasBounced;
    ULONGLONG bounceCooldownStart;
    ULONGLONG kickCooldownStart;
    ULONGLONG idleCooldownStart;

    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() = 0; 

    virtual void OnCollisionWith(LPCOLLISIONEVENT e);

public:
    KoopaTroopa(float x, float y);
    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void SetState(int state) override;

    virtual int IsCollidable() override;
    virtual int IsBlocking() override { return state == KOOPA_STATE_SHELL_RUNNING; }

    void StartShellIdleTimer() { shellIdleStart = GetTickCount64(); }
    void ResetShellIdleTimer() { shellIdleStart = 0; }
    bool IsShellIdleTimeout() { return shellIdleStart > 0 && (GetTickCount64() - shellIdleStart) > KOOPA_SHELL_IDLE_TIMEOUT; }
    int GetDirection() const { return direction; }
    void SetDirection(int dir) { direction = dir; }
    void ReverseDirection() { direction = -direction; vx = direction * abs(vx); }
    bool CanBounce() { return bounceCooldownStart == 0 || (GetTickCount64() - bounceCooldownStart) > KOOPA_BOUNCE_COOLDOWN; }
    bool IsKickCooldownActive() { return kickCooldownStart > 0 && (GetTickCount64() - kickCooldownStart) <= KOOPA_KICK_COOLDOWN; }
    bool IsIdleCooldownActive() { return idleCooldownStart > 0 && (GetTickCount64() - idleCooldownStart) <= KOOPA_IDLE_COOLDOWN; }
};