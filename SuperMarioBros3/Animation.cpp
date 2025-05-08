#include "Animation.h"
#include "debug.h"

void CAnimation::Add(int spriteId, DWORD time)
{
    int t = time;
    if (time == 0) t = this->defaultTime;

    LPSPRITE sprite = CSprites::GetInstance()->Get(spriteId);
    if (sprite == NULL)
    {
        DebugOut(L"[ERROR] Sprite ID %d not found!\n", spriteId);
        return; // Do not add frame if sprite is null
    }

    LPANIMATION_FRAME frame = new CAnimationFrame(sprite, t);
    frames.push_back(frame);
}

void CAnimation::Render(float x, float y)
{
    ULONGLONG now = GetTickCount64();
    if (currentFrame == -1)
    {
        currentFrame = 0;
        lastFrameTime = now;
    }
    else
    {
        DWORD t = frames[currentFrame]->GetTime();
        if (now - lastFrameTime > t)
        {
            currentFrame++;
            lastFrameTime = now;
            if (currentFrame == frames.size()) currentFrame = 0;
        }
    }

    LPSPRITE sprite = frames[currentFrame]->GetSprite();
    if (sprite)
    {
        sprite->Draw(x, y);
    }
    else
    {
        DebugOut(L"[ERROR] Sprite in animation frame %d is null!\n", currentFrame);
    }
}