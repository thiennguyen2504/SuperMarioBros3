#include "GreenKoopaTroopa.h"
#include "Game.h"
#include "Pipe.h"
#include "BrickPlatform.h"
#include "Collision.h"
#include "Mario.h"
#include "PlayScene.h"

GreenKoopaTroopa::GreenKoopaTroopa(float x, float y) : Enemy(x, y)
{
    this->startX = x;
    this->direction = -1; // Di chuyển qua trái
    this->isOnPlatform = false;
    this->isPositionFixed = false;
    this->hasBounced = false;
    this->bounceCooldownStart = 0;
    this->shellIdleStart = 0;
    this->kickCooldownStart = 0;
    this->idleCooldownStart = 0;
    this->vx = direction * GREEN_KOOPA_WALKING_SPEED;
    this->vy = 0;
    this->ay = GREEN_KOOPA_GRAVITY;
    SetState(GREEN_KOOPA_STATE_WALKING);
}

void GreenKoopaTroopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - GREEN_KOOPA_BBOX_WIDTH / 2;
    top = y - (state == GREEN_KOOPA_STATE_WALKING ? GREEN_KOOPA_BBOX_HEIGHT : GREEN_KOOPA_SHELL_BBOX_HEIGHT) / 2;
    right = left + GREEN_KOOPA_BBOX_WIDTH;
    bottom = top + (state == GREEN_KOOPA_STATE_WALKING ? GREEN_KOOPA_BBOX_HEIGHT : GREEN_KOOPA_SHELL_BBOX_HEIGHT);
}

void GreenKoopaTroopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    hasBounced = false;

    if (kickCooldownStart > 0 && (GetTickCount64() - kickCooldownStart) > GREEN_KOOPA_KICK_COOLDOWN)
    {
        kickCooldownStart = 0;
    }

    if (state == GREEN_KOOPA_STATE_SHELL_IDLE && isPositionFixed)
    {
        y = fixedY;
        vx = 0;
        vy = 0;
    }
    else if (state == GREEN_KOOPA_STATE_CARRIED)
    {
        // Skip velocity and position updates when carried
    }
    else
    {
        vy += ay * dt;

        if (state == GREEN_KOOPA_STATE_WALKING)
        {
            vx = direction * GREEN_KOOPA_WALKING_SPEED;
        }
        else if (state == GREEN_KOOPA_STATE_SHELL_RUNNING)
        {
            vx = direction * GREEN_KOOPA_SHELL_RUNNING_SPEED;
        }

        Enemy::Update(dt, coObjects);
    }

    if (state == GREEN_KOOPA_STATE_SHELL_IDLE || state == GREEN_KOOPA_STATE_CARRIED)
    {
        if (IsShellIdleTimeout())
        {
            SetState(GREEN_KOOPA_STATE_WALKING);
            ResetShellIdleTimer();
        }
    }

    float leftEdge, topEdge, rightEdge, bottomEdge;
    GetBoundingBox(leftEdge, topEdge, rightEdge, bottomEdge);
    isOnPlatform = false;
    for (size_t i = 0; i < coObjects->size(); i++)
    {
        LPGAMEOBJECT obj = coObjects->at(i);
        if (obj->IsBlocking())
        {
            float objLeft, objTop, objRight, objBottom;
            obj->GetBoundingBox(objLeft, objTop, objRight, objBottom);

            if (bottomEdge >= objTop - 8 && bottomEdge <= objTop + 8 &&
                rightEdge > objLeft && leftEdge < objRight)
            {
                isOnPlatform = true;
                vy = 0;
                y = objTop - (state == GREEN_KOOPA_STATE_WALKING ? GREEN_KOOPA_BBOX_HEIGHT : GREEN_KOOPA_SHELL_BBOX_HEIGHT) / 2 - 0.1f;
                if (state == GREEN_KOOPA_STATE_SHELL_IDLE)
                {
                    fixedY = y;
                    isPositionFixed = true;
                }
                break;
            }
        }
    }

    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void GreenKoopaTroopa::Render()
{
    int aniId = -1;
    if (state == GREEN_KOOPA_STATE_WALKING)
    {
        aniId = ID_ANI_GREEN_KOOPA_WALK_LEFT;
    }
    else if (state == GREEN_KOOPA_STATE_SHELL_IDLE || state == GREEN_KOOPA_STATE_CARRIED)
    {
        aniId = ID_ANI_GREEN_KOOPA_SHELL_IDLE;
    }
    else if (state == GREEN_KOOPA_STATE_SHELL_RUNNING)
    {
        aniId = ID_ANI_GREEN_KOOPA_SHELL_RUN;
    }

    bool shouldRender = true;
    if (state == GREEN_KOOPA_STATE_SHELL_IDLE || state == GREEN_KOOPA_STATE_CARRIED)
    {
        if (shellIdleStart > 0)
        {
            ULONGLONG timeSinceIdle = GetTickCount64() - shellIdleStart;
            ULONGLONG timeUntilRevive = GREEN_KOOPA_SHELL_IDLE_TIMEOUT - timeSinceIdle;
            if (timeUntilRevive <= GREEN_KOOPA_BLINK_DURATION)
            {
                ULONGLONG blinkPhase = timeSinceIdle % (2 * GREEN_KOOPA_BLINK_INTERVAL);
                shouldRender = (blinkPhase < GREEN_KOOPA_BLINK_INTERVAL);
            }
        }
    }

    CAnimations* animations = CAnimations::GetInstance();
    if (shouldRender && aniId != -1)
    {
        animations->Get(aniId)->Render(x, y);
    }
}

