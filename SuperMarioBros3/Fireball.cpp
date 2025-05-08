#include "Fireball.h"
#include "Sprites.h"
#include "Game.h"
#include "debug.h"

Fireball::Fireball(float x, float y, float targetX, float targetY) : CGameObject(x, y)
{
    // Calculate direction to target (Mario's position)
    float dx = targetX - x;
    float dy = targetY - y;
    float distance = sqrt(dx * dx + dy * dy);

    if (distance != 0)
    {
        // Normalize direction and apply speed
        this->vx = (dx / distance) * FIREBALL_SPEED;
        this->vy = (dy / distance) * FIREBALL_SPEED;
    }
    else
    {
        this->vx = FIREBALL_SPEED; // Default direction if Mario is at the same position
        this->vy = 0;
    }
}

void Fireball::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - FIREBALL_BBOX_WIDTH / 2;
    top = y - FIREBALL_BBOX_HEIGHT / 2;
    right = left + FIREBALL_BBOX_WIDTH;
    bottom = top + FIREBALL_BBOX_HEIGHT;
}

void Fireball::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    x += vx * dt;
    y += vy * dt;

    // Check if Fireball is out of camera bounds
    CGame* game = CGame::GetInstance();
    if (game)
    {
        float camX, camY;
        game->GetCamPos(camX, camY);
        float camWidth = game->GetBackBufferWidth();
        float camHeight = game->GetBackBufferHeight();
        if (x < camX || x > camX + camWidth || y < camY || y > camY + camHeight)
        {
            isDeleted = true;
            return;
        }
    }
    else
    {
        DebugOut(L"[ERROR] CGame instance is null in Fireball::Update\n");
        isDeleted = true;
        return;
    }

    // Fireball passes through walls and enemies, only collides with Mario
    CGameObject::Update(dt, coObjects);
    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void Fireball::Render()
{
    CAnimations* animations = CAnimations::GetInstance();
    int aniId = ID_ANI_FIREBALL;
	animations->Get(aniId)->Render(x, y);

    //RenderBoundingBox();
}