#pragma once
#include "GameObject.h"

class CBrickPlatform : public CGameObject
{
protected:
    float cellWidth;
    float cellHeight;
    int length;
    int spriteIdBegin, spriteIdMiddle, spriteIdEnd;
    float scale;

public:
    CBrickPlatform(float x, float y,
        float cell_width, float cell_height, int length,
        int sprite_begin, int sprite_middle, int sprite_end,
        float scale = 0.33f);

    virtual void Render();
    virtual void GetBoundingBox(float& l, float& t, float& r, float& b);
    virtual int IsDirectionColliable(float nx, float ny);
};