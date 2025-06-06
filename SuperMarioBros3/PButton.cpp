#include "PButton.h"
#include "PlayScene.h"
#include "GoldBrick.h"
#include "StaticCoin.h"
#include "debug.h"

PButton::PButton(float x, float y) : CGameObject(x, y)
{
    SetState(P_BUTTON_STATE_NORMAL);
}

void PButton::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - P_BUTTON_BBOX_WIDTH / 2;
    top = y - P_BUTTON_BBOX_HEIGHT / 2;
    right = left + P_BUTTON_BBOX_WIDTH;
    bottom = top + P_BUTTON_BBOX_HEIGHT;
}

void PButton::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    CGameObject::Update(dt, coObjects);
}

void PButton::Render()
{
    if (isDeleted) return;
	if (state == P_BUTTON_STATE_NORMAL)
	{
		CAnimations::GetInstance()->Get(ID_ANI_P_BUTTON_NORMAL)->Render(x, y);
	}
	else if (state == P_BUTTON_STATE_PRESSED)
	{
		CAnimations::GetInstance()->Get(ID_ANI_P_BUTTON_PRESSED)->Render(x, y + 3.0f);
	}
}

void PButton::SetState(int state)
{
    if (state == P_BUTTON_STATE_PRESSED && this->state == P_BUTTON_STATE_NORMAL)
    {
        CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
        vector<LPGAMEOBJECT> goldBricks = scene->GetGoldBricks();
        for (LPGAMEOBJECT brick : goldBricks)
        {
            if (brick && !brick->IsDeleted() && dynamic_cast<GoldBrick*>(brick))
            {
                float bx, by;
                brick->GetPosition(bx, by);
                StaticCoin* coin = new StaticCoin(bx, by, 40004); 
                scene->AddObject(coin);
                brick->Delete();
            }
        }
    }
    CGameObject::SetState(state);
}