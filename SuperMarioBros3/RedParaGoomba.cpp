#include "RedParaGoomba.h"
#include "Game.h"
#include "Collision.h"
#include "PlayScene.h"
#include "Goomba.h"
#include "RacoonMario.h"
#include "RedKoopaTroopa.h"

RedParaGoomba::RedParaGoomba(float x, float y) : Enemy(x, y)
{
    ax = 0;
    ay = RED_PARAGOOMBA_GRAVITY;
    vx = -RED_PARAGOOMBA_WALKING_SPEED;
    target = nullptr;
    jumpCount = 0;
    isOnPlatform = false;
    isHighJumping = false;
    die_start = hop_start = rest_start = -1;
    SetState(RED_PARAGOOMBA_STATE_WALKING);
}

void RedParaGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    if (state == RED_PARAGOOMBA_STATE_DIE || state == RED_PARAGOOMBA_STATE_HEADSHOT)
    {
        left = x - RED_PARAGOOMBA_BBOX_WIDTH / 2;
        top = y - RED_PARAGOOMBA_BBOX_HEIGHT_DIE / 2;
        right = left + RED_PARAGOOMBA_BBOX_WIDTH;
        bottom = top + RED_PARAGOOMBA_BBOX_HEIGHT_DIE;
    }
    else if (state == RED_PARAGOOMBA_STATE_HOPPING)
    {
        left = x - RED_PARAGOOMBA_BBOX_WIDTH / 2;
        top = y - RED_PARAGOOMBA_BBOX_HEIGHT_FLY / 2;
        right = left + RED_PARAGOOMBA_BBOX_WIDTH;
        bottom = top + RED_PARAGOOMBA_BBOX_HEIGHT_FLY;
    }
    else
    {
        left = x - RED_PARAGOOMBA_BBOX_WIDTH / 2;
        top = y - RED_PARAGOOMBA_BBOX_HEIGHT / 2;
        right = left + RED_PARAGOOMBA_BBOX_WIDTH;
        bottom = top + RED_PARAGOOMBA_BBOX_HEIGHT;
    }
}

void RedParaGoomba::OnNoCollision(DWORD dt)
{
    x += vx * dt;
    y += vy * dt;
}

void RedParaGoomba::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (e->obj->IsBlocking())
    {
        if (e->ny != 0)
        {
            vy = 0;
            if (e->ny < 0)
            {
                isOnPlatform = true;
            }
		}
		else if (e->nx != 0)
		{
			vx = 0;
		}
    }

}

void RedParaGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    vy += ay * dt;
    vx += ax * dt;

    if (state == RED_PARAGOOMBA_STATE_HEADSHOT)
    {
        if (y > CGame::GetInstance()->GetBackBufferHeight())
        {
            isDeleted = true;
        }
    }
    else
    {
        if (target == nullptr)
        {
            for (size_t i = 0; i < coObjects->size(); i++)
            {
                LPGAMEOBJECT obj = coObjects->at(i);
                if (obj != nullptr)
                {
                    if (dynamic_cast<CMario*>(obj))
                        target = dynamic_cast<CMario*>(obj);
                    else if (dynamic_cast<CRaccoonMario*>(obj))
                        target = dynamic_cast<CRaccoonMario*>(obj);
                    break;
                }
            }
        }

        isOnPlatform = false;
        for (size_t i = 0; i < coObjects->size(); i++)
        {
            LPGAMEOBJECT obj = coObjects->at(i);
            if (obj == nullptr || obj == this)
                continue;

            if (obj->IsBlocking())
            {
                float objLeft, objTop, objRight, objBottom;
                obj->GetBoundingBox(objLeft, objTop, objRight, objBottom);

                float leftEdge, topEdge, rightEdge, bottomEdge;
                GetBoundingBox(leftEdge, topEdge, rightEdge, bottomEdge);

                if (bottomEdge >= objTop - 4 && bottomEdge <= objTop + 4 &&
                    rightEdge > objLeft && leftEdge < objRight)
                {
                    isOnPlatform = true;
                    vy = 0;
                    y = objTop - (state == RED_PARAGOOMBA_STATE_HOPPING ? RED_PARAGOOMBA_BBOX_HEIGHT_FLY : RED_PARAGOOMBA_BBOX_HEIGHT) / 2;
                    break;
                }
            }
        }
        CCollision::GetInstance()->Process(this, dt, coObjects);
        if (isOnPlatform)
        {
            if (isHighJumping && target != nullptr)
            {
                float marioX, marioY;
                target->GetPosition(marioX, marioY);
                nx = (marioX > x) ? 1 : -1;
                vx = nx * RED_PARAGOOMBA_WALKING_SPEED;
                isHighJumping = false;
            }

            if (state == RED_PARAGOOMBA_STATE_WALKING)
            {
                if (GetTickCount64() - rest_start > RED_PARAGOOMBA_REST_TIMEOUT)
                {
                    SetState(RED_PARAGOOMBA_STATE_HOPPING);
                }
            }
            else if (state == RED_PARAGOOMBA_STATE_HOPPING)
            {
                if (GetTickCount64() - hop_start > RED_PARAGOOMBA_HOP_TIMEOUT)
                {
                    if (jumpCount < 2)
                    {
                        vy = RED_PARAGOOMBA_LOW_JUMP_SPEED;
                        jumpCount++;
                        SetState(RED_PARAGOOMBA_STATE_HOPPING);
                    }
                    else
                    {
                        vy = RED_PARAGOOMBA_HIGH_JUMP_SPEED;
                        jumpCount = 0;
                        isHighJumping = true;
                        SetState(RED_PARAGOOMBA_STATE_WALKING);
                    }
                }
            }
        }
    }

    x += vx * dt;
    y += vy * dt;


}

void RedParaGoomba::Render()
{
    if (IsDeleted())
        return;

    int aniId = ID_ANI_RED_PARAGOOMBA_WALK;
    if (state == RED_PARAGOOMBA_STATE_HOPPING)
    {
        aniId = ID_ANI_RED_PARAGOOMBA_FLY;
    }
    else if (state == RED_PARAGOOMBA_STATE_DIE || state == RED_PARAGOOMBA_STATE_HEADSHOT)
    {
        aniId = ID_ANI_RED_PARAGOOMBA_DIE;
    }

    LPANIMATION ani = CAnimations::GetInstance()->Get(aniId);
    if (ani != nullptr)
    {
        ani->Render(x, y);
    }

   // RenderBoundingBox();
}

void RedParaGoomba::SetState(int state)
{
    Enemy::SetState(state);
    switch (state)
    {
    case RED_PARAGOOMBA_STATE_DIE:
        die_start = GetTickCount64();
        y += (RED_PARAGOOMBA_BBOX_HEIGHT - RED_PARAGOOMBA_BBOX_HEIGHT_DIE) / 2;
        vx = 0;
        vy = 0;
        ay = 0;
        jumpCount = 0;
        isHighJumping = false;
        break;
    case RED_PARAGOOMBA_STATE_WALKING:
        rest_start = GetTickCount64();
        ay = RED_PARAGOOMBA_GRAVITY;
        ax = 0;
        break;
    case RED_PARAGOOMBA_STATE_HOPPING:
        hop_start = GetTickCount64();
        ay = RED_PARAGOOMBA_GRAVITY;
        break;
    case RED_PARAGOOMBA_STATE_HEADSHOT:
        die_start = GetTickCount64();
        y += (RED_PARAGOOMBA_BBOX_HEIGHT - RED_PARAGOOMBA_BBOX_HEIGHT_DIE) / 2;
        vy = -HEADSHOT_JUMP_SPEED;
        ay = RED_PARAGOOMBA_GRAVITY;
        jumpCount = 0;
        isHighJumping = false;
        break;
    }
}