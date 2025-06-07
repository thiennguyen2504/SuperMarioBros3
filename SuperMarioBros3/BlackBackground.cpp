#include "BlackBackground.h"
#include "Sprites.h"
#include "debug.h"

BlackBackground::BlackBackground(float x, float y, float mapWidth, float mapHeight) : CGameObject(x, y)
{
    this->mapWidth = mapWidth;
    this->mapHeight = mapHeight;
}

void BlackBackground::Render()
{
    LPSPRITE verticalSprite = CSprites::GetInstance()->Get(BLACKBACKGROUND_SPRITE_ID_VERTICAL);
    LPSPRITE fillSprite = CSprites::GetInstance()->Get(BLACKBACKGROUND_SPRITE_ID_FILL);

    if (!verticalSprite || !fillSprite)
    {
        DebugOut(L"[ERROR] Sprite ID %d or %d not found!\n", BLACKBACKGROUND_SPRITE_ID_VERTICAL, BLACKBACKGROUND_SPRITE_ID_FILL);
        return;
    }

    for (float y = 0; y < mapHeight; y += BLACKBACKGROUND_HEIGHT)
    {
        verticalSprite->DrawWithScale(x, y, BLACKBACKGROUND_SCALE);
    }

    for (float y = 0; y < mapHeight; y += BLACKBACKGROUND_HEIGHT)
    {
        for (float xPos = x + BLACKBACKGROUND_WIDTH; xPos < x + mapWidth; xPos += BLACKBACKGROUND_WIDTH)
        {
            fillSprite->DrawWithScale(xPos - 1.0f, y, BLACKBACKGROUND_SCALE);
        }
    }
}

void BlackBackground::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x;
    top = y;
    right = x + mapWidth;
    bottom = y + mapHeight;
}