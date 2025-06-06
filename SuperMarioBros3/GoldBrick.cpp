#include "GoldBrick.h"
#include "Mario.h"
#include "RacoonMario.h"
#include "KoopaTroopa.h"
#include "PlayScene.h"
#include "Effect.h"
#include "PButton.h"
#include "QuestionBlock.h"
#include "debug.h"

#define GOLD_PIECE_SPEED 0.15f
#define GOLD_PIECE_GRAVITY 0.002f
#define GOLD_PIECE_LIFETIME 500

#define CENTER_THRESHOLD (GOLD_BRICK_BBOX_HEIGHT / 4.0f)

class GoldPiece : public CGameObject
{
protected:
    ULONGLONG spawnTime;
public:
    GoldPiece(float x, float y, float vx, float vy) : CGameObject(x, y)
    {
        this->vx = vx;
        this->vy = vy;
        spawnTime = GetTickCount64();
    }
    void GetBoundingBox(float& l, float& t, float& r, float& b)
    {
        l = x - 12 / 2;
        t = y - 14 / 2;
        r = l + 12;
        b = t + 14;
    }
    void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
    {
        vy += GOLD_PIECE_GRAVITY * dt;
        x += vx * dt;
        y += vy * dt;
        if (GetTickCount64() - spawnTime > GOLD_PIECE_LIFETIME)
            Delete();
    }
    void Render()
    {
        CAnimations::GetInstance()->Get(9998)->Render(x, y);
    }
    int IsCollidable() { return 0; }
    int IsBlocking() { return 0; }
};

GoldBrick::GoldBrick(float x, float y, int type) : CGameObject(x, y)
{
    this->type = type;
    SetState(GOLD_BRICK_STATE_ACTIVE);
}

void GoldBrick::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    left = x - GOLD_BRICK_BBOX_WIDTH / 2;
    top = y - GOLD_BRICK_BBOX_HEIGHT / 2;
    right = left + GOLD_BRICK_BBOX_WIDTH;
    bottom = top + GOLD_BRICK_BBOX_HEIGHT;
}

void GoldBrick::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    if (state == GOLD_BRICK_STATE_DESTROYED)
    {
        Delete();
        return;
    }
    CGameObject::Update(dt, coObjects);
    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void GoldBrick::Render()
{
    if (state == GOLD_BRICK_STATE_DESTROYED) return;
    CAnimations::GetInstance()->Get(ID_ANI_GOLD_BRICK)->Render(x, y);
}

int GoldBrick::IsBlocking(LPGAMEOBJECT obj)
{
    if (state != GOLD_BRICK_STATE_ACTIVE) return 0;

    if (dynamic_cast<KoopaTroopa*>(obj))
    {
        KoopaTroopa* koopa = dynamic_cast<KoopaTroopa*>(obj);
        if (koopa->GetState() == KOOPA_STATE_SHELL_RUNNING)
        {
            float koopaX, koopaY;
            koopa->GetPosition(koopaX, koopaY);
            if (abs(koopaY - y) > CENTER_THRESHOLD)
                return 0;
        }
    }
    return 1; 
}

void GoldBrick::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (state == GOLD_BRICK_STATE_DESTROYED) return;

    CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
    if (dynamic_cast<CMario*>(e->obj) || dynamic_cast<CRaccoonMario*>(e->obj))
    {
        if (e->ny < 0) 
        {
            SetState(GOLD_BRICK_STATE_DESTROYED);
            CEffect* effect = new CEffect(x, y, 100);
            scene->AddObject(effect);
            if (type == GOLD_BRICK_TYPE_NORMAL)
            {
                float angle45 = 3.14159f / 4;
                scene->AddObject(new GoldPiece(x, y, GOLD_PIECE_SPEED * cos(angle45), -GOLD_PIECE_SPEED * sin(angle45)));
                scene->AddObject(new GoldPiece(x, y, GOLD_PIECE_SPEED * cos(angle45 + 3.14159f / 2), -GOLD_PIECE_SPEED * sin(angle45 + 3.14159f / 2)));
                scene->AddObject(new GoldPiece(x, y, GOLD_PIECE_SPEED * cos(angle45 + 3.14159f), -GOLD_PIECE_SPEED * sin(angle45 + 3.14159f)));
                scene->AddObject(new GoldPiece(x, y, GOLD_PIECE_SPEED * cos(angle45 + 3 * 3.14159f / 2), -GOLD_PIECE_SPEED * sin(angle45 + 3 * 3.14159f / 2)));
            }
            else
            {
                CQuestionBlock* block = new CQuestionBlock(x, y, QUESTION_BLOCK_TYPE_COIN);
                block->SetState(QUESTION_BLOCK_STATE_INACTIVE);
                scene->AddObject(block);
                float topEdge = y - GOLD_BRICK_BBOX_HEIGHT / 2;
                PButton* button = new PButton(x, topEdge - P_BUTTON_BBOX_HEIGHT / 2);
                scene->AddObject(button);
            }
        }
    }
    else if (dynamic_cast<KoopaTroopa*>(e->obj))
    {
        KoopaTroopa* koopa = dynamic_cast<KoopaTroopa*>(e->obj);
        if (koopa->GetState() == KOOPA_STATE_SHELL_RUNNING && e->nx != 0 && koopa->CanBounce()) 
        {

            float koopaX, koopaY;
            koopa->GetPosition(koopaX, koopaY);
            if (abs(koopaY - y) <= CENTER_THRESHOLD)
            {
                SetState(GOLD_BRICK_STATE_DESTROYED);
                koopa->ReverseDirection(); 
                CEffect* effect = new CEffect(x, y, 100);
                scene->AddObject(effect);
                if (type == GOLD_BRICK_TYPE_NORMAL)
                {
                    float angle45 = 3.14159f / 4; 
                    scene->AddObject(new GoldPiece(x, y, GOLD_PIECE_SPEED * cos(angle45), -GOLD_PIECE_SPEED * sin(angle45)));
                    scene->AddObject(new GoldPiece(x, y, GOLD_PIECE_SPEED * cos(angle45 + 3.14159f / 2), -GOLD_PIECE_SPEED * sin(angle45 + 3.14159f / 2)));
                    scene->AddObject(new GoldPiece(x, y, GOLD_PIECE_SPEED * cos(angle45 + 3.14159f), -GOLD_PIECE_SPEED * sin(angle45 + 3.14159f)));
                    scene->AddObject(new GoldPiece(x, y, GOLD_PIECE_SPEED * cos(angle45 + 3 * 3.14159f / 2), -GOLD_PIECE_SPEED * sin(angle45 + 3 * 3.14159f / 2)));
                }
                else
                {
                    CQuestionBlock* block = new CQuestionBlock(x, y, QUESTION_BLOCK_TYPE_COIN);
                    block->SetState(QUESTION_BLOCK_STATE_INACTIVE);
                    scene->AddObject(block);
                    float topEdge = y - GOLD_BRICK_BBOX_HEIGHT / 2;
                    PButton* button = new PButton(x, topEdge - P_BUTTON_BBOX_HEIGHT / 2);
                    scene->AddObject(button);
                }
            }
        }
    }
}

void GoldBrick::SetState(int state)
{
    CGameObject::SetState(state);
}