#include "SampleKeyEventHandler.h"

#include "debug.h"
#include "Game.h"

#include "Mario.h"
#include "PlayScene.h"

void CSampleKeyHandler::OnKeyDown(int KeyCode)
{
	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	switch (KeyCode)
	{
	case DIK_DOWN:
		mario->SetState(MARIO_STATE_SIT);
		break;
	case DIK_S:
		mario->SetState(MARIO_STATE_JUMP);
		break;
	case DIK_1:
		mario->SetLevel(MARIO_LEVEL_SMALL);
		break;
	case DIK_2:
		mario->SetLevel(MARIO_LEVEL_BIG);
		break;
	case DIK_0:
		mario->SetState(MARIO_STATE_DIE);
		break;
	case DIK_R:
		break;
	}
}

void CSampleKeyHandler::OnKeyUp(int KeyCode)
{
	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();
	switch (KeyCode)
	{
	case DIK_DOWN:
		mario->SetState(MARIO_STATE_SIT_RELEASE);
		break;
	case DIK_S:
		mario->SetState(MARIO_STATE_RELEASE_JUMP);
		break;
	case DIK_K:
		mario->SetState(MARIO_STATE_SIT_RELEASE);
		break;
	case DIK_A:
		if (mario->IsHolding())
		{
			RedKoopaTroopa* koopa = mario->GetHeldKoopa();
			koopa->SetState(RED_KOOPA_STATE_SHELL_RUNNING);
			koopa->SetDirection(mario->GetDirection());
			mario->SetHeldKoopa(nullptr);
		}
		mario->SetRunning(false);
		break;
	}
}

void CSampleKeyHandler::KeyState(BYTE* states)
{
	LPGAME game = CGame::GetInstance();
	CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

	if (game->IsKeyDown(DIK_L))
	{
		if (game->IsKeyDown(DIK_A))
		{
			mario->SetRunning(true);
			mario->SetState(MARIO_STATE_RUNNING_RIGHT);
		}
		else
		{
			mario->SetState(MARIO_STATE_WALKING_RIGHT);
		}
	}
	else if (game->IsKeyDown(DIK_J))
	{
		if (game->IsKeyDown(DIK_A))
		{
			mario->SetRunning(true);
			mario->SetState(MARIO_STATE_RUNNING_LEFT);
		}
		else
		{
			mario->SetState(MARIO_STATE_WALKING_LEFT);
		}
	}
	else
	{
		mario->SetState(MARIO_STATE_IDLE);
	}
}