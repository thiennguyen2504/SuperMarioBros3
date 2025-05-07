#include "Fireball.h"
#include "Sprites.h"
#include "Game.h"

Fireball::Fireball(float x, float y, bool isFacingRight) : CGameObject(x, y)
{
    this->vx = isFacingRight ? FIREBALL_SPEED : -FIREBALL_SPEED;
    this->vy = 0;
}

void Fireball::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - FIREBALL_BBOX_WIDTH / 2;
    top = y - FIREBALL_BBOX_HEIGHT / 2;
    right = left + FIREBALL_BBOX_WIDTH;
    bottom = top + FIREBALL_BBOX_HEIGHT;
}

void Fireball::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    x += vx * dt;
    y += vy * dt;

    float camX, camY;
    CGame::GetInstance()->GetCamPos(camX, camY);
    float camWidth = CGame::GetInstance()->GetBackBufferWidth();
    float camHeight = CGame::GetInstance()->GetBackBufferHeight();
    if (x < camX || x > camX + camWidth || y < camY || y > camY + camHeight)
    {
        isDeleted = true;
        return;
    }

    CGameObject::Update(dt, coObjects);
    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void Fireball::Render()
{
    LPANIMATION ani = CAnimations::GetInstance()->Get(ID_ANI_FIREBALL);
    if (ani)
    {
        ani->Render(x, y);
    }
    RenderBoundingBox();
}