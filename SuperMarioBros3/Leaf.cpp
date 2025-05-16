#include "Leaf.h"
#include "Mario.h"
#include "RacoonMario.h"
#include "Collision.h"
#include "PlayScene.h"
#include <cmath>

CLeaf::CLeaf(float x, float y) : CGameObject(x, y)
{
    vy = LEAF_BOUNCE_SPEED;
    ay = 0.00002f;
    vx = 0.0f;
    spawnTime = GetTickCount64();
    SetState(LEAF_STATE_ACTIVE);
}

void CLeaf::Render()
{
    if (isDeleted) return;
    CAnimations* animations = CAnimations::GetInstance();
    animations->Get(ID_ANI_LEAF)->Render(x, y);
}

void CLeaf::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    if (isDeleted) return;

    ULONGLONG now = GetTickCount64();
    if (now - spawnTime <= LEAF_BOUNCE_DURATION)
    {
        vy += ay * dt;
        vx = 0.0f;
    }
    else
    {
        vy = LEAF_FALL_SPEED;
        float t = (float)(now - spawnTime - LEAF_BOUNCE_DURATION);
        vx = LEAF_HORIZONTAL_SPEED * sin(LEAF_WAVE_FREQUENCY * t);
    }

    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CLeaf::GetBoundingBox(float& l, float& t, float& r, float& b)
{
    l = x - LEAF_BBOX_WIDTH / 2;
    t = y - LEAF_BBOX_HEIGHT / 2;
    r = l + LEAF_BBOX_WIDTH;
    b = t + LEAF_BBOX_HEIGHT;
}

void CLeaf::OnNoCollision(DWORD dt)
{
    x += vx * dt;
    y += vy * dt;
}

void CLeaf::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (dynamic_cast<CMario*>(e->obj))
    {
        CMario* mario = dynamic_cast<CMario*>(e->obj);
        if (mario->GetLevel() == MARIO_LEVEL_BIG)
        {
            CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
            float x, y;
			mario->GetPosition(x, y);
            CRaccoonMario* raccoonMario = new CRaccoonMario(x, y);

            raccoonMario->SetState(mario->GetState());
            raccoonMario->SetSpeed(mario->GetVx(), mario->GetVy());
			float nx = mario->GetDirection();
            raccoonMario->SetNx(nx);
            raccoonMario->StartAppearing();


            scene->AddObject(raccoonMario);

            scene->SetPlayer(raccoonMario);

            mario->Delete();
        }
        isDeleted = true;
    }
}

bool CLeaf::IsCollidableWith(LPGAMEOBJECT obj)
{
    return dynamic_cast<CMario*>(obj) != nullptr;
}

void CLeaf::SetState(int state)
{
    CGameObject::SetState(state);
}