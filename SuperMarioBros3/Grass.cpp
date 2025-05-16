#include "Grass.h"
#include "Sprites.h"

CGrass::CGrass(float x, float y, int height, int type) : CGameObject(x, y)
{
	this->height = height > 0 ? height : 1;
	this->type = type;
}

void CGrass::Render()
{
	CSprites* sprites = CSprites::GetInstance();
	float totalHeight = GRASS_HEAD_HEIGHT + height * GRASS_BODY_HEIGHT;
	float baseX = x;
	float baseY = y + totalHeight / 2.0f;

	for (int i = 0; i < height; i++)
	{
		float bodyY = baseY - (height - i - 1) * GRASS_BODY_HEIGHT - GRASS_BODY_HEIGHT;

		if (type == GRASS_TYPE_LEFT)
		{
			sprites->Get(GRASS_SPRITE_BODY_LEFT)->DrawWithScale(baseX - (GRASS_BODY_WIDTH / 2.0f), bodyY, GRASS_SCALE);
			sprites->Get(GRASS_SPRITE_BODY_MID)->DrawWithScale(baseX + (GRASS_BODY_WIDTH / 2.0f), bodyY, GRASS_SCALE);
		}
		else if (type == GRASS_TYPE_MID)
		{
			sprites->Get(GRASS_SPRITE_BODY_MID)->DrawWithScale(baseX - (GRASS_BODY_WIDTH / 2.0f), bodyY, GRASS_SCALE);
			sprites->Get(GRASS_SPRITE_BODY_MID)->DrawWithScale(baseX + (GRASS_BODY_WIDTH / 2.0f), bodyY, GRASS_SCALE);
		}
		else if (type == GRASS_TYPE_RIGHT)
		{
			sprites->Get(GRASS_SPRITE_BODY_MID)->DrawWithScale(baseX - (GRASS_BODY_WIDTH / 2.0f), bodyY, GRASS_SCALE);
			sprites->Get(GRASS_SPRITE_BODY_RIGHT)->DrawWithScale(baseX + (GRASS_BODY_WIDTH / 2.0f), bodyY, GRASS_SCALE);
		}
	}

	float headY = baseY - height * GRASS_BODY_HEIGHT - GRASS_HEAD_HEIGHT;
	sprites->Get(GRASS_SPRITE_HEAD)->DrawWithScale(baseX, headY, GRASS_SCALE);
}