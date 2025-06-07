#include "Card.h"
#include "Mario.h"
#include "RacoonMario.h"
#include "PlayScene.h"
#include "HUD.h"
#include "debug.h"

Card::Card(float x, float y) : CGameObject(x, y)
{
    currentType = CardType::FLOWER;
    lastSwitchTime = GetTickCount64();
}

void Card::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    ULONGLONG now = GetTickCount64();
    if (now - lastSwitchTime >= CARD_SWITCH_INTERVAL)
    {
        switch (currentType)
        {
        case CardType::FLOWER:
            currentType = CardType::STAR;
            break;
        case CardType::STAR:
            currentType = CardType::MUSHROOM;
            break;
        case CardType::MUSHROOM:
            currentType = CardType::FLOWER;
            break;
        }
        lastSwitchTime = now;
    }

    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void Card::Render()
{
    int spriteId = -1;
    switch (currentType)
    {
    case CardType::FLOWER:
        spriteId = CARD_SPRITE_FLOWER;
        break;
    case CardType::STAR:
        spriteId = CARD_SPRITE_STAR;
        break;
    case CardType::MUSHROOM:
        spriteId = CARD_SPRITE_MUSHROOM;
        break;
    }

    LPSPRITE sprite = CSprites::GetInstance()->Get(spriteId);
    if (sprite != nullptr)
    {
        sprite->DrawWithScale(x, y, CARD_SCALE);
    }
    else
    {
        DebugOut(L"[ERROR] Card sprite ID %d not found!\n", spriteId);
    }
}

void Card::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - CARD_BBOX_WIDTH / 2;
    top = y - CARD_BBOX_HEIGHT / 2;
    right = left + CARD_BBOX_WIDTH;
    bottom = top + CARD_BBOX_HEIGHT;
}

void Card::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (dynamic_cast<CMario*>(e->obj) || dynamic_cast<CRaccoonMario*>(e->obj))
    {
        CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
        CHUD* hud = scene->GetHUD();
        if (hud)
        {
            int hudSpriteId = -1;
            switch (currentType)
            {
            case CardType::FLOWER:
                hudSpriteId = CARD_HUD_FLOWER;
                break;
            case CardType::STAR:
                hudSpriteId = CARD_HUD_STAR;
                break;
            case CardType::MUSHROOM:
                hudSpriteId = CARD_HUD_MUSHROOM;
                break;
            }
            hud->UpdateCard(hudSpriteId);
        }
        Delete();
    }
}