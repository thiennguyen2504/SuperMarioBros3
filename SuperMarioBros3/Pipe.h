#pragma once

#include "GameObject.h"

#define ID_SPRITE_PIPE_HEAD 60000
#define ID_SPRITE_PIPE_BODY 60001

class CPipe : public CGameObject
{
protected:
    int height;
    float bodyWidth;
    float bodyHeight;
    float headWidth;
    float headHeight;
    int spriteIdHead, spriteIdBody;

public:
    CPipe(float x, float y, int height,
        int sprite_id_head, int sprite_id_body) :CGameObject(x, y)
    {
        this->height = height;
        this->spriteIdHead = sprite_id_head;
        this->spriteIdBody = sprite_id_body;

        this->headWidth = 30;
        this->headHeight = 15;
        this->bodyWidth = 30;
        this->bodyHeight = 15;
    }

    void Render();
    void Update(DWORD dt) {}
    void GetBoundingBox(float& l, float& t, float& r, float& b);
    int IsDirectionColliable(float nx, float ny) { return 1; }
};

typedef CPipe* LPPIPE;