void GreenKoopaTroopa::SetState(int state)
{
    int previousState = this->state;
    Enemy::SetState(state);
    switch (state)
    {
    case GREEN_KOOPA_STATE_WALKING:
        vx = direction * GREEN_KOOPA_WALKING_SPEED;
        vy = 0;
        ay = GREEN_KOOPA_GRAVITY;
        isPositionFixed = false;
        idleCooldownStart = 0;
        if (previousState == GREEN_KOOPA_STATE_CARRIED)
        {
            CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
            if (mario && mario->GetHeldKoopa() == this)
            {
                mario->OnHitByKoopa();
                mario->SetHeldKoopa(nullptr);
            }
        }
        break;
    case GREEN_KOOPA_STATE_SHELL_IDLE:
        vx = 0;
        vy = 0;
        ay = 0;
        StartShellIdleTimer();
        idleCooldownStart = GetTickCount64();
        break;
    case GREEN_KOOPA_STATE_SHELL_RUNNING:
        vx = direction * GREEN_KOOPA_SHELL_RUNNING_SPEED;
        vy = 0;
        ay = GREEN_KOOPA_GRAVITY;
        ResetShellIdleTimer();
        isPositionFixed = false;
        idleCooldownStart = 0;
        kickCooldownStart = GetTickCount64();
        break;
    case GREEN_KOOPA_STATE_CARRIED:
        vx = 0;
        vy = 0;
        ay = 0;
        StartShellIdleTimer();
        isPositionFixed = false;
        idleCooldownStart = 0;
        break;
    }
}

void GreenKoopaTroopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (e->obj->IsBlocking())
    {
        if (e->ny > 0)
        {
            vy = 0;
            y += e->t * vy + e->ny * 0.4f;
            if (state == GREEN_KOOPA_STATE_SHELL_IDLE)
            {
                fixedY = y;
                isPositionFixed = true;
            }
        }
        if (e->nx != 0 && state == GREEN_KOOPA_STATE_SHELL_RUNNING && !hasBounced && CanBounce())
        {
            bool shouldBounce = dynamic_cast<CPipe*>(e->obj) != nullptr || dynamic_cast<CBrickPlatform*>(e->obj) != nullptr;
            if (shouldBounce)
            {
                float objLeft, objTop, objRight, objBottom;
                e->obj->GetBoundingBox(objLeft, objTop, objRight, objBottom);
                ReverseDirection();
                if (e->nx > 0)
                {
                    x = objRight + GREEN_KOOPA_BBOX_WIDTH / 2 + 2.0f;
                }
                else
                {
                    x = objLeft - GREEN_KOOPA_BBOX_WIDTH / 2 - 2.0f;
                }
                hasBounced = true;
                bounceCooldownStart = GetTickCount64();
            }
        }
    }
}

int GreenKoopaTroopa::IsCollidable()
{
    if (state == GREEN_KOOPA_STATE_SHELL_RUNNING && IsKickCooldownActive())
    {
        return 0;
    }
    return 1;
}