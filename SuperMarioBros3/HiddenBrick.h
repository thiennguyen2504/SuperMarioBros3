#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define ID_ANI_HIDDEN_BRICK 9995
#define HIDDEN_BRICK_BBOX_HEIGHT 16

class CHiddenBrick : public CGameObject {
protected:
    int length; 
    float width; 
    float scale; 

public:
    CHiddenBrick(float x, float y, int length = 1, float width = 16.0f, float scale = 0.33f)
        : CGameObject(x, y) {
        this->length = length; this->width = width; this->scale = scale;
    }
    void Render();
    void Update(DWORD dt) {}
    void GetBoundingBox(float& l, float& t, float& r, float& b);
};