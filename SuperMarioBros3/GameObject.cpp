#include <d3dx10.h>
#include <algorithm>
#include "debug.h"
#include "Textures.h"
#include "Game.h"
#include "GameObject.h"
#include "Sprites.h"


void CGameObject::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
}

void CGameObject::RenderBoundingBox()
{
    D3DXVECTOR3 p(x, y, 0);
    RECT rect;
    LPTEXTURE bbox = CTextures::GetInstance()->Get(ID_TEX_BBOX);
    float l, t, r, b;
    GetBoundingBox(l, t, r, b);
    rect.left = 0;
    rect.top = 0;
    rect.right = (int)r - (int)l;
    rect.bottom = (int)b - (int)t;
    float cx, cy;
    CGame::GetInstance()->GetCamPos(cx, cy);
    CGame::GetInstance()->Draw(x - cx, y - cy, bbox, &rect, BBOX_ALPHA);
}

void CGameObject::OnNoCollision(DWORD dt)
{
    x += vx * dt;
    y += vy * dt;
}

CGameObject::~CGameObject()
{
}