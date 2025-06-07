#include "Goomba.h"
#include "Game.h"
#include "HUD.h"
#include "PlayScene.h"
#include "Effect.h"

CGoomba::CGoomba(float x, float y, int type) : Enemy(x, y)
{
    this->type = type;
    this->isOnPlatform = false;
    this->ignoreTerrain = false; // Initialize ignoreTerrain
    SetState(GOOMBA_STATE_WALKING);
}

void CGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    if (state == GOOMBA_STATE_DIE || state == GOOMBA_STATE_HEADSHOT)
    {
        left = x - GOOMBA_BBOX_WIDTH / 2;
        top = y - GOOMBA_BBOX_HEIGHT_DIE / 2;
        right = left + GOOMBA_BBOX_WIDTH;
        bottom = top + GOOMBA_BBOX_HEIGHT_DIE;
    }
    else
    {
        left = x - GOOMBA_BBOX_WIDTH / 2;
        top = y - GOOMBA_BBOX_HEIGHT / 2;
        right = left + GOOMBA_BBOX_WIDTH;
        bottom = top + GOOMBA_BBOX_HEIGHT;
    }
}

void CGoomba::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    vy += ay * dt;
    vx += ax * dt;

    if (state == GOOMBA_STATE_HEADSHOT)
    {
        if (y > CGame::GetInstance()->GetBackBufferHeight())
        {
            isDeleted = true;
        }
        ignoreTerrain = true; // Ignore terrain in HEADSHOT state
    }

    Enemy::Update(dt, coObjects);
    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CGoomba::OnNoCollision(DWORD dt)
{
    x += vx * dt;
    y += vy * dt;
}

void CGoomba::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (e->obj->IsBlocking() && !ignoreTerrain)
    {
        if (e->ny != 0)
        {
            vy = 0;
            if (e->ny < 0)
            {
                isOnPlatform = true;
                y += e->t * vy + e->ny * 0.4f;
            }
        }
        if (e->nx != 0)
        {
            vx = -vx;
            x += e->t * vx + e->nx * 0.4f;
        }
    }
}

void CGoomba::Render()
{
    int aniId = -1;
    if (type == GOOMBA_TYPE_RED)
    {
        aniId = ID_ANI_RED_GOOMBA_WALK;
        if (state == GOOMBA_STATE_DIE)
        {
            aniId = ID_ANI_RED_GOOMBA_DIE;
        }
        else if (state == GOOMBA_STATE_HEADSHOT)
        {
            aniId = ID_ANI_RED_GOOMBA_WALK; // Use walk animation for HEADSHOT
        }
    }
    else
    {
        aniId = ID_ANI_GOOMBA_WALKING;
        if ( state == GOOMBA_STATE_HEADSHOT)
        {
            aniId = ID_ANI_GOOMBA_WALKING; // Use walk animation for HEADSHOT
		}
		else if (state == GOOMBA_STATE_DIE)
		{
			aniId = ID_ANI_GOOMBA_DIE;
		}
    }

    LPANIMATION animation = CAnimations::GetInstance()->Get(aniId);
    if (animation != nullptr)
    {
        if (state == GOOMBA_STATE_HEADSHOT)
        {
            animation->RenderFlipped180(x, y); // Render flipped 180 degrees
        }
        else
        {
            animation->Render(x, y);
        }
    }
    //RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
    Enemy::SetState(state);
    switch (state)
    {
    case GOOMBA_STATE_DIE:
        y += (GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE) / 2;
        vx = 0;
        vy = 0;
        ay = 0;
        break;
    case GOOMBA_STATE_WALKING:
        vx = -GOOMBA_WALKING_SPEED;
        ay = GOOMBA_GRAVITY;
        break;
    case GOOMBA_STATE_HEADSHOT:
        y += (GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE) / 2;
        vy = -HEADSHOT_JUMP_SPEED;
        vx = 0; 
        ay = GOOMBA_GRAVITY;
        ignoreTerrain = true; 
        break;
    }
}