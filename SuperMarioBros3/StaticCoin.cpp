#include "StaticCoin.h"
#include "Sprites.h"

StaticCoin::StaticCoin(float x, float y, int spriteId) : CGameObject(x, y)
{
    this->spriteId = spriteId;
}

void StaticCoin::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - STATIC_COIN_BBOX_WIDTH / 2;
    top = y - STATIC_COIN_BBOX_HEIGHT / 2;
    right = left + STATIC_COIN_BBOX_WIDTH;
    bottom = top + STATIC_COIN_BBOX_HEIGHT;
}

void StaticCoin::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
}

void StaticCoin::Render()
{
    if (isDeleted) return;
    CSprites::GetInstance()->Get(spriteId)->Draw(x, y);
}