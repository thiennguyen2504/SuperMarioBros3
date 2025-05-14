#include "QuestionBlock.h"
#include "Collision.h"
#include "Mario.h"
#include "PlayScene.h"
#include "Mushroom.h"
#include "Coin.h"
#include "Leaf.h"

CQuestionBlock::CQuestionBlock(float x, float y, int blockType) : CGameObject(x, y)
{
    type = blockType;
    hasItem = true;
    isBouncing = false;
    originalY = y;
    bounceStart = 0;
    SetState(QUESTION_BLOCK_STATE_ACTIVE);
}

void CQuestionBlock::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - QUESTION_BLOCK_BBOX_WIDTH / 2;
    top = y - QUESTION_BLOCK_BBOX_HEIGHT / 2;
    right = left + QUESTION_BLOCK_BBOX_WIDTH;
    bottom = top + QUESTION_BLOCK_BBOX_HEIGHT;
}

void CQuestionBlock::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    CGameObject::Update(dt, coObjects);

    if (isBouncing)
    {
        ULONGLONG now = GetTickCount64();
        if (now - bounceStart < QUESTION_BLOCK_BOUNCE_TIME / 2)
        {
            y = originalY - QUESTION_BLOCK_BOUNCE_HEIGHT * (1.0f - (float)(now - bounceStart) / (QUESTION_BLOCK_BOUNCE_TIME / 2));
        }
        else if (now - bounceStart < QUESTION_BLOCK_BOUNCE_TIME)
        {
            y = originalY - QUESTION_BLOCK_BOUNCE_HEIGHT * ((float)(now - bounceStart - QUESTION_BLOCK_BOUNCE_TIME / 2) / (QUESTION_BLOCK_BOUNCE_TIME / 2));
        }
        else
        {
            y = originalY;
            isBouncing = false;
        }
    }

    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CQuestionBlock::Render()
{
    int aniId = (state == QUESTION_BLOCK_STATE_ACTIVE) ? ID_ANI_QUESTION_BLOCK_ACTIVE : ID_ANI_QUESTION_BLOCK_INACTIVE;
    CAnimations::GetInstance()->Get(aniId)->Render(x, y);
}

void CQuestionBlock::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (dynamic_cast<CMario*>(e->obj))
    {
        CMario* mario = dynamic_cast<CMario*>(e->obj);
        if (e->ny < 0 && hasItem && state == QUESTION_BLOCK_STATE_ACTIVE)
        {
            CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
            if (type == QUESTION_BLOCK_TYPE_COIN)
            {
                CCoin* coin = new CCoin(x, y - QUESTION_BLOCK_BBOX_HEIGHT, true);
                scene->AddObject(coin);
                hasItem = false;
                SetState(QUESTION_BLOCK_STATE_INACTIVE);
            }
            else if (type == QUESTION_BLOCK_TYPE_ITEM)
            {
                if (mario->GetLevel() == MARIO_LEVEL_SMALL)
                {
                    CMushroom* mushroom = new CMushroom(x, y);
                    scene->AddObject(mushroom);
                }
                else
                {
                    CLeaf* leaf = new CLeaf(x, y - QUESTION_BLOCK_BBOX_HEIGHT);
                    scene->AddObject(leaf);
                }
                hasItem = false;
                SetState(QUESTION_BLOCK_STATE_INACTIVE);
            }

            StartBounce();
            mario->SetVy(0.2f);
        }
    }
}

void CQuestionBlock::StartBounce()
{
    isBouncing = true;
    bounceStart = GetTickCount64();
}

void CQuestionBlock::SetState(int state)
{
    CGameObject::SetState(state);
}