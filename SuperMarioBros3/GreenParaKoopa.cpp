#include "GreenParaKoopa.h"
#include "Game.h"
#include "Collision.h"
#include "debug.h"
#include "GreenKoopaTroopa.h"

GreenParaKoopa::GreenParaKoopa(float x, float y) : Enemy(x, y)
{
    this->ax = 0;
    this->ay = GREEN_PARAKOOPA_GRAVITY;
    this->vx = -GREEN_PARAKOOPA_WALKING_SPEED;
    this->isOnPlatform = false;
    this->hover_start = -1;
    this->die_start = -1;
    SetState(GREEN_PARAKOOPA_STATE_JUMPING);
}

void GreenParaKoopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    if (state == GREEN_PARAKOOPA_STATE_DIE || state == GREEN_PARAKOOPA_STATE_HEADSHOT)
    {
        left = x - GREEN_PARAKOOPA_BBOX_WIDTH / 2;
        top = y - GREEN_PARAKOOPA_BBOX_HEIGHT_DIE / 2;
        right = left + GREEN_PARAKOOPA_BBOX_WIDTH;
        bottom = top + GREEN_PARAKOOPA_BBOX_HEIGHT_DIE;
    }
    else
    {
        left = x - GREEN_PARAKOOPA_BBOX_WIDTH / 2;
        top = y - GREEN_PARAKOOPA_BBOX_HEIGHT / 2;
        right = left + GREEN_PARAKOOPA_BBOX_WIDTH;
        bottom = top + GREEN_PARAKOOPA_BBOX_HEIGHT;
    }
}

void GreenParaKoopa::OnNoCollision(DWORD dt)
{
    x += vx * dt;
    y += vy * dt;
}

void GreenParaKoopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    vy += ay * dt;
    vx += ax * dt;

    if (state == GREEN_PARAKOOPA_STATE_DIE && GetTickCount64() - die_start > ENEMY_DIE_TIMEOUT)
    {
        Delete();
        return;
    }
    else if (state == GREEN_PARAKOOPA_STATE_HEADSHOT)
    {
        if (y > CGame::GetInstance()->GetBackBufferHeight())
        {
            isDeleted = true;
            return;
        }
    }

    isOnPlatform = false;
    for (size_t i = 0; i < coObjects->size(); i++)
    {
        LPGAMEOBJECT obj = coObjects->at(i);
        if (obj == nullptr || obj == this) continue;

        if (obj->IsBlocking())
        {
            float objLeft, objTop, objRight, objBottom;
            obj->GetBoundingBox(objLeft, objTop, objRight, objBottom);

            float leftEdge, topEdge, rightEdge, bottomEdge;
            GetBoundingBox(leftEdge, topEdge, rightEdge, bottomEdge);

            if (bottomEdge >= objTop - 6 && bottomEdge <= objTop + 6 &&
                rightEdge > objLeft && leftEdge < objRight && vy >= 0)
            {
                isOnPlatform = true;
                vy = 0;
                y = objTop - (state == GREEN_PARAKOOPA_STATE_HEADSHOT ? GREEN_PARAKOOPA_BBOX_HEIGHT_DIE : GREEN_PARAKOOPA_BBOX_HEIGHT) / 2;
                break;
            }
        }
    }

    if (isOnPlatform && state == GREEN_PARAKOOPA_STATE_JUMPING)
    {
        vy = GREEN_PARAKOOPA_JUMP_SPEED;
        hover_start = GetTickCount64();
        ay = GREEN_PARAKOOPA_GRAVITY;
    }

    if (hover_start > 0 && GetTickCount64() - hover_start < GREEN_PARAKOOPA_HOVER_DURATION && vy > -0.02f && vy < 0.02f)
    {
        ay = GREEN_PARAKOOPA_HOVER_GRAVITY;
    }
    else
    {
        ay = GREEN_PARAKOOPA_GRAVITY;
        if (GetTickCount64() - hover_start >= GREEN_PARAKOOPA_HOVER_DURATION)
        {
            hover_start = -1;
        }
    }

    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void GreenParaKoopa::Render()
{
    if (IsDeleted()) return;

    int aniId = ID_ANI_GREEN_PARAKOOPA_JUMP;
    if (state == GREEN_PARAKOOPA_STATE_DIE || state == GREEN_PARAKOOPA_STATE_HEADSHOT)
    {
        aniId = ID_ANI_GREEN_KOOPA_SHELL_IDLE;
    }

    CAnimations* animations = CAnimations::GetInstance();
    if (aniId != -1)
    {
        LPANIMATION animation = animations->Get(aniId);
        if (animation != nullptr)
        {
            if (state == GREEN_PARAKOOPA_STATE_HEADSHOT)
            {
                animation->RenderFlipped180(x, y); 
            }
            else
            {
                animation->Render(x, y);
            }
        }
    }
}

void GreenParaKoopa::SetState(int state)
{
    Enemy::SetState(state);
    switch (state)
    {
    case GREEN_PARAKOOPA_STATE_JUMPING:
        ay = GREEN_PARAKOOPA_GRAVITY;
        ax = 0;
        break;
    case GREEN_PARAKOOPA_STATE_DIE:
        die_start = GetTickCount64();
        y += (GREEN_PARAKOOPA_BBOX_HEIGHT - GREEN_PARAKOOPA_BBOX_HEIGHT_DIE) / 2;
        vx = 0;
        vy = 0;
        ay = 0;
        break;
    case GREEN_PARAKOOPA_STATE_HEADSHOT:
        die_start = GetTickCount64();
        y += (GREEN_PARAKOOPA_BBOX_HEIGHT - GREEN_PARAKOOPA_BBOX_HEIGHT_DIE) / 2;
        vy = HEADSHOT_JUMP_SPEED;
        vx = 0;
        ay = HEADSHOT_GRAVITY;
        break;
    }
}