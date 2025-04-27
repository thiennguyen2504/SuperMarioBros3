#include "Bush.h"
#include "Sprites.h"
#include "AssetIDs.h"

CBush::CBush(float x, float y, float scale) : CGameObject(x, y) {
    this->scale = scale;
}

void CBush::Render() {
    CSprites::GetInstance()->Get(ID_SPRITE_BUSH)->DrawWithScale(x, y, scale);
}

void CBush::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) {

}

void CBush::GetBoundingBox(float& l, float& t, float& r, float& b) {

    float width = (671 - 624) * scale / 2; 
    float height = (624 - 576) * scale / 2; 
    l = x - width;
    t = y - height;
    r = x + width;
    b = y + height;
}