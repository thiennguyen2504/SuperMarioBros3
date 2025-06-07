#pragma once
#include "GameObject.h"

#define BLACKBACKGROUND_SPRITE_ID_VERTICAL 140009
#define BLACKBACKGROUND_SPRITE_ID_FILL 140010
#define BLACKBACKGROUND_WIDTH 16 
#define BLACKBACKGROUND_HEIGHT 16 
#define BLACKBACKGROUND_SCALE 0.33f
class BlackBackground : public CGameObject
{
protected:
    float mapHeight;
    float mapWidth;

public:
    BlackBackground(float x, float y, float mapWidth, float mapHeight);
    virtual void Render() override;
    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
	int IsBlocking() override { return 0; } // Not blocking any collisions
};