#pragma once
#include "GameObject.h"
#include "Animation.h"
#include "Animations.h"

#define GRASS_SPRITE_HEAD 140000
#define GRASS_SPRITE_BODY_LEFT 140001
#define GRASS_SPRITE_BODY_RIGHT 140002
#define GRASS_SPRITE_BODY_MID 140003

#define BLACK_GRASS_SPRITE_HEAD 140011
#define BLACK_GRASS_SPRITE_BODY_LEFT 140012
#define BLACK_GRASS_SPRITE_BODY_RIGHT 140013
#define BLACK_GRASS_SPRITE_BODY_MID 140010

#define GRASS_TYPE_LEFT 1
#define GRASS_TYPE_MID 2
#define GRASS_TYPE_RIGHT 3

#define BLACK_GRASS_TYPE_LEFT 4
#define BLACK_GRASS_TYPE_RIGHT 5
#define BLACK_GRASS_TYPE_MID 6

#define GRASS_SCALE 0.33f

#define GRASS_HEAD_WIDTH 89.0f * GRASS_SCALE
#define GRASS_HEAD_HEIGHT 47.0f * GRASS_SCALE
#define GRASS_BODY_WIDTH 47.0f * GRASS_SCALE
#define GRASS_BODY_HEIGHT 47.0f * GRASS_SCALE



class CGrass : public CGameObject
{
protected:
	int height; 
	int type;   

public:
	CGrass(float x, float y, int height, int type);
	void Render() override;
	void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override {}
	void GetBoundingBox(float& left, float& top, float& right, float& bottom) override {}
	int IsCollidable() { return 0; }
	int IsBlocking() { return 0; }
};typedef CGrass* LPGRASS;