#include "RectPlatform.h"

#include "Sprite.h"
#include "Sprites.h"

#include "Textures.h"
#include "Game.h"
#include "AssetIDs.h"

// Hằng số cho sprite bóng
#define SHADOW_SPRITE_WIDTH 22.0f
#define SHADOW_SPRITE_HEIGHT 97.0f
#define SHADOW_HEIGHT_REDUCTION 20.0f
#define SHADOW_OFFSET_X 6.0f

void CRectPlatform::Render()
{
    if (this->width <= 0 || this->height <= 0) return;

    CSprites* s = CSprites::GetInstance();
    float cx, cy;
    CGame::GetInstance()->GetCamPos(cx, cy);

    // Render shadow
    float l, t, r, b;
    GetBoundingBox(l, t, r, b);
    float platform_height = this->height * this->cellHeight * this->scale;
    float shadow_height = (this->height * this->cellHeight - SHADOW_HEIGHT_REDUCTION) * this->scale;
    float shadow_scale_y = shadow_height / SHADOW_SPRITE_HEIGHT;
    float shadow_scale_x = this->scale;
    float shadow_width = SHADOW_SPRITE_WIDTH * shadow_scale_x;
    float shadow_x = r + (shadow_width / 2.0f) + SHADOW_OFFSET_X;
    float shadow_y = b - shadow_height / 2;
    s->Get(ID_SPRITE_PLATFORM_SHADOW)->DrawWithScaleY(shadow_x, shadow_y, shadow_scale_y, shadow_scale_x);

    // Render corners
    s->Get(this->spriteIdTopLeft)->DrawWithScale(
        x - (this->width * this->cellWidth * this->scale) / 2,
        y - (this->height * this->cellHeight * this->scale) / 2,
        this->scale);

    s->Get(this->spriteIdTopRight)->DrawWithScale(
        x + (this->width * this->cellWidth * this->scale) / 2 - this->cellWidth * this->scale,
        y - (this->height * this->cellHeight * this->scale) / 2,
        this->scale);

    s->Get(this->spriteIdBottomLeft)->DrawWithScale(
        x - (this->width * this->cellWidth * this->scale) / 2,
        y + (this->height * this->cellHeight * this->scale) / 2 - this->cellHeight * this->scale,
        this->scale);

    s->Get(this->spriteIdBottomRight)->DrawWithScale(
        x + (this->width * this->cellWidth * this->scale) / 2 - this->cellWidth * this->scale,
        y + (this->height * this->cellHeight * this->scale) / 2 - this->cellHeight * this->scale,
        this->scale);

    // Render edges
    for (int i = 1; i < this->width - 1; i++) {
        s->Get(this->spriteIdTop)->DrawWithScale(
            x - (this->width * this->cellWidth * this->scale) / 2 + i * this->cellWidth * this->scale,
            y - (this->height * this->cellHeight * this->scale) / 2,
            this->scale);
    }

    for (int i = 1; i < this->width - 1; i++) {
        s->Get(this->spriteIdBottom)->DrawWithScale(
            x - (this->width * this->cellWidth * this->scale) / 2 + i * this->cellWidth * this->scale,
            y + (this->height * this->cellHeight * this->scale) / 2 - this->cellHeight * this->scale,
            this->scale);
    }

    for (int i = 1; i < this->height - 1; i++) {
        s->Get(this->spriteIdLeft)->DrawWithScale(
            x - (this->width * this->cellWidth * this->scale) / 2,
            y - (this->height * this->cellHeight * this->scale) / 2 + i * this->cellHeight * this->scale,
            this->scale);
    }

    for (int i = 1; i < this->height - 1; i++) {
        s->Get(this->spriteIdRight)->DrawWithScale(
            x + (this->width * this->cellWidth * this->scale) / 2 - this->cellWidth * this->scale,
            y - (this->height * this->cellHeight * this->scale) / 2 + i * this->cellHeight * this->scale,
            this->scale);
    }

    // Render middle part
    for (int i = 1; i < this->width - 1; i++) {
        for (int j = 1; j < this->height - 1; j++) {
            s->Get(this->spriteIdMiddle)->DrawWithScale(
                x - (this->width * this->cellWidth * this->scale) / 2 + i * this->cellWidth * this->scale,
                y - (this->height * this->cellHeight * this->scale) / 2 + j * this->cellHeight * this->scale,
                this->scale);
        }
    }
}

void CRectPlatform::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - (this->width * this->cellWidth * this->scale) / 2;
    t = y - ((this->height + 1) * this->cellHeight * this->scale) / 2;
    r = x + (this->width * this->cellWidth * this->scale) / 2 - this->cellWidth * this->scale;
    b = y + ((this->height - 1) * this->cellHeight * this->scale) / 2;
}

int CRectPlatform::IsDirectionColliable(float nx, float ny)
{
    if (nx == 0 && (ny == -1 || ny < 0)) return 1;
    else return 0;
}