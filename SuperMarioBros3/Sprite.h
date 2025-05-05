#pragma once

#include "Texture.h"

class CSprite
{
    int id;				// Sprite ID in the sprite database

    int left;
    int top;
    int right;
    int bottom;

    LPTEXTURE texture;
    D3DX10_SPRITE sprite;
    D3DXMATRIX matScaling;

public:
    CSprite(int id, int left, int top, int right, int bottom, LPTEXTURE tex);

    void Draw(float x, float y);
    void DrawWithScale(float x, float y, float scale);
    void DrawWithScaleY(float x, float y, float scale_y, float scale_x);

    // Getters for sprite coordinates
    int GetBottom() const { return bottom; }
    int GetTop() const { return top; }
    int GetLeft() const { return left; }
    int GetRight() const { return right; }
};

typedef CSprite* LPSPRITE;