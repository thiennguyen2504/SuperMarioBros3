#include "RedKoopaTroopa.h"
#include "Game.h"
#include "Brick.h"
#include "Goomba.h"
#include "VenusFire.h"
#include "Pipe.h"
#include "BrickPlatform.h"
#include "Collision.h"
#include "AssetIDs.h"

RedKoopaTroopa::RedKoopaTroopa(float x, float y) : Enemy(x, y)
{
    this->startX = x;
    this->direction = -1; // Start walking left
    this->isOnPlatform = false;
    this->isPositionFixed = false;
    this->hasBounced = false;
    this->bounceCooldownStart = 0;
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
    // Reset bounce flag at the start of each frame
    hasBounced = false;

    // Skip updating position in SHELL_IDLE to prevent unwanted movement
    if (state == RED_KOOPA_STATE_SHELL_IDLE && isPositionFixed)
    {
        y = fixedY; // Keep Y position fixed
        vx = 0;
        vy = 0;
    }
    else
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

        Enemy::Update(dt, coObjects);
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
            if (bottomEdge >= objTop - 8 && bottomEdge <= objTop + 8 &&
                rightEdge > objLeft && leftEdge < objRight)
            {
                isOnPlatform = true;
                vy = 0; // Prevent falling through the platform
                y = objTop - (state == RED_KOOPA_STATE_WALKING ? RED_KOOPA_BBOX_HEIGHT : RED_KOOPA_SHELL_BBOX_HEIGHT) / 2 - 0.1f; // Adjust position to sit on top
                if (state == RED_KOOPA_STATE_SHELL_IDLE)
                {
                    fixedY = y; // Store the fixed Y position
                    isPositionFixed = true;
                }
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
                if (bottomEdge >= objTop - 8 && bottomEdge <= objTop + 8)
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

    // Use collision processing to handle interactions with other objects
    CCollision::GetInstance()->Process(this, dt, coObjects);
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
        isPositionFixed = false;
        break;
    case RED_KOOPA_STATE_SHELL_IDLE:
        vx = 0;
        vy = 0;
        ay = 0; 
        StartShellIdleTimer();
        break;
    case RED_KOOPA_STATE_SHELL_RUNNING:
        vx = direction * RED_KOOPA_SHELL_RUNNING_SPEED;
        vy = 0;
        ay = RED_KOOPA_GRAVITY;
        ResetShellIdleTimer();
        isPositionFixed = false;
        break;
    case RED_KOOPA_STATE_CARRIED:
        vx = 0;
        vy = 0;
        ay = 0;
        StartShellIdleTimer();
        isPositionFixed = false;
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

void RedKoopaTroopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
    // Handle collisions with Goomba
    if (dynamic_cast<CGoomba*>(e->obj))
    {
        OnCollisionWithGoomba(e);
    }
    // Handle collisions with VenusFire
    else if (dynamic_cast<VenusFire*>(e->obj))
    {
        OnCollisionWithVenusFire(e);
    }
    // Handle collisions with blocking objects (for bouncing)
    else if (e->obj->IsBlocking())
    {
        if (e->ny > 0) // Koopa lands on top of a blocking object
        {
            vy = 0;
            y += e->t * vy + e->ny * 0.4f; // Adjust position to sit on top
            if (state == RED_KOOPA_STATE_SHELL_IDLE)
            {
                fixedY = y;
                isPositionFixed = true;
            }
        }
        if (e->nx != 0 && state == RED_KOOPA_STATE_SHELL_RUNNING && !hasBounced && CanBounce())
        {
            // Only bounce off Pipe or BrickPlatform
            bool shouldBounce = dynamic_cast<CPipe*>(e->obj) != nullptr || dynamic_cast<CBrickPlatform*>(e->obj) != nullptr;
            if (shouldBounce)
            {
                float objLeft, objTop, objRight, objBottom;
                e->obj->GetBoundingBox(objLeft, objTop, objRight, objBottom);
                ReverseDirection();
                if (e->nx > 0)
                {
                    x = objRight + RED_KOOPA_BBOX_WIDTH / 2 + 2.0f; // Further increased offset
                }
                else
                {
                    x = objLeft - RED_KOOPA_BBOX_WIDTH / 2 - 2.0f; // Further increased offset
                }
                hasBounced = true;
                bounceCooldownStart = GetTickCount64();
            }
        }
    }
}