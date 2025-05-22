#pragma once
#include "Enemy.h"

#define GREEN_KOOPA_WALKING_SPEED 0.01f
#define GREEN_KOOPA_SHELL_RUNNING_SPEED 0.08f
#define GREEN_KOOPA_BBOX_WIDTH 16
#define GREEN_KOOPA_BBOX_HEIGHT 26
#define GREEN_KOOPA_SHELL_BBOX_HEIGHT 16
#define GREEN_KOOPA_SHELL_IDLE_TIMEOUT 5000
#define GREEN_KOOPA_GRAVITY 0.001f
#define GREEN_KOOPA_BOUNCE_COOLDOWN 100
#define GREEN_KOOPA_BLINK_INTERVAL 100
#define GREEN_KOOPA_BLINK_DURATION 1000
#define GREEN_KOOPA_KICK_COOLDOWN 10
#define GREEN_KOOPA_IDLE_COOLDOWN 200

#define ID_ANI_GREEN_KOOPA_WALK_LEFT 200031
#define ID_ANI_GREEN_KOOPA_SHELL_IDLE 200033
#define ID_ANI_GREEN_KOOPA_SHELL_RUN 200034

#define GREEN_KOOPA_STATE_WALKING 100
#define GREEN_KOOPA_STATE_SHELL_IDLE 200
#define GREEN_KOOPA_STATE_SHELL_RUNNING 300
#define GREEN_KOOPA_STATE_CARRIED 400

class GreenKoopaTroopa : public Enemy
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

    void OnCollisionWith(LPCOLLISIONEVENT e);

public:
    GreenKoopaTroopa(float x, float y);
    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void SetState(int state) override;

    virtual int IsCollidable();
    virtual int IsBlocking() { return state == GREEN_KOOPA_STATE_SHELL_RUNNING; }

    void StartShellIdleTimer() { shellIdleStart = GetTickCount64(); }
    void ResetShellIdleTimer() { shellIdleStart = 0; }
    bool IsShellIdleTimeout() { return shellIdleStart > 0 && (GetTickCount64() - shellIdleStart) > GREEN_KOOPA_SHELL_IDLE_TIMEOUT; }
    int GetDirection() const { return direction; }
    void SetDirection(int dir) { direction = dir; }
    void ReverseDirection() { direction = -direction; vx = direction * abs(vx); }
    bool CanBounce() { return bounceCooldownStart == 0 || (GetTickCount64() - bounceCooldownStart) > GREEN_KOOPA_BOUNCE_COOLDOWN; }
    bool IsKickCooldownActive() { return kickCooldownStart > 0 && (GetTickCount64() - kickCooldownStart) <= GREEN_KOOPA_KICK_COOLDOWN; }
    bool IsIdleCooldownActive() { return idleCooldownStart > 0 && (GetTickCount64() - idleCooldownStart) <= GREEN_KOOPA_IDLE_COOLDOWN; }
};