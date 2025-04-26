#include "Pipe.h"
#include "debug.h"
#include "Sprite.h"
#include "Sprites.h"

#include "Textures.h"
#include "Game.h"


void CPipe::Render()
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

void CPipe::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - this->headWidth / 2;
    t = y - this->headHeight / 2;
    r = l + this->headWidth;
    b = t + this->headHeight + (this->bodyHeight * this->height);
}