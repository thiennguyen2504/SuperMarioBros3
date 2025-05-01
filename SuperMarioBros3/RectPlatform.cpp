#include "RectPlatform.h"

#include "Sprite.h"
#include "Sprites.h"

#include "Textures.h"
#include "Game.h"

void CRectPlatform::Render()
{
	if (this->width <= 0 || this->height <= 0) return;

	CSprites* s = CSprites::GetInstance();
	float cx, cy;
	CGame::GetInstance()->GetCamPos(cx, cy);

	// Render corners
	// Top-left corner
	s->Get(this->spriteIdTopLeft)->DrawWithScale(
		x - (this->width * this->cellWidth * this->scale) / 2,
		y - (this->height * this->cellHeight * this->scale) / 2,
		this->scale);

	// Top-right corner
	s->Get(this->spriteIdTopRight)->DrawWithScale(
		x + (this->width * this->cellWidth * this->scale) / 2 - this->cellWidth * this->scale,
		y - (this->height * this->cellHeight * this->scale) / 2,
		this->scale);

	// Bottom-left corner
	s->Get(this->spriteIdBottomLeft)->DrawWithScale(
		x - (this->width * this->cellWidth * this->scale) / 2,
		y + (this->height * this->cellHeight * this->scale) / 2 - this->cellHeight * this->scale,
		this->scale);

	// Bottom-right corner
	s->Get(this->spriteIdBottomRight)->DrawWithScale(
		x + (this->width * this->cellWidth * this->scale) / 2 - this->cellWidth * this->scale,
		y + (this->height * this->cellHeight * this->scale) / 2 - this->cellHeight * this->scale,
		this->scale);

	// Render edges
	// Top edge
	for (int i = 1; i < this->width - 1; i++) {
		s->Get(this->spriteIdTop)->DrawWithScale(
			x - (this->width * this->cellWidth * this->scale) / 2 + i * this->cellWidth * this->scale,
			y - (this->height * this->cellHeight * this->scale) / 2,
			this->scale);
	}

	// Bottom edge
	for (int i = 1; i < this->width - 1; i++) {
		s->Get(this->spriteIdBottom)->DrawWithScale(
			x - (this->width * this->cellWidth * this->scale) / 2 + i * this->cellWidth * this->scale,
			y + (this->height * this->cellHeight * this->scale) / 2 - this->cellHeight * this->scale,
			this->scale);
	}

	// Left edge
	for (int i = 1; i < this->height - 1; i++) {
		s->Get(this->spriteIdLeft)->DrawWithScale(
			x - (this->width * this->cellWidth * this->scale) / 2,
			y - (this->height * this->cellHeight * this->scale) / 2 + i * this->cellHeight * this->scale,
			this->scale);
	}

	// Right edge
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
	if (nx == 0 && ny == -1) return 1;
	else return 0;
}