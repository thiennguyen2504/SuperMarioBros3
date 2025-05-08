#include "RedKoopaTroopa.h"
#include "Game.h"
#include "Brick.h"
#include "Goomba.h"
#include "VenusFire.h"
#include "Collision.h"
#include "AssetIDs.h"

RedKoopaTroopa::RedKoopaTroopa(float x, float y) : Enemy(x, y)
{
    this->startX = x;
    this->direction = -1; // Start walking left
    this->isOnPlatform = false;
    this->shellIdleStart = 0;
    this->vx = direction * RED_KOOPA_WALKING_SPEED;
    this->vy = 0;
    this->ay = RED_KOOPA_GRAVITY;
    SetState(RED_KOOPA_STATE_WALKING);
}

void RedKoopaTroopa::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - RED_KOOPA_BBOX_WIDTH / 2;
    top = y - (state == RED_KOOPA_STATE_WALKING ? RED_KOOPA_BBOX_HEIGHT : RED_KOOPA_SHELL_BBOX_HEIGHT) / 2;
    right = left + RED_KOOPA_BBOX_WIDTH;
    bottom = top + (state == RED_KOOPA_STATE_WALKING ? RED_KOOPA_BBOX_HEIGHT : RED_KOOPA_SHELL_BBOX_HEIGHT);
}

void RedKoopaTroopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    vy += ay * dt;

    // Update velocity based on state
    if (state == RED_KOOPA_STATE_WALKING)
    {
        vx = direction * RED_KOOPA_WALKING_SPEED;
    }
    else if (state == RED_KOOPA_STATE_SHELL_RUNNING)
    {
        vx = direction * RED_KOOPA_SHELL_RUNNING_SPEED;
    }

    // Check for shell idle timeout to revive
    if (state == RED_KOOPA_STATE_SHELL_IDLE || state == RED_KOOPA_STATE_CARRIED)
    {
        if (IsShellIdleTimeout())
        {
            SetState(RED_KOOPA_STATE_WALKING);
            ResetShellIdleTimer();
        }
    }

    // Update isOnPlatform by checking if Koopa is on a blocking object
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

            // Check if Koopa's bottom edge is just above or touching the object's top
            if (bottomEdge >= objTop - 1 && bottomEdge <= objTop + 1 &&
                rightEdge > objLeft && leftEdge < objRight)
            {
                isOnPlatform = true;
                break;
            }
        }
    }

    // Check for platform edge to turn around
    if (state == RED_KOOPA_STATE_WALKING && isOnPlatform)
    {
        // Check if the edge of Koopa's bounding box is still on a platform
        bool edgeOnPlatform = false;
        for (size_t i = 0; i < coObjects->size(); i++)
        {
            LPGAMEOBJECT obj = coObjects->at(i);
            if (obj->IsBlocking())
            {
                float objLeft, objTop, objRight, objBottom;
                obj->GetBoundingBox(objLeft, objTop, objRight, objBottom);

                // Check if Koopa's bottom edge is just above or touching the object's top
                if (bottomEdge >= objTop - 1 && bottomEdge <= objTop + 1)
                {
                    if (direction == 1 && rightEdge + 1 >= objLeft && rightEdge <= objRight)
                    {
                        edgeOnPlatform = true;
                        break;
                    }
                    else if (direction == -1 && leftEdge - 1 <= objRight && leftEdge >= objLeft)
                    {
                        edgeOnPlatform = true;
                        break;
                    }
                }
            }
        }

        if (!edgeOnPlatform)
        {
            ReverseDirection();
        }
    }

    Enemy::Update(dt, coObjects);
}

void RedKoopaTroopa::Render()
{
    int aniId = -1;
    if (state == RED_KOOPA_STATE_WALKING)
    {
        aniId = (direction == 1) ? ID_ANI_RED_KOOPA_WALK_RIGHT : ID_ANI_RED_KOOPA_WALK_LEFT;
    }
    else if (state == RED_KOOPA_STATE_SHELL_IDLE || state == RED_KOOPA_STATE_CARRIED)
    {
        aniId = ID_ANI_RED_KOOPA_SHELL_IDLE;
    }
    else if (state == RED_KOOPA_STATE_SHELL_RUNNING)
    {
        aniId = ID_ANI_RED_KOOPA_SHELL_RUN;
    }

    CAnimations* animations = CAnimations::GetInstance();
    animations->Get(aniId)->Render(x, y);
}

void RedKoopaTroopa::SetState(int state)
{
    Enemy::SetState(state);
    switch (state)
    {
    case RED_KOOPA_STATE_WALKING:
        vx = direction * RED_KOOPA_WALKING_SPEED;
        vy = 0;
        ay = RED_KOOPA_GRAVITY;
        break;
    case RED_KOOPA_STATE_SHELL_IDLE:
        vx = 0;
        vy = 0;
        ay = RED_KOOPA_GRAVITY;
        StartShellIdleTimer();
        break;
    case RED_KOOPA_STATE_SHELL_RUNNING:
        vx = direction * RED_KOOPA_SHELL_RUNNING_SPEED;
        vy = 0;
        ay = RED_KOOPA_GRAVITY;
        ResetShellIdleTimer();
        break;
    case RED_KOOPA_STATE_CARRIED:
        vx = 0;
        vy = 0;
        ay = 0;
        StartShellIdleTimer();
        break;
    }
}

void RedKoopaTroopa::OnCollisionWithBrick(LPCOLLISIONEVENT e)
{
    CBrick* brick = dynamic_cast<CBrick*>(e->obj);
    if (state == RED_KOOPA_STATE_SHELL_RUNNING && e->nx != 0)
    {
        brick->Delete();
    }
}

void RedKoopaTroopa::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
    CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
    if (state == RED_KOOPA_STATE_SHELL_RUNNING && e->nx != 0)
    {
        goomba->SetState(ENEMY_STATE_DIE);
    }
}

void RedKoopaTroopa::OnCollisionWithVenusFire(LPCOLLISIONEVENT e)
{
    VenusFire* venusFire = dynamic_cast<VenusFire*>(e->obj);
    if (state == RED_KOOPA_STATE_SHELL_RUNNING && e->nx != 0)
    {
        venusFire->Delete();
    }
}