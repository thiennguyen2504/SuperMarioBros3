#include "SampleKeyEventHandler.h"
#include "debug.h"
#include "Game.h"
#include "Mario.h"
#include "KoopaTroopa.h"
#include "PlayScene.h"
#include "RacoonMario.h"

void CSampleKeyHandler::OnKeyDown(int KeyCode)
{
    CMario* mario = (CMario*)((LPPLAYSCENE)CGame::GetInstance()->GetCurrentScene())->GetPlayer();

    switch (KeyCode)
    {
    case DIK_DOWN:
        mario->SetState(MARIO_STATE_SIT);
        break;
    case DIK_S:
        if (mario->GetIsOnPlatform())
        {
            mario->SetState(MARIO_STATE_JUMP);
        }
        else if (dynamic_cast<CRaccoonMario*>(mario) && mario->GetLevel() == MARIO_LEVEL_RACCOON)
        {
            CRaccoonMario* raccoonMario = dynamic_cast<CRaccoonMario*>(mario);
            if (raccoonMario->CanStartFlying() && !raccoonMario->IsFlying())
            {
                raccoonMario->SetState(MARIO_STATE_FLY);
            }
            raccoonMario->Flap();
        }
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
    case DIK_A:
        if (dynamic_cast<CRaccoonMario*>(mario) && !mario->IsHolding())
        {
            CRaccoonMario* raccoonMario = dynamic_cast<CRaccoonMario*>(mario);
            if (!raccoonMario->IsTailAttacking() &&
                GetTickCount64() - raccoonMario->GetTailAttackCooldown() > MARIO_TAIL_ATTACK_COOLDOWN)
            {
                mario->SetState(MARIO_STATE_TAIL_ATTACK);
            }
        }
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
            KoopaTroopa* koopa = mario->GetHeldKoopa();
            koopa->SetState(KOOPA_STATE_SHELL_RUNNING);
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

    if (!mario->GetIsOnPlatform() || (dynamic_cast<CRaccoonMario*>(mario) && dynamic_cast<CRaccoonMario*>(mario)->IsTailAttacking()))
    {
        if (game->IsKeyDown(DIK_L) && !game->IsKeyDown(DIK_J))
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
        else if (game->IsKeyDown(DIK_J) && !game->IsKeyDown(DIK_L))
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
    }
    else
    {
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
}