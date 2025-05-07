#pragma once
#include "Enemy.h"

#define GOOMBA_WALKING_SPEED 0.05f

#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT 14
#define GOOMBA_BBOX_HEIGHT_DIE 7

#define ID_ANI_GOOMBA_WALKING 5000
#define ID_ANI_GOOMBA_DIE 5001

class CGoomba : public Enemy
{
protected:
    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void Render() override;

public:
    CGoomba(float x, float y);
    virtual void SetState(int state) override;
};