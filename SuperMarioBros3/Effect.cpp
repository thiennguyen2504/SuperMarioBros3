#include "Effect.h"
#include "Game.h"
#include "PlayScene.h"
#include "HUD.h"

CEffect::CEffect(float x, float y, int point)
{
	this->x = x;
	this->y = y;
	this->point = point;
	vy = EFFECT_VY;
	startTime = GetTickCount64();
}

void CEffect::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
	CGameObject::Update(dt, coObjects);

	// Di chuy?n l�n
	y += vy * dt;

	// Ki?m tra th?i gian t?n t?i
	if (GetTickCount64() - startTime > EFFECT_DURATION)
	{
		// C?ng ?i?m v�o HUD
		CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
		CHUD* hud = scene->GetHUD(); // Gi? ??nh CPlayScene c� ph??ng th?c GetHUD
		if (hud)
			hud->AddScore(point);

		// X�a hi?u ?ng
		Delete();
	}
}

void CEffect::Render()
{
	LPSPRITE sprite = CSprites::GetInstance()->Get(point); // point l� sprite ID (100, 200, 1000, 2000)
	if (sprite != nullptr)
	{
		sprite->Draw(x, y);
	}
}