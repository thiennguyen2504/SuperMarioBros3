#pragma once
#include "Mario.h"

#pragma region ANIMATION_ID
#define ID_ANI_RACCOON_MARIO_IDLE_RIGHT 1900
#define ID_ANI_RACCOON_MARIO_IDLE_LEFT 1907
#define ID_ANI_RACCOON_MARIO_WALK_RIGHT 1901
#define ID_ANI_RACCOON_MARIO_WALK_LEFT 1908
#define ID_ANI_RACCOON_MARIO_BRACE_RIGHT 1902
#define ID_ANI_RACCOON_MARIO_BRACE_LEFT 1909
#define ID_ANI_RACCOON_MARIO_RUN_RIGHT 1903
#define ID_ANI_RACCOON_MARIO_RUN_LEFT 1910
#define ID_ANI_RACCOON_MARIO_JUMP_RIGHT 1904
#define ID_ANI_RACCOON_MARIO_JUMP_LEFT 1911
#define ID_ANI_RACCOON_MARIO_FALL_RIGHT 1905
#define ID_ANI_RACCOON_MARIO_FALL_LEFT 1912
#define ID_ANI_RACCOON_MARIO_HOLD_RIGHT 1906
#define ID_ANI_RACCOON_MARIO_HOLD_LEFT 1913
#define ID_ANI_RACCOON_MARIO_APPEAR 1914
#pragma endregion


class CRaccoonMario : public CMario
{
protected:
    virtual int GetAniIdRaccoon();
    void OnHit();

public:
    CRaccoonMario(float x, float y) : CMario(x, y)
    {
        level = MARIO_LEVEL_RACCOON;
    }
    virtual void OnCollisionWith(LPCOLLISIONEVENT e) override;
    virtual void OnCollisionWithGoomba(LPCOLLISIONEVENT e) override;
    virtual void OnCollisionWithVenusFire(LPCOLLISIONEVENT e) override;
    virtual void OnCollisionWithFireball(LPCOLLISIONEVENT e) override;
    virtual void OnCollisionWithRedKoopaTroopa(LPCOLLISIONEVENT e) override;
    virtual void OnCollisionWithRedParaGoomba(LPCOLLISIONEVENT e) override;
	virtual void OnCollisionWithPortal(LPCOLLISIONEVENT e) override;
    virtual void OnHitByKoopa() override;
    virtual void Render() override;
    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
};