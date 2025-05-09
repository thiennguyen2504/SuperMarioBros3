#include "Goomba.h"

CGoomba::CGoomba(float x, float y, int type ) : Enemy(x, y)
{
	this->type = type;
    SetState(ENEMY_STATE_WALKING);
}

void CGoomba::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    if (state == ENEMY_STATE_DIE)
    {
        left = x - GOOMBA_BBOX_WIDTH / 2;
        top = y - GOOMBA_BBOX_HEIGHT_DIE / 2;
        right = left + GOOMBA_BBOX_WIDTH;
        bottom = top + GOOMBA_BBOX_HEIGHT_DIE;
    }
    else
    {
        left = x - GOOMBA_BBOX_WIDTH / 2;
        top = y - GOOMBA_BBOX_HEIGHT / 2;
        right = left + GOOMBA_BBOX_WIDTH;
        bottom = top + GOOMBA_BBOX_HEIGHT;
    }
}

void CGoomba::Render()
{
    int aniId = -1;
	if (type == GOOMBA_TYPE_RED) 
	{
		aniId = ID_ANI_RED_GOOMBA_WALK;
		if (state == ENEMY_STATE_DIE)
		{
			aniId = ID_ANI_RED_GOOMBA_DIE;
		}
	}
    else {
        aniId = ID_ANI_GOOMBA_WALKING;
        if (state == ENEMY_STATE_DIE)
        {
            aniId = ID_ANI_GOOMBA_DIE;
        }
    }

    CAnimations::GetInstance()->Get(aniId)->Render(x, y);
    RenderBoundingBox();
}

void CGoomba::SetState(int state)
{
    Enemy::SetState(state);
    switch (state)
    {
    case ENEMY_STATE_DIE:
        y += (GOOMBA_BBOX_HEIGHT - GOOMBA_BBOX_HEIGHT_DIE) / 2;
        vx = 0;
        vy = 0;
        ay = 0;
        break;
    case ENEMY_STATE_WALKING:
        vx = -GOOMBA_WALKING_SPEED;
        break;
    }
}