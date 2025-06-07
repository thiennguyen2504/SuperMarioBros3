#pragma once
#include "GameObject.h"
#include "Sprites.h"

#define CARD_SPRITE_FLOWER 140016
#define CARD_SPRITE_STAR 140015
#define CARD_SPRITE_MUSHROOM 140017
#define CARD_HUD_MUSHROOM 150007
#define CARD_HUD_FLOWER 150008
#define CARD_HUD_STAR 150009
#define CARD_SWITCH_INTERVAL 500 
#define CARD_BBOX_WIDTH 16
#define CARD_BBOX_HEIGHT 16
#define CARD_SCALE 0.3f

enum class CardType {
    FLOWER,
    STAR,
    MUSHROOM
};

class Card : public CGameObject
{
protected:
    CardType currentType;
    ULONGLONG lastSwitchTime;

public:
    Card(float x, float y);
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() override;
    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void OnCollisionWith(LPCOLLISIONEVENT e) override;
    CardType GetCardType() { return currentType; }
};

typedef Card* LPCARD;