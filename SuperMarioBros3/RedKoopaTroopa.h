#pragma once
#include "Enemy.h"

#define RED_KOOPA_WALKING_SPEED 0.03f
#define RED_KOOPA_SHELL_RUNNING_SPEED 0.2f
#define RED_KOOPA_BBOX_WIDTH 16
#define RED_KOOPA_BBOX_HEIGHT 26
#define RED_KOOPA_SHELL_BBOX_HEIGHT 16
#define RED_KOOPA_SHELL_IDLE_TIMEOUT 5000 // 5 seconds before reviving
#define RED_KOOPA_GRAVITY 0.002f

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

    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() override;

    void OnCollisionWithBrick(LPCOLLISIONEVENT e);
    void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
    void OnCollisionWithVenusFire(LPCOLLISIONEVENT e);

public:
    RedKoopaTroopa(float x, float y);
    virtual void SetState(int state) override;

    virtual int IsCollidable() { return 1; }
    virtual int IsBlocking() { return state == RED_KOOPA_STATE_SHELL_IDLE || state == RED_KOOPA_STATE_SHELL_RUNNING; }

    void StartShellIdleTimer() { shellIdleStart = GetTickCount64(); }
    void ResetShellIdleTimer() { shellIdleStart = 0; }
    bool IsShellIdleTimeout() { return shellIdleStart > 0 && (GetTickCount64() - shellIdleStart) > RED_KOOPA_SHELL_IDLE_TIMEOUT; }
    int GetDirection() const { return direction; }
    void SetDirection(int dir) { direction = dir; }
    void ReverseDirection() { direction = -direction; vx = direction * abs(vx); }
};