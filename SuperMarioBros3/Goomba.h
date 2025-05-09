#pragma once
#include "Enemy.h"

#define GOOMBA_WALKING_SPEED 0.05f

#define GOOMBA_BBOX_WIDTH 16
#define GOOMBA_BBOX_HEIGHT 14
#define GOOMBA_BBOX_HEIGHT_DIE 7

#define ID_ANI_GOOMBA_WALKING 5000
#define ID_ANI_GOOMBA_DIE 5001

#define ID_ANI_RED_GOOMBA_WALK 7000
#define ID_ANI_RED_GOOMBA_DIE 7001

#define GOOMBA_TYPE_NORMAL 0
#define GOOMBA_TYPE_RED 1
class CGoomba : public Enemy
{
protected:
    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void Render() override;
	int type; // 0: Goomba, 1: Red Goomba
public:
    CGoomba(float x, float y, int type = GOOMBA_TYPE_NORMAL);
    virtual void SetState(int state) override;
};