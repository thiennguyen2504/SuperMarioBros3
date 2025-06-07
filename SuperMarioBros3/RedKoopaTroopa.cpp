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
#include "GameObject.h"

RedKoopaTroopa::RedKoopaTroopa(float x, float y) : KoopaTroopa(x, y)
{
    SetShouldJumpOnHeadshot(true);
}

void RedKoopaTroopa::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    KoopaTroopa::Update(dt, coObjects);

    if (state == KOOPA_STATE_WALKING && isOnPlatform)
    {
        bool edgeOnPlatform = false;
        float leftEdge, topEdge, rightEdge, bottomEdge;
        GetBoundingBox(leftEdge, topEdge, rightEdge, bottomEdge);

        for (size_t i = 0; i < coObjects->size(); i++)
        {
            LPGAMEOBJECT obj = coObjects->at(i);
            if (obj->IsBlocking())
            {
                float objLeft, objTop, objRight, objBottom;
                obj->GetBoundingBox(objLeft, objTop, objRight, objBottom);

                if (bottomEdge >= objTop - 8 && bottomEdge <= objTop + 8)
                {
                    if (direction == 1 && x >= objRight - 2 && x <= objRight + 2)
                    {
                        edgeOnPlatform = true;
                        break;
                    }
                    else if (direction == -1 && x >= objLeft - 2 && x <= objLeft + 2)
                    {
                        edgeOnPlatform = true;
                        break;
                    }
                }
            }
        }

        if (edgeOnPlatform)
        {
            ReverseDirection();
        }
    }
}

void RedKoopaTroopa::Render()
{
    int aniId = -1;
    if (state == KOOPA_STATE_WALKING)
    {
        aniId = (direction == 1) ? ID_ANI_RED_KOOPA_WALK_RIGHT : ID_ANI_RED_KOOPA_WALK_LEFT;
    }
    else if (state == KOOPA_STATE_SHELL_IDLE || state == KOOPA_STATE_CARRIED)
    {
        aniId = ID_ANI_RED_KOOPA_SHELL_IDLE;
    }
    else if (state == KOOPA_STATE_SHELL_RUNNING || state == KOOPA_STATE_HEADSHOT)
    {
        aniId = (vx == 0) ? ID_ANI_RED_KOOPA_SHELL_IDLE : ID_ANI_RED_KOOPA_SHELL_RUN;
    }

    bool shouldRender = true;
    if (state == KOOPA_STATE_SHELL_IDLE || state == KOOPA_STATE_CARRIED)
    {
        if (shellIdleStart > 0)
        {
            ULONGLONG timeSinceIdle = GetTickCount64() - shellIdleStart;
            ULONGLONG timeUntilRevive = KOOPA_SHELL_IDLE_TIMEOUT - timeSinceIdle;
            if (timeUntilRevive <= KOOPA_BLINK_DURATION)
            {
                ULONGLONG blinkPhase = timeSinceIdle % (2 * KOOPA_BLINK_INTERVAL);
                shouldRender = (blinkPhase < KOOPA_BLINK_INTERVAL);
            }
        }
    }

    CAnimations* animations = CAnimations::GetInstance();
    if (shouldRender && aniId != -1)
    {
        LPANIMATION animation = animations->Get(aniId);
        if (animation != nullptr)
        {
            if (IsFlipped())
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

void RedKoopaTroopa::OnCollisionWithVenusFire(LPCOLLISIONEVENT e)
{
    VenusFire* venusFire = dynamic_cast<VenusFire*>(e->obj);
    if (state == KOOPA_STATE_SHELL_RUNNING && e->nx != 0)
    {
        venusFire->SetState(VENUS_FIRE_STATE_DIE);
    }
}

void RedKoopaTroopa::OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e)
{
    CQuestionBlock* block = dynamic_cast<CQuestionBlock*>(e->obj);
    if (state == KOOPA_STATE_SHELL_RUNNING)
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
    if (state == KOOPA_STATE_SHELL_RUNNING)
    {
        goomba->SetState(GOOMBA_STATE_HEADSHOT);
    }
}

void RedKoopaTroopa::OnCollisionWithRedParaGoomba(LPCOLLISIONEVENT e)
{
    RedParaGoomba* paraGoomba = dynamic_cast<RedParaGoomba*>(e->obj);
    if (state == KOOPA_STATE_SHELL_RUNNING)
    {
        paraGoomba->SetState(RED_PARAGOOMBA_STATE_HEADSHOT);
    }
}

void RedKoopaTroopa::OnCollisionWith(LPCOLLISIONEVENT e)
{
    KoopaTroopa::OnCollisionWith(e);
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
}