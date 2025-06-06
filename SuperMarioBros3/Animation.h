#pragma once
#include <vector>
#include <Windows.h>
#include "Sprite.h"
#include "AnimationFrame.h"

using namespace std;

class CAnimation
{
	DWORD lastFrameTime;
	int defaultTime;
	int currentFrame;
	vector<LPANIMATION_FRAME> frames;

public:
	CAnimation(int defaultTime = 100) { this->defaultTime = defaultTime; lastFrameTime = -1; currentFrame = -1; }
	void Add(int spriteId, DWORD time = 0);
	void Render(float x, float y);
	void RenderFlipped180(float x, float y); // New method to render with 180-degree flip
};

typedef CAnimation* LPANIMATION;