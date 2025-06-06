#pragma once
#include "Enemy.h"

#define GREEN_PARAKOOPA_GRAVITY 0.0003f
#define GREEN_PARAKOOPA_JUMP_SPEED -0.14f
#define GREEN_PARAKOOPA_HOVER_GRAVITY 0.0001f
#define GREEN_PARAKOOPA_WALKING_SPEED 0.01f
#define GREEN_PARAKOOPA_HOVER_DURATION 500

#define GREEN_PARAKOOPA_BBOX_WIDTH 16
#define GREEN_PARAKOOPA_BBOX_HEIGHT 26
#define GREEN_PARAKOOPA_BBOX_HEIGHT_DIE 16

#define GREEN_PARAKOOPA_STATE_JUMPING 100
#define GREEN_PARAKOOPA_STATE_DIE 200

#define ID_ANI_GREEN_PARAKOOPA_JUMP 200035

class GreenParaKoopa : public Enemy
{
protected:
    float ax;
    float ay;
    bool isOnPlatform;
    ULONGLONG hover_start;
    ULONGLONG die_start;

    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() override;

    virtual int IsCollidable() { return state != GREEN_PARAKOOPA_STATE_DIE; }
    virtual int IsBlocking() { return 0; }
    virtual void OnNoCollision(DWORD dt) override;

public:
    GreenParaKoopa(float x, float y);
    virtual void SetState(int state) override;
};