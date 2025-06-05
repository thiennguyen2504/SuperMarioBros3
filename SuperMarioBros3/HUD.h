#pragma once
#include "GameObject.h"
#include "Sprites.h"

#define SCORE_DIGITS 7
#define COIN_DIGITS 2
#define TIME_DIGITS 3
#define LIVES_DIGITS 1
#define P_METER_BLINK_INTERVAL 150 // ms for P-Meter blinking

class CHUD
{
protected:
	float x, y;
	LPSPRITE hudSprite;
	LPSPRITE cardSprite;
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
	void AddScore(int points) { score += points; } // Thêm ?i?m
};

typedef CHUD* LPHUD;