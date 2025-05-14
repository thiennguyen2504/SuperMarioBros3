#include "Mushroom.h"
#include "Collision.h"
#include "Mario.h"
#include "PlayScene.h"

CMushroom::CMushroom(float x, float y) : CGameObject(x, y)
{
    isMovingUp = true;
    startY = y;
    ax = 0;
    ay = MUSHROOM_GRAVITY;
    SetState(MUSHROOM_STATE_EMERGING);
}

void CMushroom::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - MUSHROOM_BBOX_WIDTH / 2;
    top = y - MUSHROOM_BBOX_HEIGHT / 2;
    right = left + MUSHROOM_BBOX_WIDTH;
    bottom = top + MUSHROOM_BBOX_HEIGHT;
}

void CMushroom::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    if (!isMovingUp)
        vy += ay * dt;
    vx += ax * dt;

    if (isMovingUp && y + MUSHROOM_BBOX_HEIGHT / 2 <= startY - QUESTION_BLOCK_BBOX_HEIGHT / 2)
    {
        y = startY - QUESTION_BLOCK_BBOX_HEIGHT / 2 - MUSHROOM_BBOX_HEIGHT / 2;
        SetState(MUSHROOM_STATE_WALKING);
    }

    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMushroom::Render()
{
    if (IsDeleted())
        return;

    CAnimations::GetInstance()->Get(ID_ANI_MUSHROOM)->Render(x, y);
   // RenderBoundingBox();
}

void CMushroom::OnNoCollision(DWORD dt)
{
    x += vx * dt;
    y += vy * dt;
}

void CMushroom::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (dynamic_cast<CMario*>(e->obj))
    {
        CMario* mario = dynamic_cast<CMario*>(e->obj);
        if (mario->GetLevel() == MARIO_LEVEL_SMALL)
        {
            mario->SetLevel(MARIO_LEVEL_BIG);
        }
        isDeleted = true;
    }
    else if (e->obj->IsBlocking())
    {
        if (e->ny < 0)
        {
            vy = 0;
        }
        else if (e->nx != 0 && state == MUSHROOM_STATE_WALKING)
        {
            vx = -vx;
        }
    }

    CGameObject::OnCollisionWith(e);
}

void CMushroom::SetState(int state)
{
    CGameObject::SetState(state);
    switch (state)
    {
    case MUSHROOM_STATE_EMERGING:
        isMovingUp = true;
        vx = 0;
        vy = MUSHROOM_EMERGING_SPEED;
        break;
    case MUSHROOM_STATE_WALKING:
        isMovingUp = false;
        vx = MUSHROOM_WALKING_SPEED;
        vy = 0;
        break;
    }
}