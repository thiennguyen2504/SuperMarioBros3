#include <algorithm>
#include "Mario.h"
#include "Game.h"
#include "Goomba.h"
#include "Coin.h"
#include "Portal.h"
#include "VenusFire.h"
#include "Fireball.h"
#include "RedKoopaTroopa.h"
#include "Collision.h"

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	vy += ay * dt;
	vx += ax * dt;

	if (abs(vx) > abs(maxVx)) vx = maxVx;

	if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
	{
		untouchable_start = 0;
		untouchable = 0;
	}

	if (isHolding && heldKoopa)
	{
		float marioLeft, marioTop, marioRight, marioBottom;
		GetBoundingBox(marioLeft, marioTop, marioRight, marioBottom);

		float koopaLeft, koopaTop, koopaRight, koopaBottom;
		heldKoopa->GetBoundingBox(koopaLeft, koopaTop, koopaRight, koopaBottom);

		float koopaX, koopaY;
		if (nx > 0)
		{
			koopaX = marioRight + (koopaRight - koopaLeft) / 2;
		}
		else
		{
			koopaX = marioLeft - (koopaRight - koopaLeft) / 2;
		}
		koopaY = y;

		heldKoopa->SetPosition(koopaX, koopaY);
	}

	CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMario::OnNoCollision(DWORD dt)
{
	x += vx * dt;
	y += vy * dt;
	isOnPlatform = false;
}

void CMario::OnCollisionWith(LPCOLLISIONEVENT e)
{
	if (e->ny != 0 && e->obj->IsBlocking())
	{
		vy = 0;
		if (e->ny < 0) isOnPlatform = true;
	}
	else if (e->nx != 0 && e->obj->IsBlocking())
	{
		vx = 0;
	}

	if (dynamic_cast<CGoomba*>(e->obj))
		OnCollisionWithGoomba(e);
	else if (dynamic_cast<CCoin*>(e->obj))
		OnCollisionWithCoin(e);
	else if (dynamic_cast<CPortal*>(e->obj))
		OnCollisionWithPortal(e);
	else if (dynamic_cast<VenusFire*>(e->obj))
		OnCollisionWithVenusFire(e);
	else if (dynamic_cast<Fireball*>(e->obj))
		OnCollisionWithFireball(e);
	else if (dynamic_cast<RedKoopaTroopa*>(e->obj))
		OnCollisionWithRedKoopaTroopa(e);
}

void CMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
	CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

	if (e->ny < 0)
	{
		if (goomba->GetState() != ENEMY_STATE_DIE)
		{
			goomba->SetState(ENEMY_STATE_DIE);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
	}
	else
	{
		if (untouchable == 0)
		{
			if (goomba->GetState() != ENEMY_STATE_DIE)
			{
				if (level > MARIO_LEVEL_SMALL)
				{
					level = MARIO_LEVEL_SMALL;
					StartUntouchable();
				}
				else
				{
					SetState(MARIO_STATE_DIE);
				}
			}
		}
	}
}

void CMario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
	e->obj->Delete();
	coin++;
}

void CMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	CPortal* p = (CPortal*)e->obj;
	CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}

void CMario::OnCollisionWithVenusFire(LPCOLLISIONEVENT e)
{
	VenusFire* venusFire = dynamic_cast<VenusFire*>(e->obj);

	if (e->ny >= 0)
	{
		if (untouchable == 0)
		{
			if (level > MARIO_LEVEL_SMALL)
			{
				level = MARIO_LEVEL_SMALL;
				StartUntouchable();
			}
			else
			{
				SetState(MARIO_STATE_DIE);
			}
		}
	}
}

void CMario::OnCollisionWithFireball(LPCOLLISIONEVENT e)
{
	Fireball* fireball = dynamic_cast<Fireball*>(e->obj);
	fireball->Delete();

	if (untouchable == 0)
	{
		if (level > MARIO_LEVEL_SMALL)
		{
			level = MARIO_LEVEL_SMALL;
			StartUntouchable();
		}
		else
		{
			SetState(MARIO_STATE_DIE);
		}
	}
}

