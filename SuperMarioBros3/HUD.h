#pragma once
#include "GameObject.h"
#include "Sprites.h"

#define SCORE_DIGITS 7
#define COIN_DIGITS 2
#define TIME_DIGITS 3
#define LIVES_DIGITS 1

class CHUD
{
protected:
	float x, y;
	LPSPRITE hudSprite;
	LPSPRITE cardSprite;
	LPSPRITE arrowBlackSprite;
	LPSPRITE pMeterBlackSprite;
	int score;
	int coins;
	int time;
	int lives;

public:
	CHUD();
	void Render();
};

typedef CHUD* LPHUD;