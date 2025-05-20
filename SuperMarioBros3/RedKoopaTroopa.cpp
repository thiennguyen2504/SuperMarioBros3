#include "RedKoopaTroopa.h"
#include "Game.h"
#include "Brick.h"
#include "Goomba.h"
#include "VenusFire.h"
#include "Pipe.h"
#include "BrickPlatform.h"
#include "Collision.h"
#include "AssetIDs.h"
#include "Mario.h"
#include "PlayScene.h"
#include "RedParaGoomba.h"
#include "QuestionBlock.h"
#include "Coin.h"
#include "Mushroom.h"
#include "Leaf.h"

RedKoopaTroopa::RedKoopaTroopa(float x, float y) : Enemy(x, y)
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
    hasBounced = false;

    if (kickCooldownStart > 0 && (GetTickCount64() - kickCooldownStart) > RED_KOOPA_KICK_COOLDOWN)
    {
        kickCooldownStart = 0;
    }

    if (state == RED_KOOPA_STATE_SHELL_IDLE && isPositionFixed)
    {
        y = fixedY;
        vx = 0;
        vy = 0;
    }
    else if (state == RED_KOOPA_STATE_CARRIED)
    {
        // Skip velocity and position updates when carried
    }
    else
    {
        vy += ay * dt;

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

    if (state == RED_KOOPA_STATE_SHELL_IDLE || state == RED_KOOPA_STATE_CARRIED)
    {
        if (IsShellIdleTimeout())
        {
            SetState(RED_KOOPA_STATE_WALKING);
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
                y = objTop - (state == RED_KOOPA_STATE_WALKING ? RED_KOOPA_BBOX_HEIGHT : RED_KOOPA_SHELL_BBOX_HEIGHT) / 2 - 0.1f;
                if (state == RED_KOOPA_STATE_SHELL_IDLE)
                {
                    fixedY = y;
                    isPositionFixed = true;
                }
                break;
            }
        }
    }

    if (state == RED_KOOPA_STATE_WALKING && isOnPlatform)
    {
        bool edgeOnPlatform = false;
        for (size_t i = 0; i < coObjects->size(); i++)
        {
            LPGAMEOBJECT obj = coObjects->at(i);
            if (obj->IsBlocking())
            {
                float objLeft, objTop, objRight, objBottom;
                obj->GetBoundingBox(objLeft, objTop, objRight, objBottom);

                if (bottomEdge >= objTop - 8 && bottomEdge <= objTop + 8)
                {
                    if (direction == 1 && rightEdge >= objLeft && rightEdge <= objRight)
                    {
                        edgeOnPlatform = true;
                        break;
                    }
                    else if (direction == -1 && leftEdge <= objRight && leftEdge >= objLeft)
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

    bool shouldRender = true;
    if (state == RED_KOOPA_STATE_SHELL_IDLE || state == RED_KOOPA_STATE_CARRIED)
    {
        if (shellIdleStart > 0)
        {
            ULONGLONG timeSinceIdle = GetTickCount64() - shellIdleStart;
            ULONGLONG timeUntilRevive = RED_KOOPA_SHELL_IDLE_TIMEOUT - timeSinceIdle;
            if (timeUntilRevive <= RED_KOOPA_BLINK_DURATION)
            {
                ULONGLONG blinkPhase = timeSinceIdle % (2 * RED_KOOPA_BLINK_INTERVAL);
                shouldRender = (blinkPhase < RED_KOOPA_BLINK_INTERVAL);
            }
        }
    }

    CAnimations* animations = CAnimations::GetInstance();
    if (shouldRender && aniId != -1)
    {
        animations->Get(aniId)->Render(x, y);
    }
}

void RedKoopaTroopa::SetState(int state)
{
    int previousState = this->state;
    Enemy::SetState(state);
    switch (state)
    {
    case RED_KOOPA_STATE_WALKING:
        vx = direction * RED_KOOPA_WALKING_SPEED;
        vy = 0;
        ay = RED_KOOPA_GRAVITY;
        isPositionFixed = false;
        idleCooldownStart = 0;
        if (previousState == RED_KOOPA_STATE_CARRIED)
        {
            CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
            if (mario && mario->GetHeldKoopa() == this)
            {
                mario->OnHitByKoopa();
                mario->SetHeldKoopa(nullptr);
            }
        }
        break;
    case RED_KOOPA_STATE_SHELL_IDLE:
        vx = 0;
        vy = 0;
        ay = 0;
        StartShellIdleTimer();
        idleCooldownStart = GetTickCount64(); 
        break;
    case RED_KOOPA_STATE_SHELL_RUNNING:
        vx = direction * RED_KOOPA_SHELL_RUNNING_SPEED;
        vy = 0;
        ay = RED_KOOPA_GRAVITY;
        ResetShellIdleTimer();
        isPositionFixed = false;
        idleCooldownStart = 0;
        kickCooldownStart = GetTickCount64();
        break;
    case RED_KOOPA_STATE_CARRIED:
        vx = 0;
        vy = 0;
        ay = 0;
        StartShellIdleTimer();
        isPositionFixed = false;
        idleCooldownStart = 0;
        break;
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

void RedKoopaTroopa::OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e)
{
    CQuestionBlock* block = dynamic_cast<CQuestionBlock*>(e->obj);
    if (state == RED_KOOPA_STATE_SHELL_RUNNING)
    {
        if (e->nx != 0 && block->GetState() == QUESTION_BLOCK_STATE_ACTIVE && block->HasItem())
        {
            CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
            CMario* mario = (CMario*)scene->GetPlayer();
            float bx, by;
            block->GetPosition(bx, by);
            if (block->GetType() == QUESTION_BLOCK_TYPE_COIN)
            {
                CCoin* coin = new CCoin(bx, by - QUESTION_BLOCK_BBOX_HEIGHT, true);
                scene->AddObject(coin);
                block->SetState(QUESTION_BLOCK_STATE_INACTIVE);
            }
            else if (block->GetType() == QUESTION_BLOCK_TYPE_ITEM)
            {
                if (mario->GetLevel() == MARIO_LEVEL_SMALL)
                {
                    CMushroom* mushroom = new CMushroom(bx, by);
                    scene->AddObject(mushroom);
                }
                else
                {
                    CLeaf* leaf = new CLeaf(bx, by - QUESTION_BLOCK_BBOX_HEIGHT);
                    scene->AddObject(leaf);
                }
                block->SetState(QUESTION_BLOCK_STATE_INACTIVE);
            }
            block->StartBounce();
        }

        if ((e->nx != 0 || e->ny > 0) && !hasBounced && CanBounce())
        {
            ReverseDirection();
            if (e->nx > 0)
            {
                x += e->t * vx + e->nx * 0.4f;
            }
            else if (e->nx < 0)
            {
                x += e->t * vx + e->nx * 0.4f;
            }
            if (e->ny > 0)
            {
                vy = 0;
            }
            hasBounced = true;
            bounceCooldownStart = GetTickCount64();
        }
    }
}

void RedKoopaTroopa::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
    CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);
    if (state == RED_KOOPA_STATE_SHELL_RUNNING)
    {
        goomba->SetState(GOOMBA_STATE_HEADSHOT);
    }
}

void RedKoopaTroopa::OnCollisionWithRedParaGoomba(LPCOLLISIONEVENT e)
{
    RedParaGoomba* paraGoomba = dynamic_cast<RedParaGoomba*>(e->obj);
    if (state == RED_KOOPA_STATE_SHELL_RUNNING)
    {
        paraGoomba->SetState(RED_PARAGOOMBA_STATE_HEADSHOT);
    }
}

void RedKoopaTroopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (dynamic_cast<VenusFire*>(e->obj))
    {
        OnCollisionWithVenusFire(e);
    }
    else if (dynamic_cast<CQuestionBlock*>(e->obj))
    {
        OnCollisionWithQuestionBlock(e);
    }
    else if (dynamic_cast<CGoomba*>(e->obj))
    {
        OnCollisionWithGoomba(e);
    }
    else if (dynamic_cast<RedParaGoomba*>(e->obj))
    {
        OnCollisionWithRedParaGoomba(e);
    }
    else if (e->obj->IsBlocking())
    {
        if (e->ny > 0)
        {
            vy = 0;
            y += e->t * vy + e->ny * 0.4f;
            if (state == RED_KOOPA_STATE_SHELL_IDLE)
            {
                fixedY = y;
                isPositionFixed = true;
            }
        }
        if (e->nx != 0 && state == RED_KOOPA_STATE_SHELL_RUNNING && !hasBounced && CanBounce())
        {
            bool shouldBounce = dynamic_cast<CPipe*>(e->obj) != nullptr || dynamic_cast<CBrickPlatform*>(e->obj) != nullptr;
            if (shouldBounce)
            {
                float objLeft, objTop, objRight, objBottom;
                e->obj->GetBoundingBox(objLeft, objTop, objRight, objBottom);
                ReverseDirection();
                if (e->nx > 0)
                {
                    x = objRight + RED_KOOPA_BBOX_WIDTH / 2 + 2.0f;
                }
                else
                {
                    x = objLeft - RED_KOOPA_BBOX_WIDTH / 2 - 2.0f;
                }
                hasBounced = true;
                bounceCooldownStart = GetTickCount64();
            }
        }
    }
}

int RedKoopaTroopa::IsCollidable()
{
    if (state == RED_KOOPA_STATE_SHELL_RUNNING && IsKickCooldownActive())
    {
        return 0;
    }
    return 1;
}