void CMario::OnCollisionWithRedKoopaTroopa(LPCOLLISIONEVENT e)
{
	RedKoopaTroopa* koopa = dynamic_cast<RedKoopaTroopa*>(e->obj);

	if (e->ny < 0)
	{
		if (koopa->GetState() == RED_KOOPA_STATE_WALKING)
		{
			koopa->SetState(RED_KOOPA_STATE_SHELL_IDLE);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
		else if (koopa->GetState() == RED_KOOPA_STATE_SHELL_RUNNING)
		{
			koopa->SetState(RED_KOOPA_STATE_SHELL_IDLE);
			vy = -MARIO_JUMP_DEFLECT_SPEED;
		}
	}
	else if (e->nx != 0)
	{
		if (koopa->GetState() == RED_KOOPA_STATE_WALKING ||
			(koopa->GetState() == RED_KOOPA_STATE_SHELL_RUNNING && !koopa->IsKickCooldownActive()))
		{
			if (untouchable == 0)
			{
				if (level > MARIO_LEVEL_SMALL)
				{
					level = MARIO_LEVEL_SMALL;
					StartUntouchable();
				}
				else
				{
					SetState(MARIO_STATE_DIE);
				}
			}
		}
		else if (koopa->GetState() == RED_KOOPA_STATE_SHELL_IDLE)
		{
			if (IsKeyDown(DIK_A))
			{
				if (!isHolding)
				{
					koopa->SetState(RED_KOOPA_STATE_CARRIED);
					SetHeldKoopa(koopa);
				}
			}
			else if (!isHolding)
			{
				koopa->SetState(RED_KOOPA_STATE_SHELL_RUNNING);
				koopa->SetDirection(nx);
			}
		}
	}
}

void CMario::OnHitByKoopa()
{
	if (untouchable == 0)
	{
		if (level > MARIO_LEVEL_SMALL)
		{
			level = MARIO_LEVEL_SMALL;
			StartUntouchable();
		}
		else
		{
			SetState(MARIO_STATE_DIE);
		}
	}
}

int CMario::GetAniIdSmall()
{
	int aniId = -1;

	if (isHolding)
	{
		if (vx == 0)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_SMALL_HOLD_IDLE_RIGHT;
			else
				aniId = ID_ANI_MARIO_SMALL_HOLD_IDLE_LEFT;
		}
		else if (abs(ax) == MARIO_ACCEL_RUN_X)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_SMALL_HOLD_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_SMALL_HOLD_RUN_LEFT;
		}
		else
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_SMALL_HOLD_IDLE_RIGHT;
			else
				aniId = ID_ANI_MARIO_SMALL_HOLD_IDLE_LEFT;
		}
	}
	else
	{
		if (!isOnPlatform)
		{
			if (abs(ax) == MARIO_ACCEL_RUN_X)
			{
				if (nx >= 0)
					aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT;
				else
					aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT;
			}
			else
			{
				if (nx >= 0)
					aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT;
				else
					aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT;
			}
		}
		else
			if (isSitting)
			{
				if (nx > 0)
					aniId = ID_ANI_MARIO_SIT_RIGHT;
				else
					aniId = ID_ANI_MARIO_SIT_LEFT;
			}
			else
				if (vx == 0)
				{
					if (nx > 0) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;
					else aniId = ID_ANI_MARIO_SMALL_IDLE_LEFT;
				}
				else if (vx > 0)
				{
					if (ax < 0)
						aniId = ID_ANI_MARIO_SMALL_BRACE_RIGHT;
					else if (ax == MARIO_ACCEL_RUN_X)
						aniId = ID_ANI_MARIO_SMALL_RUNNING_RIGHT;
					else if (ax == MARIO_ACCEL_WALK_X)
						aniId = ID_ANI_MARIO_SMALL_WALKING_RIGHT;
				}
				else
				{
					if (ax > 0)
						aniId = ID_ANI_MARIO_SMALL_BRACE_LEFT;
					else if (ax == -MARIO_ACCEL_RUN_X)
						aniId = ID_ANI_MARIO_SMALL_RUNNING_LEFT;
					else if (ax == -MARIO_ACCEL_WALK_X)
						aniId = ID_ANI_MARIO_SMALL_WALKING_LEFT;
				}
	}

	if (aniId == -1) aniId = nx > 0 ? ID_ANI_MARIO_SMALL_IDLE_RIGHT : ID_ANI_MARIO_SMALL_IDLE_LEFT;

	return aniId;
}

