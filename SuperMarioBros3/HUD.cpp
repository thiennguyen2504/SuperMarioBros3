#include "HUD.h"
#include "Game.h"
#include "debug.h"

void CHUD::Render()
{
    CSprites* sprites = CSprites::GetInstance();
    LPSPRITE hudSprite = sprites->Get(HUD_SPRITE_ID);

    float backBufferWidth = CGame::GetInstance()->GetBackBufferWidth();
    float backBufferHeight = CGame::GetInstance()->GetBackBufferHeight();

    float spriteWidth = hudSprite->GetRight() - hudSprite->GetLeft();
    float spriteHeight = hudSprite->GetBottom() - hudSprite->GetTop();

    float targetX = (backBufferWidth - spriteWidth) / 2.0f + 100.0f; 
    float targetY = backBufferHeight - spriteHeight;     

    // Lấy tọa độ camera
    float cx, cy;
    CGame::GetInstance()->GetCamPos(cx, cy);

    // Vẽ HUD tại tọa độ đã bù camera để hủy offset trong CSprite::Draw
    float renderX = targetX + cx;
    float renderY = targetY + cy;
    hudSprite->Draw(renderX, renderY);

}