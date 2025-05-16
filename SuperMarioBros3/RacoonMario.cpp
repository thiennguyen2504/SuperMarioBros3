#include "RacoonMario.h"
#include "Goomba.h"
#include "VenusFire.h"
#include "Fireball.h"
#include "RedKoopaTroopa.h"
#include "RedParaGoomba.h"
#include "PlayScene.h"
#include "Mario.h"
#include "Portal.h"

void CRaccoonMario::OnHit()
{
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	CMario* bigMario = new CMario(x, y);
	bigMario->SetLevel(MARIO_LEVEL_BIG);
	bigMario->SetState(state);
	bigMario->SetSpeed(vx, vy);
	bigMario->SetNx(nx);
	bigMario->StartUntouchable();
	scene->AddObject(bigMario);
	scene->SetPlayer(bigMario);
	Delete();
}

int CRaccoonMario::GetAniIdRaccoon()
{
	int aniId = -1;

	if (isHolding)
	{
		if (vx == 0)
		{
			aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_HOLD_RIGHT : ID_ANI_RACCOON_MARIO_HOLD_LEFT;
		}
		else if (abs(ax) == MARIO_ACCEL_RUN_X)
		{
			aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_HOLD_RIGHT : ID_ANI_RACCOON_MARIO_HOLD_LEFT;
		}
		else
		{
			aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_HOLD_RIGHT : ID_ANI_RACCOON_MARIO_HOLD_LEFT;
		}
	}
	else
	{
		if (!isOnPlatform)
		{
			if (abs(ax) == MARIO_ACCEL_RUN_X)
			{
				aniId = (nx >= 0) ? ID_ANI_RACCOON_MARIO_JUMP_RIGHT : ID_ANI_RACCOON_MARIO_JUMP_LEFT;
			}
			else
			{
				if (vy < 0)
					aniId = (nx >= 0) ? ID_ANI_RACCOON_MARIO_JUMP_RIGHT : ID_ANI_RACCOON_MARIO_JUMP_LEFT;
				else
					aniId = (nx >= 0) ? ID_ANI_RACCOON_MARIO_FALL_RIGHT : ID_ANI_RACCOON_MARIO_FALL_LEFT;
			}
		}
		else if (isSitting)
		{
			aniId = (nx > 0) ? ID_ANI_MARIO_SIT_RIGHT : ID_ANI_MARIO_SIT_LEFT;
		}
		else if (vx == 0)
		{
			aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_IDLE_RIGHT : ID_ANI_RACCOON_MARIO_IDLE_LEFT;
		}
		else if (vx > 0)
		{
			if (ax < 0)
				aniId = ID_ANI_RACCOON_MARIO_BRACE_RIGHT;
			else if (ax == MARIO_ACCEL_RUN_X)
				aniId = ID_ANI_RACCOON_MARIO_RUN_RIGHT;
			else if (ax == MARIO_ACCEL_WALK_X)
				aniId = ID_ANI_RACCOON_MARIO_WALK_RIGHT;
		}
		else
		{
			if (ax > 0)
				aniId = ID_ANI_RACCOON_MARIO_BRACE_LEFT;
			else if (ax == -MARIO_ACCEL_RUN_X)
				aniId = ID_ANI_RACCOON_MARIO_RUN_LEFT;
			else if (ax == -MARIO_ACCEL_WALK_X)
				aniId = ID_ANI_RACCOON_MARIO_WALK_LEFT;
		}
	}

	if (aniId == -1)
		aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_IDLE_RIGHT : ID_ANI_RACCOON_MARIO_IDLE_LEFT;

	return aniId;
}

void CRaccoonMario::OnCollisionWith(LPCOLLISIONEVENT e)
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
	else if (dynamic_cast<RedParaGoomba*>(e->obj))
		OnCollisionWithRedParaGoomba(e);
}

void CRaccoonMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
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
		if (untouchable == 0 && goomba->GetState() != ENEMY_STATE_DIE)
		{
			OnHit();
		}
	}
}

void CRaccoonMario::OnCollisionWithVenusFire(LPCOLLISIONEVENT e)
{
	VenusFire* venusFire = dynamic_cast<VenusFire*>(e->obj);

	if (e->ny >= 0 && untouchable == 0)
	{
		OnHit();
	}
}

void CRaccoonMario::OnCollisionWithFireball(LPCOLLISIONEVENT e)
{
	Fireball* fireball = dynamic_cast<Fireball*>(e->obj);
	fireball->Delete();

	if (untouchable == 0)
	{
		OnHit();
	}
}

void CRaccoonMario::OnCollisionWithRedKoopaTroopa(LPCOLLISIONEVENT e)
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
				OnHit();
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

void CRaccoonMario::OnCollisionWithRedParaGoomba(LPCOLLISIONEVENT e)
{
	RedParaGoomba* paraGoomba = dynamic_cast<RedParaGoomba*>(e->obj);

	if (e->ny < 0)
	{
		CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		CGoomba* goomba = new CGoomba(paraGoomba->GetX(), paraGoomba->GetY(), GOOMBA_TYPE_RED);
		goomba->SetState(ENEMY_STATE_WALKING);
		scene->AddObject(goomba);
		paraGoomba->Delete();
		vy = -MARIO_JUMP_DEFLECT_SPEED;
	}
	else if (e->nx != 0)
	{
		if (untouchable == 0 && paraGoomba->GetState() != RED_PARAGOOMBA_STATE_DIE)
		{
			OnHit();
		}
	}
}

void CRaccoonMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
	CPortal* p = (CPortal*)e->obj;
	CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}

void CRaccoonMario::OnHitByKoopa()
{
	if (untouchable == 0)
	{
		OnHit();
	}
}

void CRaccoonMario::Render()
{
	if (isDeleted) return;

	CAnimations* animations = CAnimations::GetInstance();
	int aniId = -1;

	bool shouldRender = true;
	if (untouchable == 1 && untouchable_start != -1)
	{
		ULONGLONG timeSinceUntouchable = GetTickCount64() - untouchable_start;
		ULONGLONG blinkPhase = timeSinceUntouchable % (2 * MARIO_UNTOUCHABLE_BLINK_INTERVAL);
		shouldRender = (blinkPhase < MARIO_UNTOUCHABLE_BLINK_INTERVAL);
	}

	if (isAppearing && appearStart != 0)
	{
		ULONGLONG timeSinceAppear = GetTickCount64() - appearStart;
		if (timeSinceAppear < MARIO_APPEAR_TIME)
		{
			aniId = ID_ANI_RACCOON_MARIO_APPEAR;
		}
		else
		{
			isAppearing = false;
			appearStart = 0;
			aniId = GetAniIdRaccoon();
		}
	}
	else
	{
		aniId = GetAniIdRaccoon();
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

void CRaccoonMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
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
		left = x - MARIO_RACCOON_BBOX_WIDTH / 2;
		top = y - MARIO_RACCOON_BBOX_HEIGHT / 2;
		right = left + MARIO_RACCOON_BBOX_WIDTH;
		bottom = top + MARIO_RACCOON_BBOX_HEIGHT;
	}
}