int CMario::GetAniIdBig()
{
	int aniId = -1;

	if (isHolding)
	{
		if (vx == 0)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_BIG_HOLD_IDLE_RIGHT;
			else
				aniId = ID_ANI_MARIO_BIG_HOLD_IDLE_LEFT;
		}
		else if (abs(ax) == MARIO_ACCEL_RUN_X)
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_BIG_HOLD_RUN_RIGHT;
			else
				aniId = ID_ANI_MARIO_BIG_HOLD_RUN_LEFT;
		}
		else
		{
			if (nx > 0)
				aniId = ID_ANI_MARIO_BIG_HOLD_IDLE_RIGHT;
			else
				aniId = ID_ANI_MARIO_BIG_HOLD_IDLE_LEFT;
		}
	}
	else
	{
		if (!isOnPlatform)
		{
			if (abs(ax) == MARIO_ACCEL_RUN_X)
			{
				if (nx >= 0)
					aniId = ID_ANI_MARIO_JUMP_RUN_RIGHT;
				else
					aniId = ID_ANI_MARIO_JUMP_RUN_LEFT;
			}
			else
			{
				if (nx >= 0)
					aniId = ID_ANI_MARIO_JUMP_WALK_RIGHT;
				else
					aniId = ID_ANI_MARIO_JUMP_WALK_LEFT;
			}
		}
		else
			if (isSitting)
			{
				if (nx > 0)
					aniId = ID_ANI_MARIO_SIT_RIGHT;
				else
					aniId = ID_ANI_MARIO_SIT_LEFT;
			}
			else
				if (vx == 0)
				{
					if (nx > 0) aniId = ID_ANI_MARIO_IDLE_RIGHT;
					else aniId = ID_ANI_MARIO_IDLE_LEFT;
				}
				else if (vx > 0)
				{
					if (ax < 0)
						aniId = ID_ANI_MARIO_BRACE_RIGHT;
					else if (ax == MARIO_ACCEL_RUN_X)
						aniId = ID_ANI_MARIO_RUNNING_RIGHT;
					else if (ax == MARIO_ACCEL_WALK_X)
						aniId = ID_ANI_MARIO_WALKING_RIGHT;
				}
				else
				{
					if (ax > 0)
						aniId = ID_ANI_MARIO_BRACE_LEFT;
					else if (ax == -MARIO_ACCEL_RUN_X)
						aniId = ID_ANI_MARIO_RUNNING_LEFT;
					else if (ax == -MARIO_ACCEL_WALK_X)
						aniId = ID_ANI_MARIO_WALKING_LEFT;
				}
	}

	if (aniId == -1) aniId = nx > 0 ? ID_ANI_MARIO_IDLE_RIGHT : ID_ANI_MARIO_IDLE_LEFT;

	return aniId;
}

void CMario::Render()
{
	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	bool shouldRender = true;
	if (untouchable == 1 && untouchable_start != -1)
	{
		ULONGLONG timeSinceUntouchable = GetTickCount64() - untouchable_start;
		ULONGLONG blinkPhase = timeSinceUntouchable % (2 * MARIO_UNTOUCHABLE_BLINK_INTERVAL);
		shouldRender = (blinkPhase < MARIO_UNTOUCHABLE_BLINK_INTERVAL);
	}

	if (state == MARIO_STATE_DIE)
	{
		aniId = ID_ANI_MARIO_DIE;
	}
	else if (isBlinking && blinkStart != 0)
	{
		ULONGLONG timeSinceBlink = GetTickCount64() - blinkStart;
		if (timeSinceBlink < MARIO_BLINK_TIME)
		{
			ULONGLONG blinkPhase = timeSinceBlink % (2 * MARIO_BLINK_INTERVAL);
			if (blinkPhase < MARIO_BLINK_INTERVAL)
				aniId = (level == MARIO_LEVEL_BIG) ? GetAniIdSmall() : GetAniIdBig();
			else
				aniId = (level == MARIO_LEVEL_BIG) ? GetAniIdBig() : GetAniIdSmall();
		}
		else
		{
			isBlinking = false;
			isLocked = false;
			blinkStart = 0;
			aniId = (level == MARIO_LEVEL_BIG) ? GetAniIdBig() : GetAniIdSmall();
		}
	}
	else
	{
		aniId = (level == MARIO_LEVEL_BIG) ? GetAniIdBig() : GetAniIdSmall();
	}

	if (shouldRender && aniId != -1)
	{
		LPANIMATION animation = animations->Get(aniId);
		if (animation != nullptr)
		{
			animation->Render(x, y);
		}
	}
}

