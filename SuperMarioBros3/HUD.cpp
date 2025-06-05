#include "HUD.h"
#include "AssetIDs.h"
#include "Game.h"
#include "PlayScene.h"
#include "Mario.h"
#include <cmath>

CHUD::CHUD()
{
	// Get window dimensions
	float backWidth = CGame::GetInstance()->GetBackBufferWidth();
	float backHeight = CGame::GetInstance()->GetBackBufferHeight();

	// Position HUD
	x = 100.0f; // 100 pixels from left edge
	y = backHeight - HUD_HEIGHT / 2.0f; // Center vertically in HUD area (y=259)

	// Initialize values
	score = 100;
	coins = 0;
	time = 0;
	lives = 0;

	// Load sprites
	hudSprite = CSprites::GetInstance()->Get(ID_SPRITE_HUD);
	cardSprite = CSprites::GetInstance()->Get(ID_SPRITE_CARD);
	arrowBlackSprite = CSprites::GetInstance()->Get(ID_SPRITE_ARROW_BLACK);
	arrowWhiteSprite = CSprites::GetInstance()->Get(ID_SPRITE_ARROW_WHITE);
	pMeterBlackSprite = CSprites::GetInstance()->Get(ID_SPRITE_P_METER_BLACK);
	pMeterWhiteSprite = CSprites::GetInstance()->Get(ID_SPRITE_P_METER_WHITE);
}

void CHUD::Render()
{
	// Draw HUD
	if (hudSprite != nullptr)
	{
		hudSprite->DrawStatic(floor(x), floor(y));
	}

	// Draw 3 cards
	if (cardSprite != nullptr)
	{
		float backWidth = CGame::GetInstance()->GetBackBufferWidth();
		const float CARD_WIDTH = 24.0f; // Sprite 150002: 195-172=23, add 1 for border
		const float CARD_SPACING = 0.0f; // No gap between cards
		const float RIGHT_MARGIN = 10.0f; // 10 pixels from right edge

		for (int i = 0; i < 3; i++)
		{
			float cardX = backWidth - RIGHT_MARGIN - (i + 1) * CARD_WIDTH - i * CARD_SPACING;
			cardSprite->DrawStatic(floor(cardX), floor(y));
		}
	}

	// Draw digits
	const float DIGIT_WIDTH = 8.0f; // Sprite 0-9: 7 pixels wide, add 1 for border
	const float DIGIT_Y1 = floor(y - 4.0f); // y=255
	const float DIGIT_Y2 = floor(y + 4.0f); // y=263

	// Score (7 digits)
	int scoreTemp = score;
	int scoreDigits[SCORE_DIGITS] = { 0 };
	for (int i = SCORE_DIGITS - 1; i >= 0; i--)
	{
		scoreDigits[i] = scoreTemp % 10;
		scoreTemp /= 10;
	}
	float scoreX = 80.0f; // Start 10 pixels left of HUD left edge
	for (int i = 0; i < SCORE_DIGITS; i++)
	{
		LPSPRITE digitSprite = CSprites::GetInstance()->Get(scoreDigits[i]);
		if (digitSprite != nullptr)
		{
			digitSprite->DrawStatic(floor(scoreX + i * DIGIT_WIDTH), DIGIT_Y2);
		}
	}

	// Coins (2 digits)
	int coinsTemp = coins;
	int coinDigits[COIN_DIGITS] = { 0 };
	for (int i = COIN_DIGITS - 1; i >= 0; i--)
	{
		coinDigits[i] = coinsTemp % 10;
		coinsTemp /= 10;
	}
	float coinX = 160.0f; // Start after score
	for (int i = 0; i < COIN_DIGITS; i++)
	{
		LPSPRITE digitSprite = CSprites::GetInstance()->Get(coinDigits[i]);
		if (digitSprite != nullptr)
		{
			digitSprite->DrawStatic(floor(coinX + i * DIGIT_WIDTH), DIGIT_Y1);
		}
	}

	// Time (3 digits)
	int timeTemp = time;
	int timeDigits[TIME_DIGITS] = { 0 };
	for (int i = TIME_DIGITS - 1; i >= 0; i--)
	{
		timeDigits[i] = timeTemp % 10;
		timeTemp /= 10;
	}
	float timeX = 153.0f; // Start after coins
	for (int i = 0; i < TIME_DIGITS; i++)
	{
		LPSPRITE digitSprite = CSprites::GetInstance()->Get(timeDigits[i]);
		if (digitSprite != nullptr)
		{
			digitSprite->DrawStatic(floor(timeX + i * DIGIT_WIDTH), DIGIT_Y2);
		}
	}

	// Lives (1 digit)
	int livesDigit = lives % 10;
	float livesX = 65.0f; // Start before score
	LPSPRITE digitSprite = CSprites::GetInstance()->Get(livesDigit);
	if (digitSprite != nullptr)
	{
		digitSprite->DrawStatic(floor(livesX), DIGIT_Y2);
	}

	// Get runProgress from Mario
	CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
	CMario* mario = (CMario*)scene->GetPlayer();
	float runProgress = mario ? mario->GetRunProgress() : 0.0f;

	// Draw 6 Arrows
	const float ARROW_WIDTH = 8.0f; // Sprite 150004: 98-91=7, add 1 for border
	const float ARROW_Y = floor(y - 4.0f); // y=255
	const int NUM_ARROWS = 6;
	float arrowX = 80.0f; // Start at x=80

	for (int i = 0; i < NUM_ARROWS; i++)
	{
		LPSPRITE arrowSprite = (runProgress >= (i + 1) / (float)NUM_ARROWS) ?
			arrowWhiteSprite : arrowBlackSprite;
		if (arrowSprite != nullptr)
		{
			arrowSprite->DrawStatic(floor(arrowX + i * ARROW_WIDTH), ARROW_Y);
		}
	}

	// Draw P-Meter
	const float P_METER_WIDTH = 15.0f; // Sprite 150006: 114-100=14, add 1 for border
	float pMeterX = 133.0f; // After 6 arrows
	LPSPRITE pMeterSprite = pMeterBlackSprite;
	if (runProgress >= 1.0f)
	{
		ULONGLONG currentTime = GetTickCount64();
		ULONGLONG blinkPhase = currentTime % (2 * P_METER_BLINK_INTERVAL);
		pMeterSprite = (blinkPhase < P_METER_BLINK_INTERVAL) ? pMeterWhiteSprite : pMeterBlackSprite;
	}
	if (pMeterSprite != nullptr)
	{
		pMeterSprite->DrawStatic(floor(pMeterX), ARROW_Y);
	}
}