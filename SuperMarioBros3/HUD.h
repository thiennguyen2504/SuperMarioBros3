#pragma once
#include "GameObject.h"
#include "Sprites.h"
#include <vector>

#define SCORE_DIGITS 7
#define COIN_DIGITS 2
#define TIME_DIGITS 3
#define LIVES_DIGITS 1
#define P_METER_BLINK_INTERVAL 150 
#define HUD_CARD_EMPTY 150002

class CHUD
{
protected:
    float x, y;
    LPSPRITE hudSprite;
    std::vector<int> cardSpriteIds; 
    LPSPRITE arrowBlackSprite;
    LPSPRITE arrowWhiteSprite;
    LPSPRITE pMeterBlackSprite;
    LPSPRITE pMeterWhiteSprite;
    int score;
    int coins;
    int time;
    int lives;

public:
    CHUD();
    void Render();
    void AddScore(int points) { score += points; }
    void AddCoin() { coins++; }
    void UpdateCard(int spriteId);
    void AddLives() { lives++; }
};

typedef CHUD* LPHUD;