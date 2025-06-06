#include "Enemy.h"
#include "debug.h"

Enemy::Enemy(float x, float y) : CGameObject(x, y)
{
    this->ax = 0;
    this->ay = ENEMY_GRAVITY;
    this->die_start = -1;
}

void Enemy::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    vy += ay * dt;
    vx += ax * dt;

    if ((state == ENEMY_STATE_DIE) && (GetTickCount64() - die_start > ENEMY_DIE_TIMEOUT))
    {
        Delete();
        return;
    }

    CGameObject::Update(dt, coObjects);
    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void Enemy::OnNoCollision(DWORD dt)
{
    x += vx * dt;
    y += vy * dt;
}

void Enemy::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (!e->obj->IsBlocking()) return;
    if (dynamic_cast<Enemy*>(e->obj)) return;

    if (e->ny != 0)
    {
        vy = 0;
    }
    else if (e->nx != 0)
    {
        vx = -vx;
    }
}

void Enemy::SetState(int state)
{
    CGameObject::SetState(state);
    if (state == ENEMY_STATE_DIE)
    {
        die_start = GetTickCount64();
    }
}