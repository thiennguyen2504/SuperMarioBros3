#include "VenusFire.h"
#include "Sprites.h"
#include "Mario.h"
#include "Fireball.h"
#include "PlayScene.h"
#include "RacoonMario.h"

VenusFire::VenusFire(float x, float y, int color,
    int spriteIdLeftDown, int spriteIdLeftUp, int spriteIdRightDown, int spriteIdRightUp)
    : Enemy(x, y)
{
    this->color = color;
    this->spriteIdLeftDown = spriteIdLeftDown;
    this->spriteIdLeftUp = spriteIdLeftUp;
    this->spriteIdRightDown = spriteIdRightDown;
    this->spriteIdRightUp = spriteIdRightUp;
    this->moveTimer = 0;
    this->waitTimer = 0;
    this->cycleTimer = 0;
    this->baseY = y;
    this->targetY = y;
    this->maxEmergeY = y - VENUS_FIRE_EMERGE_HEIGHT;
    this->isFacingRight = true;
    this->isMarioAbove = false;
    this->hasFired = false;
    this->vx = 0;
    this->vy = 0;
    this->ay = 0;
    this->state = VENUS_FIRE_STATE_HIDDEN;
}

void VenusFire::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    CSprites* sprites = CSprites::GetInstance();
    LPSPRITE sprite = sprites->Get(spriteIdLeftUp);
    if (sprite)
    {
        left = x - (sprite->GetRight() - sprite->GetLeft() + 1) / 2;
        top = y - (sprite->GetBottom() - sprite->GetTop() + 1) / 2;
        right = left + (sprite->GetRight() - sprite->GetLeft() + 1);
        bottom = top + (sprite->GetBottom() - sprite->GetTop() + 1);
    }
    else
    {
        left = x - 8;
        top = y - 16;
        right = x + 8;
        bottom = y + 16;
    }
}

void VenusFire::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    CMario* mario = nullptr;
    CRaccoonMario* raccoonMario = nullptr;
    for (size_t i = 0; i < coObjects->size(); i++)
    {
        LPGAMEOBJECT obj = coObjects->at(i);
        if (obj)
        {
            if (dynamic_cast<CMario*>(obj))
            {
                mario = dynamic_cast<CMario*>(obj);
                break;
            }
            else if (dynamic_cast<CRaccoonMario*>(obj))
            {
                raccoonMario = dynamic_cast<CRaccoonMario*>(obj);
                break;
            }
        }
    }

    bool isMarioNear = false;
    bool isMarioOnPipe = false;
    float marioX = 0, marioY = 0;
    if (mario || raccoonMario)
    {
        if (mario)
            mario->GetPosition(marioX, marioY);
        else
            raccoonMario->GetPosition(marioX, marioY);

        float dx = abs(x - marioX);
        float dy = abs(y - marioY);
        float distance = sqrt(dx * dx + dy * dy);
        isMarioNear = (distance < VENUS_FIRE_ATTACK_RANGE);

        isFacingRight = (marioX > x);
        isMarioAbove = (marioY < y);
    }

    cycleTimer += dt;

    if (state == VENUS_FIRE_STATE_HIDDEN)
    {
        if (cycleTimer >= VENUS_FIRE_HIDDEN_TIME && !isMarioOnPipe && isMarioNear)
        {
            SetState(VENUS_FIRE_STATE_EMERGING);
        }
    }
    else if (state == VENUS_FIRE_STATE_EMERGING)
    {
        vy = -VENUS_FIRE_EMERGE_SPEED;
        y += vy * dt;

        if (y <= maxEmergeY)
        {
            y = maxEmergeY;
            SetState(VENUS_FIRE_STATE_WAITING);
        }
    }
    else if (state == VENUS_FIRE_STATE_WAITING)
    {
        waitTimer += dt;

        if (waitTimer >= VENUS_FIRE_WAIT_TIME && !hasFired && (mario || raccoonMario))
        {
            Fireball* fireball = new Fireball(x, y, marioX, marioY);
            CPlayScene* playScene = dynamic_cast<CPlayScene*>(CGame::GetInstance()->GetCurrentScene());
            if (playScene)
            {
                playScene->AddObject(fireball);
            }
            hasFired = true;
        }

        if (waitTimer >= VENUS_FIRE_WAIT_TIME + VENUS_FIRE_RETREAT_DELAY)
        {
            SetState(VENUS_FIRE_STATE_RETREATING);
        }
    }
    else if (state == VENUS_FIRE_STATE_RETREATING)
    {
        vy = VENUS_FIRE_RETREAT_SPEED;
        y += vy * dt;

        if (y >= baseY)
        {
            y = baseY;
            SetState(VENUS_FIRE_STATE_HIDDEN);
        }
    }

    float originalVy = vy;
    Enemy::Update(dt, coObjects);
    vy = originalVy;
}

void VenusFire::Render()
{
    CSprites* sprites = CSprites::GetInstance();

    int spriteId;
    if (state == VENUS_FIRE_STATE_EMERGING)
    {
        spriteId = isFacingRight ? spriteIdRightUp : spriteIdLeftUp;
    }
    else if (state == VENUS_FIRE_STATE_RETREATING || state == VENUS_FIRE_STATE_HIDDEN)
    {
        spriteId = isFacingRight ? spriteIdRightDown : spriteIdLeftDown;
    }
    else
    {
        if (isMarioAbove)
        {
            spriteId = isFacingRight ? spriteIdRightUp : spriteIdLeftUp;
        }
        else
        {
            spriteId = isFacingRight ? spriteIdRightDown : spriteIdLeftDown;
        }
    }

    sprites->Get(spriteId)->Draw(x, y);
}

void VenusFire::SetState(int state)
{
    Enemy::SetState(state);
    this->state = state;

    switch (state)
    {
    case VENUS_FIRE_STATE_HIDDEN:
        cycleTimer = 0;
        vy = 0;
        break;

    case VENUS_FIRE_STATE_EMERGING:
        moveTimer = 0;
        vy = -VENUS_FIRE_EMERGE_SPEED;
        break;

    case VENUS_FIRE_STATE_WAITING:
        waitTimer = 0;
        hasFired = false;
        vy = 0;
        break;

    case VENUS_FIRE_STATE_RETREATING:
        moveTimer = 0;
        vy = VENUS_FIRE_RETREAT_SPEED;
        break;
    }
}