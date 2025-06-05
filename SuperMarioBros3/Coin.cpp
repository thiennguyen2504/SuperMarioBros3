#include "Coin.h"
#include "Collision.h"
#include "PlayScene.h"
#include "Effect.h"
#include "debug.h"

CCoin::CCoin(float x, float y, BOOLEAN fromQuestionBlock) : CGameObject(x, y)
{
    isFromQuestionBlock = fromQuestionBlock;
    vy = isFromQuestionBlock ? COIN_BOUNCE_SPEED : 0.0f;
    ay = isFromQuestionBlock ? COIN_GRAVITY : 0.0f;
    spawnTime = isFromQuestionBlock ? GetTickCount64() : 0;
    startX = x; // L?u v? trí ban ??u
    startY = y;
    SetState(COIN_STATE_ACTIVE);
}

void CCoin::Render()
{
    if (isDeleted) return;
    CAnimations* animations = CAnimations::GetInstance();
    animations->Get(ID_ANI_COIN)->Render(x, y);
}

void CCoin::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    if (isDeleted) return;
    CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();

    if (isFromQuestionBlock)
    {
        vy += ay * dt;

        if (GetTickCount64() - spawnTime > COIN_LIFETIME)
        {
            DebugOut(L"[DEBUG] Coin at (%f, %f), creating effect at (%f, %f)\n", x, y, x, y);
            isDeleted = true;
            CEffect* effect = new CEffect(x, y, 100); // Dùng x, y hi?n t?i
            scene->AddObject(effect);
            return;
        }
    }

    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CCoin::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - COIN_BBOX_WIDTH / 2;
    t = y - COIN_BBOX_HEIGHT / 2;
    r = l + COIN_BBOX_WIDTH;
    b = t + COIN_BBOX_HEIGHT;
}

void CCoin::OnNoCollision(DWORD dt)
{
    if (isFromQuestionBlock)
    {
        x += vx * dt;
        y += vy * dt;
    }
}

void CCoin::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (isFromQuestionBlock && e->obj->IsBlocking())
    {
        if (e->ny < 0)
        {
            vy = 0;
        }
    }
}