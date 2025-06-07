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
    int animationId; // ID c?a animation n?u s? d?ng animation
    BOOLEAN canGo;
    BOOLEAN useAnimation; // C? ?? xác ??nh dùng sprite hay animation

public:
    // Constructor cho sprite
    CPipe(float x, float y, int height, int sprite_id_head, int sprite_id_body, BOOLEAN can_go = false) : CGameObject(x, y)
    {
        this->height = height;
        this->spriteIdHead = sprite_id_head;
        this->spriteIdBody = sprite_id_body;
        this->canGo = can_go;
        this->useAnimation = false;
        this->animationId = -1;

        this->headWidth = 30;
        this->headHeight = 15;
        this->bodyWidth = 30;
        this->bodyHeight = 15;
    }

    // Constructor cho animation
    CPipe(float x, float y, int animation_id, BOOLEAN can_go = false) : CGameObject(x, y)
    {
        this->height = 0;
        this->spriteIdHead = -1;
        this->spriteIdBody = -1;
        this->animationId = animation_id;
        this->canGo = true;
        this->useAnimation = true;

        this->headWidth = 30;
        this->headHeight = 15;
        this->bodyWidth = 30;
        this->bodyHeight = 15;
    }

    virtual void Render();
    void Update(DWORD dt) {}
    void GetBoundingBox(float& l, float& t, float& r, float& b);
    virtual int IsBlocking() { return 1; }
    int IsDirectionColliable(float nx, float ny) { return 1; }
    BOOLEAN CanGo() const { return canGo; }
};

typedef CPipe* LPPIPE;