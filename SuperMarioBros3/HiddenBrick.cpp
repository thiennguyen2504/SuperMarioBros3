#include "HiddenBrick.h"
#include "Sprites.h"

void CHiddenBrick::Render()
{
    CSprites* sprites = CSprites::GetInstance();
    LPSPRITE sprite = sprites->Get(20013); 

    if (sprite != nullptr)
    {
        for (int i = 0; i < length; i++)
        {
            sprite->DrawWithScale(x + i * width, y, 0.33f);
        }
    }

    //RenderBoundingBox();
}

void CHiddenBrick::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - (width ) / 2;
    t = y - HIDDEN_BRICK_BBOX_HEIGHT / 2;
    r = l + (width) * length;
    b = t + HIDDEN_BRICK_BBOX_HEIGHT;
}