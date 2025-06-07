#include "HUD.h"
#include "AssetIDs.h"
#include "Game.h"
#include "PlayScene.h"
#include "Mario.h"
#include <cmath>

#define HUD_HEIGHT 26.0f 

CHUD::CHUD()
{
    float backWidth = CGame::GetInstance()->GetBackBufferWidth();
    float backHeight = CGame::GetInstance()->GetBackBufferHeight();

    x = 100.0f;
    y = backHeight - HUD_HEIGHT / 2.0f;

    score = 0;
    coins = 0;
    time = 0;
    lives = 0;

    hudSprite = CSprites::GetInstance()->Get(ID_SPRITE_HUD);
    cardSpriteIds = { HUD_CARD_EMPTY, HUD_CARD_EMPTY, HUD_CARD_EMPTY };
    arrowBlackSprite = CSprites::GetInstance()->Get(ID_SPRITE_ARROW_BLACK);
    arrowWhiteSprite = CSprites::GetInstance()->Get(ID_SPRITE_ARROW_WHITE);
    pMeterBlackSprite = CSprites::GetInstance()->Get(ID_SPRITE_P_METER_BLACK);
    pMeterWhiteSprite = CSprites::GetInstance()->Get(ID_SPRITE_P_METER_WHITE);
}

void CHUD::UpdateCard(int spriteId)
{
    if (cardSpriteIds.size() >= 3)
    {
        cardSpriteIds[0] = cardSpriteIds[1];
        cardSpriteIds[1] = cardSpriteIds[2];
        cardSpriteIds[2] = spriteId;
    }
}

void CHUD::Render()
{
    if (hudSprite != nullptr)
    {
        hudSprite->DrawStatic(floor(x), floor(y));
    }

    float backWidth = CGame::GetInstance()->GetBackBufferWidth();
    const float CARD_WIDTH = 24.0f;
    const float CARD_SPACING = 0.0f;
    const float RIGHT_MARGIN = 10.0f;

    for (int i = 0; i < 3; i++)
    {
        float cardX = backWidth - RIGHT_MARGIN - (i + 1) * CARD_WIDTH - i * CARD_SPACING;
        LPSPRITE cardSprite = CSprites::GetInstance()->Get(cardSpriteIds[i]);
        if (cardSprite != nullptr)
        {
            cardSprite->DrawStatic(floor(cardX), floor(y));
        }
    }

    const float DIGIT_WIDTH = 8.0f;
    const float DIGIT_Y1 = floor(y - 4.0f);
    const float DIGIT_Y2 = floor(y + 4.0f);

    int scoreTemp = score;
    int scoreDigits[SCORE_DIGITS] = { 0 };
    for (int i = SCORE_DIGITS - 1; i >= 0; i--)
    {
        scoreDigits[i] = scoreTemp % 10;
        scoreTemp /= 10;
    }
    float scoreX = 80.0f;
    for (int i = 0; i < SCORE_DIGITS; i++)
    {
        LPSPRITE digitSprite = CSprites::GetInstance()->Get(scoreDigits[i]);
        if (digitSprite != nullptr)
        {
            digitSprite->DrawStatic(floor(scoreX + i * DIGIT_WIDTH), DIGIT_Y2);
        }
    }

    int coinsTemp = coins;
    int coinDigits[COIN_DIGITS] = { 0 };
    for (int i = COIN_DIGITS - 1; i >= 0; i--)
    {
        coinDigits[i] = coinsTemp % 10;
        coinsTemp /= 10;
    }
    float coinX = 160.0f;
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
    float timeX = 153.0f;
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
    float livesX = 65.0f;
    LPSPRITE digitSprite = CSprites::GetInstance()->Get(livesDigit);
    if (digitSprite != nullptr)
    {
        digitSprite->DrawStatic(floor(livesX), DIGIT_Y2);
    }

    CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
    CMario* mario = (CMario*)scene->GetPlayer();
    float runProgress = mario ? mario->GetRunProgress() : 0.0f;

    const float ARROW_WIDTH = 8.0f;
    const float ARROW_Y = floor(y - 4.0f);
    const int NUM_ARROWS = 6;
    float arrowX = 80.0f;

    for (int i = 0; i < NUM_ARROWS; i++)
    {
        LPSPRITE arrowSprite = (runProgress >= (i + 1) / (float)NUM_ARROWS) ?
            arrowWhiteSprite : arrowBlackSprite;
        if (arrowSprite != nullptr)
        {
            arrowSprite->DrawStatic(floor(arrowX + i * ARROW_WIDTH), ARROW_Y);
        }
    }

    const float P_METER_WIDTH = 15.0f;
    float pMeterX = 133.0f;
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