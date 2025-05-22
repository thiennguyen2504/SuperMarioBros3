#pragma once
#include "KoopaTroopa.h"

#define ID_ANI_RED_KOOPA_WALK_LEFT 200021
#define ID_ANI_RED_KOOPA_WALK_RIGHT 200022
#define ID_ANI_RED_KOOPA_SHELL_IDLE 200023
#define ID_ANI_RED_KOOPA_SHELL_RUN 200024

class RedKoopaTroopa : public KoopaTroopa
{
protected:
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() override;

    void OnCollisionWithVenusFire(LPCOLLISIONEVENT e);
    void OnCollisionWithQuestionBlock(LPCOLLISIONEVENT e);
    void OnCollisionWithGoomba(LPCOLLISIONEVENT e);
    void OnCollisionWithRedParaGoomba(LPCOLLISIONEVENT e);
    virtual void OnCollisionWith(LPCOLLISIONEVENT e) override;

public:
    RedKoopaTroopa(float x, float y);
};