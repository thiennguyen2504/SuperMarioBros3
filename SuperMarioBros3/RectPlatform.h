#pragma once

#include "GameObject.h"

// 
// Rectangular Platform that only blocks collision from top 
// 
class CRectPlatform : public CGameObject
{
protected:
	int width;				// Number of cells horizontally
	int height;				// Number of cells vertically
	float cellWidth;		// Width of each cell
	float cellHeight;		// Height of each cell
	float scale;			// Scale factor for rendering

	// Sprite IDs for the corners
	int spriteIdTopLeft;
	int spriteIdTopRight;
	int spriteIdBottomLeft;
	int spriteIdBottomRight;

	// Sprite IDs for the edges
	int spriteIdTop;
	int spriteIdBottom;
	int spriteIdLeft;
	int spriteIdRight;

	// Sprite ID for the middle part
	int spriteIdMiddle;

public:
	CRectPlatform(float x, float y,
		float cell_width, float cell_height,
		int width, int height, float scale,
		int sprite_id_top_left, int sprite_id_top_right,
		int sprite_id_bottom_left, int sprite_id_bottom_right,
		int sprite_id_top, int sprite_id_bottom,
		int sprite_id_left, int sprite_id_right,
		int sprite_id_middle) : CGameObject(x, y)
	{
		this->width = width;
		this->height = height;
		this->cellWidth = cell_width;
		this->cellHeight = cell_height;
		this->scale = scale;

		this->spriteIdTopLeft = sprite_id_top_left;
		this->spriteIdTopRight = sprite_id_top_right;
		this->spriteIdBottomLeft = sprite_id_bottom_left;
		this->spriteIdBottomRight = sprite_id_bottom_right;

		this->spriteIdTop = sprite_id_top;
		this->spriteIdBottom = sprite_id_bottom;
		this->spriteIdLeft = sprite_id_left;
		this->spriteIdRight = sprite_id_right;

		this->spriteIdMiddle = sprite_id_middle;
	}

	void Render();
	void Update(DWORD dt) {}
	void GetBoundingBox(float& l, float& t, float& r, float& b);
	// Only collide from the top (similar to Platform class)
	int IsDirectionColliable(float nx, float ny);
};

typedef CRectPlatform* LPRECTPLATFORM;