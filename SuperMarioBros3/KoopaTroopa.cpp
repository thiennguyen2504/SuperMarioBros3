#include "KoopaTroopa.h"
#include "Game.h"
#include "Pipe.h"
#include "BrickPlatform.h"
#include "Collision.h"
#include "Mario.h"
#include "PlayScene.h"

KoopaTroopa::KoopaTroopa(float x, float y) : Enemy(x, y)
{
    this->startX = x;
    this->direction = -1;
    this->isOnPlatform = false;
    this->isPositionFixed = false;
    this->hasBounced = false;
    this->bounceCooldownStart = 0;
    this->shellIdleStart = 0;
    this->kickCooldownStart = 0;
    this->idleCooldownStart = 0;
    this->vx = direction * KOOPA_WALKING_SPEED;
    this->vy = 0;
    this->ay = KOOPA_GRAVITY;
    this->isFlipped = false;
    this->shouldJumpOnHeadshot = true;
    SetState(KOOPA_STATE_WALKING);
}

void KoopaTroopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - KOOPA_BBOX_WIDTH / 2;
    top = y - (state == KOOPA_STATE_WALKING ? KOOPA_BBOX_HEIGHT : KOOPA_SHELL_BBOX_HEIGHT) / 2;
    right = left + KOOPA_BBOX_WIDTH;
    bottom = top + (state == KOOPA_STATE_WALKING ? KOOPA_BBOX_HEIGHT : KOOPA_SHELL_BBOX_HEIGHT);
}

void KoopaTroopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    hasBounced = false;

    if (kickCooldownStart > 0 && (GetTickCount64() - kickCooldownStart) > KOOPA_KICK_COOLDOWN)
    {
        kickCooldownStart = 0;
    }

    if (state == KOOPA_STATE_SHELL_IDLE && isPositionFixed)
    {
        y = fixedY;
        vx = 0;
        vy = 0;
    }
    else if (state == KOOPA_STATE_CARRIED)
    {
        // Skip velocity and position updates when carried
    }
    else
    {
        vy += ay * dt;

        if (state == KOOPA_STATE_WALKING)
        {
            vx = direction * KOOPA_WALKING_SPEED;
        }
        else if (state == KOOPA_STATE_SHELL_RUNNING)
        {
            vx = direction * KOOPA_SHELL_RUNNING_SPEED;
        }

        Enemy::Update(dt, coObjects);
    }

    if (state == KOOPA_STATE_SHELL_IDLE || state == KOOPA_STATE_CARRIED)
    {
        if (IsShellIdleTimeout())
        {
            SetState(KOOPA_STATE_WALKING);
            ResetShellIdleTimer();
        }
    }
    else if (state == KOOPA_STATE_HEADSHOT)
    {
        if (y > CGame::GetInstance()->GetBackBufferHeight())
        {
            isDeleted = true;
        }
    }

    if (state != KOOPA_STATE_HEADSHOT || idleCooldownStart == 0 || GetTickCount64() - idleCooldownStart > 100) 
    {
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
                    rightEdge > objLeft && leftEdge < objRight && vy >= 0) 
                {
                    isOnPlatform = true;
                    vy = 0;
                    y = objTop - (state == KOOPA_STATE_WALKING ? KOOPA_BBOX_HEIGHT : KOOPA_SHELL_BBOX_HEIGHT) / 2 - 0.1f;
                    if (state == KOOPA_STATE_SHELL_IDLE)
                    {
                        fixedY = y;
                        isPositionFixed = true;
                    }
                    break;
                }
            }
        }
    }

    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void KoopaTroopa::SetState(int state)
{
    int previousState = this->state;
    Enemy::SetState(state);
    switch (state)
    {
    case KOOPA_STATE_WALKING:
        vx = direction * KOOPA_WALKING_SPEED;
        vy = 0;
        ay = KOOPA_GRAVITY;
        isPositionFixed = false;
        idleCooldownStart = 0;
        isFlipped = false;
        if (previousState == KOOPA_STATE_CARRIED)
        {
            CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
            if (mario && mario->GetHeldKoopa() == this)
            {
                mario->OnHitByKoopa();
                mario->SetHeldKoopa(nullptr);
            }
        }
        break;
    case KOOPA_STATE_SHELL_IDLE:
        vx = 0;
        vy = 0;
        ay = 0;
        StartShellIdleTimer();
        idleCooldownStart = GetTickCount64();
        break;
    case KOOPA_STATE_SHELL_RUNNING:
        vx = direction * KOOPA_SHELL_RUNNING_SPEED;
        vy = 0;
        ay = KOOPA_GRAVITY;
        ResetShellIdleTimer();
        isPositionFixed = false;
        idleCooldownStart = 0;
        kickCooldownStart = GetTickCount64();
        break;
    case KOOPA_STATE_CARRIED:
        vx = 0;
        vy = 0;
        ay = 0;
        StartShellIdleTimer();
        isPositionFixed = false;
        idleCooldownStart = 0;
        break;
    case KOOPA_STATE_HEADSHOT:
        y += (KOOPA_BBOX_HEIGHT - KOOPA_SHELL_BBOX_HEIGHT) / 2;
        vy = KOOPA_HEADSHOT_JUMP_SPEED;
        vx = 0;
        ay = HEADSHOT_GRAVITY;
        ResetShellIdleTimer();
        isPositionFixed = false;
        idleCooldownStart = GetTickCount64();
        kickCooldownStart = GetTickCount64();
        isFlipped = true;
        break;
    }
}

void KoopaTroopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (e->obj->IsBlocking())
    {
        if (e->ny > 0)
        {
            vy = 0;
            y += e->t * vy + e->ny * 0.4f;
            if (state == KOOPA_STATE_SHELL_IDLE)
            {
                fixedY = y;
                isPositionFixed = true;
            }
        }
        if (e->nx != 0 && (state == KOOPA_STATE_SHELL_RUNNING || state == KOOPA_STATE_HEADSHOT) && !hasBounced && CanBounce())
        {
            bool shouldBounce = dynamic_cast<CPipe*>(e->obj) != nullptr || dynamic_cast<CBrickPlatform*>(e->obj) != nullptr;
            if (shouldBounce)
            {
                float objLeft, objTop, objRight, objBottom;
                e->obj->GetBoundingBox(objLeft, objTop, objRight, objBottom);
                ReverseDirection();
                if (e->nx > 0)
                {
                    x = objRight + KOOPA_BBOX_WIDTH / 2 + 2.0f;
                }
                else
                {
                    x = objLeft - KOOPA_BBOX_WIDTH / 2 - 2.0f;
                }
                hasBounced = true;
                bounceCooldownStart = GetTickCount64();
            }
        }
    }

    if (dynamic_cast<KoopaTroopa*>(e->obj))
        OnCollisionWithKoopaTroopa(e);
}

void KoopaTroopa::OnCollisionWithKoopaTroopa(LPCOLLISIONEVENT e)
{
    KoopaTroopa* otherKoopa = dynamic_cast<KoopaTroopa*>(e->obj);
    if (state == KOOPA_STATE_SHELL_RUNNING && otherKoopa->GetState() != KOOPA_STATE_HEADSHOT)
    {
        otherKoopa->SetState(KOOPA_STATE_HEADSHOT);
    }
}

int KoopaTroopa::IsCollidable()
{
    if ((state == KOOPA_STATE_SHELL_RUNNING || state == KOOPA_STATE_HEADSHOT) && IsKickCooldownActive())
    {
        return 0;
    }
    return 1;
}