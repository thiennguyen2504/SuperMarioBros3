#include "Pipe.h"
#include "debug.h"
#include "Sprite.h"
#include "Sprites.h"
#include "Animations.h"
#include "Textures.h"
#include "Game.h"

void CPipe::Render()
{
    if (useAnimation)
    {
        CAnimations* animations = CAnimations::GetInstance();
        LPANIMATION ani = animations->Get(animationId);

        if (ani != nullptr)
        {
            ani->RenderFlipped180(x, y);
        }
    }
    else
    {
        CSprites* s = CSprites::GetInstance();
        float cx, cy;
        CGame::GetInstance()->GetCamPos(cx, cy);
        s->Get(this->spriteIdHead)->Draw(x, y);
        float yy = y + this->bodyHeight;
        for (int i = 0; i < this->height; i++)
        {
            s->Get(this->spriteIdBody)->Draw(x, yy);
            yy += this->bodyHeight;
        }
    }
}

void CPipe::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - this->headWidth / 2;
    t = y - this->headHeight / 2;
    r = l + this->headWidth;
    if (useAnimation)
    {
        b = t + this->headHeight;
    }
    else
    {
        b = t + this->headHeight + (this->bodyHeight * this->height);
    }
}