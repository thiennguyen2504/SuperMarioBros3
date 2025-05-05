#include "BrickPlatform.h"

#include "Sprite.h"
#include "Sprites.h"

#include "Textures.h"
#include "Game.h"

CBrickPlatform::CBrickPlatform(float x, float y,
    float cell_width, float cell_height, int length,
    int sprite_begin, int sprite_middle, int sprite_end,
    float scale)
{
    this->x = x;
    this->y = y;
    this->cellWidth = cell_width;
    this->cellHeight = cell_height;
    this->length = length;
    this->spriteIdBegin = sprite_begin;
    this->spriteIdMiddle = sprite_middle;
    this->spriteIdEnd = sprite_end;
    this->scale = scale;
}


void CBrickPlatform::Render()
{
    if (this->length <= 0) return;
    CSprites* s = CSprites::GetInstance();

    float xx = x - (this->cellWidth * this->scale * (this->length - 1)) / 2; // Căn giữa theo chiều ngang

    s->Get(this->spriteIdBegin)->DrawWithScale(xx, y, this->scale);
    xx += this->cellWidth * this->scale;
    for (int i = 1; i < this->length - 1; i++)
    {
        s->Get(this->spriteIdMiddle)->DrawWithScale(xx, y, this->scale);
        xx += this->cellWidth * this->scale;
    }
    if (length > 1)
        s->Get(this->spriteIdEnd)->DrawWithScale(xx, y, this->scale);

    //RenderBoundingBox();
}

void CBrickPlatform::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    float total_width = this->cellWidth * this->scale * this->length;

    // Lấy chiều cao thực tế của sprite begin
    CSprites* s = CSprites::GetInstance();
    LPSPRITE sprite = s->Get(this->spriteIdBegin);
    float sprite_height = 0.0f;
    if (sprite)
    {
        sprite_height = (sprite->GetBottom() - sprite->GetTop() + 1) * this->scale;
    }
    else
    {
        sprite_height = this->cellHeight * this->scale; 
    }

    l = x - total_width / 2;
    t = y - sprite_height / 2;
    r = x + total_width / 2;
    b = y + sprite_height / 2;
}

int CBrickPlatform::IsDirectionColliable(float nx, float ny)
{
    return 1; // Chặn va chạm từ mọi hướng
}