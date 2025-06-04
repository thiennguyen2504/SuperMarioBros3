#include "HUD.h"
#include "AssetIDs.h"
#include "Game.h"
#include "PlayScene.h"
#include <cmath>

CHUD::CHUD()
{
	// Get window dimensions
	float backWidth = CGame::GetInstance()->GetBackBufferWidth();
	float backHeight = CGame::GetInstance()->GetBackBufferHeight();

	// Position HUD slightly left from the edge
	x = 100.0f; // 100 pixels from left edge
	y = backHeight - HUD_HEIGHT / 2.0f; // Center vertically in HUD area (y=259)

	// Initialize values
	score = 0;
	coins = 0;
	time = 0;
	lives = 0;

	hudSprite = CSprites::GetInstance()->Get(ID_SPRITE_HUD);
	cardSprite = CSprites::GetInstance()->Get(ID_SPRITE_CARD);
	arrowBlackSprite = CSprites::GetInstance()->Get(ID_SPRITE_ARROW_BLACK);
	pMeterBlackSprite = CSprites::GetInstance()->Get(ID_SPRITE_P_METER_BLACK);
}

void CHUD::Render()
{
	if (hudSprite != nullptr)
	{
		hudSprite->DrawStatic(floor(x), floor(y));
	}

	if (cardSprite != nullptr)
	{
		float backWidth = CGame::GetInstance()->GetBackBufferWidth();
		const float CARD_WIDTH = 24.0f; // Sprite 150002: 195-172=23, add 1 for border
		const float CARD_SPACING = 0.0f; // No gap between cards
		const float RIGHT_MARGIN = 10.0f; // 10 pixels from right edge

		// Draw 3 cards from right to left
		for (int i = 0; i < 3; i++)
		{
			float cardX = backWidth - RIGHT_MARGIN - (i + 1) * CARD_WIDTH - i * CARD_SPACING;
			cardSprite->DrawStatic(floor(cardX), floor(y));
		}
	}

	// Draw digits
	const float DIGIT_WIDTH = 8.0f; // Sprite 0-9: ~7 pixels wide, add 1 for border
	const float DIGIT_Y1 = floor(y - 4.0f); // y=255
	const float DIGIT_Y2 = floor(y - (-4.0f)); // y=263

	// Convert score to digits (7 digits)
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

	// Convert coins to digits (2 digits)
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

	// Convert time to digits (3 digits)
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

	// Convert lives to digit (1 digit)
	int livesDigit = lives % 10;
	float livesX = 65.0f; // Start before score
	LPSPRITE digitSprite = CSprites::GetInstance()->Get(livesDigit);
	if (digitSprite != nullptr)
	{
		digitSprite->DrawStatic(floor(livesX), DIGIT_Y2);
	}

	// Draw 6 Arrow Black and 1 P-Meter Black
	const float ARROW_WIDTH = 8.0f; // Sprite 150004: 98-91=7, add 1 for border
	const float P_METER_WIDTH = 15.0f; // Sprite 150006: 114-100=14, add 1 for border
	const float ARROW_Y = floor(y - 4.0f); // Align with coins (y=255)
	float arrowX = 80.0f; // Start after coins/time

	// Draw 6 Arrow Black
	for (int i = 0; i < 6; i++)
	{
		if (arrowBlackSprite != nullptr)
		{
			arrowBlackSprite->DrawStatic(floor(arrowX + i * (ARROW_WIDTH )), ARROW_Y);
		}
	}

	// Draw 1 P-Meter Black
	float pMeterX = 133.0f ; // After 6 arrows
	if (pMeterBlackSprite != nullptr)
	{
		pMeterBlackSprite->DrawStatic(floor(pMeterX), ARROW_Y);
	}
}