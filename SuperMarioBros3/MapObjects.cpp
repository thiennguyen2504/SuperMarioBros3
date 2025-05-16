#include "MapObjects.h"
#include "Sprites.h"
#include "debug.h"

void CMapObjects::Render()
{
    CSprites* sprites = CSprites::GetInstance();
    LPSPRITE sprite = sprites->Get(spriteId);

    sprite->DrawWithScale(x, y, OBJECT_SCALE);
}