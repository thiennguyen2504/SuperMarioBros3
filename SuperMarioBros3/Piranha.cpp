#include "Piranha.h"
#include "Animations.h"
#include "Mario.h"
#include "RacoonMario.h"
#include "PlayScene.h"
#include "debug.h"

Piranha::Piranha(float x, float y) : Enemy(x, y)
{
    this->waitTimer = 0;
    this->cycleTimer = 0;
    this->baseY = y;
    this->maxEmergeY = y - PIRANHA_EMERGE_HEIGHT;
    this->vx = 0;
    this->vy = 0;
    this->ay = 0;
    SetState(PIRANHA_STATE_HIDDEN);
}

void Piranha::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - PIRANHA_BBOX_WIDTH / 2;
    top = y - PIRANHA_BBOX_HEIGHT / 2;
    right = left + PIRANHA_BBOX_WIDTH;
    bottom = top + PIRANHA_BBOX_HEIGHT;
}

void Piranha::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
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
        isMarioNear = (distance < PIRANHA_ATTACK_RANGE);
    }

    cycleTimer += dt;

    if (state == PIRANHA_STATE_HIDDEN)
    {
        if (cycleTimer >= PIRANHA_HIDDEN_TIME && !isMarioOnPipe && isMarioNear)
        {
            SetState(PIRANHA_STATE_EMERGING);
        }
    }
    else if (state == PIRANHA_STATE_EMERGING)
    {
        vy = -PIRANHA_EMERGE_SPEED;
        y += vy * dt;

        if (y <= maxEmergeY)
        {
            y = maxEmergeY;
            SetState(PIRANHA_STATE_WAITING);
        }
    }
    else if (state == PIRANHA_STATE_WAITING)
    {
        waitTimer += dt;

        if (waitTimer >= PIRANHA_WAIT_TIME)
        {
            SetState(PIRANHA_STATE_RETREATING);
        }
    }
    else if (state == PIRANHA_STATE_RETREATING)
    {
        vy = PIRANHA_RETREAT_SPEED;
        y += vy * dt;

        if (y >= baseY)
        {
            y = baseY;
            SetState(PIRANHA_STATE_HIDDEN);
        }
    }

    float originalVy = vy;
    Enemy::Update(dt, coObjects);
    vy = originalVy;

    DebugOut(L"[DEBUG] Piranha at (%f, %f): state=%d, vy=%f, isMarioNear=%d\n", x, y, state, vy, isMarioNear);
}

void Piranha::Render()
{
    if (IsDeleted()) return;

    CAnimations* animations = CAnimations::GetInstance();
    animations->Get(ID_ANI_PIRANHA)->Render(x, y);
}

void Piranha::SetState(int state)
{
    Enemy::SetState(state);
    this->state = state;

    switch (state)
    {
    case PIRANHA_STATE_HIDDEN:
        cycleTimer = 0;
        vy = 0;
        break;
    case PIRANHA_STATE_EMERGING:
        waitTimer = 0;
        vy = -PIRANHA_EMERGE_SPEED;
        break;
    case PIRANHA_STATE_WAITING:
        waitTimer = 0;
        vy = 0;
        break;
    case PIRANHA_STATE_RETREATING:
        waitTimer = 0;
        vy = PIRANHA_RETREAT_SPEED;
        break;
    }
}