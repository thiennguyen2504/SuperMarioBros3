#include "Bush.h"
#include "Sprites.h"
#include "AssetIDs.h"

CBush::CBush(float x, float y) : CGameObject(x, y) {
}

void CBush::Render() {
    CSprites::GetInstance()->Get(ID_SPRITE_BUSH)->Draw(x, y);
}

void CBush::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {
    // Không c?n c?p nh?t vì b?i cây là ??i t??ng t?nh
}

void CBush::GetBoundingBox(float& l, float& t, float& r, float& b) {
    l = x - 12;  // Gi?m t? 24 xu?ng 12 ?? thu nh? 50%
    t = y - 12;  // Gi?m t? 24 xu?ng 12 ?? thu nh? 50%
    r = x + 12;
    b = y + 12;
}