void CMario::SetState(int state)
{
	if (this->state == MARIO_STATE_DIE || isLocked) return;

	switch (state)
	{
	case MARIO_STATE_RUNNING_RIGHT:
		if (isSitting) break;
		maxVx = MARIO_RUNNING_SPEED;
		ax = MARIO_ACCEL_RUN_X;
		nx = 1;
		isRunning = true;
		break;
	case MARIO_STATE_RUNNING_LEFT:
		if (isSitting) break;
		maxVx = -MARIO_RUNNING_SPEED;
		ax = -MARIO_ACCEL_RUN_X;
		nx = -1;
		isRunning = true;
		break;
	case MARIO_STATE_WALKING_RIGHT:
		if (isSitting) break;
		maxVx = MARIO_WALKING_SPEED;
		ax = MARIO_ACCEL_WALK_X;
		nx = 1;
		isRunning = false;
		if (isHolding && heldKoopa && !IsKeyDown(DIK_A))
		{
			heldKoopa->SetState(RED_KOOPA_STATE_SHELL_RUNNING);
			heldKoopa->SetDirection(nx);
			SetHeldKoopa(nullptr);
		}
		break;
	case MARIO_STATE_WALKING_LEFT:
		if (isSitting) break;
		maxVx = -MARIO_WALKING_SPEED;
		ax = -MARIO_ACCEL_WALK_X;
		nx = -1;
		isRunning = false;
		if (isHolding && heldKoopa && !IsKeyDown(DIK_A))
		{
			heldKoopa->SetState(RED_KOOPA_STATE_SHELL_RUNNING);
			heldKoopa->SetDirection(nx);
			SetHeldKoopa(nullptr);
		}
		break;
	case MARIO_STATE_JUMP:
		if (isSitting) break;
		if (isOnPlatform)
		{
			if (abs(this->vx) == MARIO_RUNNING_SPEED)
				vy = -MARIO_JUMP_RUN_SPEED_Y;
			else
				vy = -MARIO_JUMP_SPEED_Y;
		}
		break;
	case MARIO_STATE_RELEASE_JUMP:
		if (vy < 0) vy += MARIO_JUMP_SPEED_Y / 2;
		break;
	case MARIO_STATE_SIT:
		if (isOnPlatform && level != MARIO_LEVEL_SMALL)
		{
			state = MARIO_STATE_IDLE;
			isSitting = true;
			vx = 0; vy = 0.0f;
			y += MARIO_SIT_HEIGHT_ADJUST;
		}
		break;
	case MARIO_STATE_SIT_RELEASE:
		if (isSitting)
		{
			isSitting = false;
			state = MARIO_STATE_IDLE;
			y -= MARIO_SIT_HEIGHT_ADJUST;
		}
		break;
	case MARIO_STATE_IDLE:
		ax = 0.0f;
		vx = 0.0f;
		isRunning = false;
		if (isHolding && heldKoopa && !IsKeyDown(DIK_A))
		{
			heldKoopa->SetState(RED_KOOPA_STATE_SHELL_RUNNING);
			heldKoopa->SetDirection(nx);
			SetHeldKoopa(nullptr);
		}
		break;
	case MARIO_STATE_DIE:
		vy = -MARIO_JUMP_DEFLECT_SPEED;
		vx = 0;
		ax = 0;
		if (isHolding && heldKoopa)
		{
			heldKoopa->SetState(RED_KOOPA_STATE_SHELL_RUNNING);
			heldKoopa->SetDirection(nx);
			SetHeldKoopa(nullptr);
		}
		break;
	}

	CGameObject::SetState(state);
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
	if (level == MARIO_LEVEL_BIG)
	{
		if (isSitting)
		{
			left = x - MARIO_BIG_SITTING_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_SITTING_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_SITTING_BBOX_WIDTH;
			bottom = top + MARIO_BIG_SITTING_BBOX_HEIGHT;
		}
		else
		{
			left = x - MARIO_BIG_BBOX_WIDTH / 2;
			top = y - MARIO_BIG_BBOX_HEIGHT / 2;
			right = left + MARIO_BIG_BBOX_WIDTH;
			bottom = top + MARIO_BIG_BBOX_HEIGHT;
		}
	}
	else
	{
		left = x - MARIO_SMALL_BBOX_WIDTH / 2;
		top = y - MARIO_SMALL_BBOX_HEIGHT / 2;
		right = left + MARIO_SMALL_BBOX_WIDTH;
		bottom = top + MARIO_SMALL_BBOX_HEIGHT;
	}
}

void CMario::SetLevel(int l)
{
	if (l != level)
	{
		isBlinking = true;
		isLocked = true;
		blinkStart = GetTickCount64();

		if (this->level == MARIO_LEVEL_SMALL && l == MARIO_LEVEL_BIG)
		{
			y -= (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
		}
		else if (this->level == MARIO_LEVEL_BIG && l == MARIO_LEVEL_SMALL)
		{
			y += (MARIO_BIG_BBOX_HEIGHT - MARIO_SMALL_BBOX_HEIGHT) / 2;
		}
		vx = 0.0f;
		ax = 0.0f;
		SetState(MARIO_STATE_IDLE);
	}

	if (isHolding && heldKoopa)
	{
		heldKoopa->SetState(RED_KOOPA_STATE_SHELL_RUNNING);
		heldKoopa->SetDirection(nx);
		SetHeldKoopa(nullptr);
	}
	level = l;
}