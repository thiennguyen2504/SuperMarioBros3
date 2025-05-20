#pragma once
#include "Enemy.h"

#define RED_KOOPA_WALKING_SPEED 0.01f
#define RED_KOOPA_SHELL_RUNNING_SPEED 0.08f
#define RED_KOOPA_BBOX_WIDTH 16
#define RED_KOOPA_BBOX_HEIGHT 26
#define RED_KOOPA_SHELL_BBOX_HEIGHT 16
#define RED_KOOPA_SHELL_IDLE_TIMEOUT 5000
#define RED_KOOPA_GRAVITY 0.001f
#define RED_KOOPA_BOUNCE_COOLDOWN 100
#define RED_KOOPA_BLINK_INTERVAL 100
#define RED_KOOPA_BLINK_DURATION 1000
#define RED_KOOPA_KICK_COOLDOWN 10
#define RED_KOOPA_IDLE_COOLDOWN 200 

#define ID_ANI_RED_KOOPA_WALK_LEFT 200021
#define ID_ANI_RED_KOOPA_WALK_RIGHT 200022
#define ID_ANI_RED_KOOPA_SHELL_IDLE 200023
#define ID_ANI_RED_KOOPA_SHELL_RUN 200024

#define RED_KOOPA_STATE_WALKING 100
#define RED_KOOPA_STATE_SHELL_IDLE 200
#define RED_KOOPA_STATE_SHELL_RUNNING 300
#define RED_KOOPA_STATE_CARRIED 400

class RedKoopaTroopa : public Enemy
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
    virtual void Render() override;

    void OnCollisionWithVenusFire(LPCOLLISIONEVENT e);
    void OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e);
    void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
    void OnCollisionWithRedParaGoomba(LPCOLLISIONEVENT e);
    void OnCollisionWith(LPCOLLISIONEVENT e);

public:
    RedKoopaTroopa(float x, float y);
    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void SetState(int state) override;

    virtual int IsCollidable();
    virtual int IsBlocking() { return state == RED_KOOPA_STATE_SHELL_RUNNING; }

    void StartShellIdleTimer() { shellIdleStart = GetTickCount64(); }
    void ResetShellIdleTimer() { shellIdleStart = 0; }
    bool IsShellIdleTimeout() { return shellIdleStart > 0 && (GetTickCount64() - shellIdleStart) > RED_KOOPA_SHELL_IDLE_TIMEOUT; }
    int GetDirection() const { return direction; }
    void SetDirection(int dir) { direction = dir; }
    void ReverseDirection() { direction = -direction; vx = direction * abs(vx); }
    bool CanBounce() { return bounceCooldownStart == 0 || (GetTickCount64() - bounceCooldownStart) > RED_KOOPA_BOUNCE_COOLDOWN; }
    bool IsKickCooldownActive() { return kickCooldownStart > 0 && (GetTickCount64() - kickCooldownStart) <= RED_KOOPA_KICK_COOLDOWN; }
    bool IsIdleCooldownActive() { return idleCooldownStart > 0 && (GetTickCount64() - idleCooldownStart) <= RED_KOOPA_IDLE_COOLDOWN; }
};