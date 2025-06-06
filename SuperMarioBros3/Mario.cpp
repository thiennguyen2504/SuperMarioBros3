#include <algorithm>
#include "Mario.h"
#include "Game.h"
#include "Goomba.h"
#include "Coin.h"
#include "Portal.h"
#include "VenusFire.h"
#include "Fireball.h"
#include "KoopaTroopa.h"
#include "RedParaGoomba.h"
#include "GreenParaKoopa.h"
#include "Piranha.h"
#include "PButton.h"
#include "StaticCoin.h"
#include "Mushroom.h"
#include "Leaf.h"
#include "RacoonMario.h"
#include "Collision.h"
#include "PlayScene.h"
#include "GreenKoopaTroopa.h"
#include "HUD.h"

void CMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
    {
        untouchable_start = 0;
        untouchable = 0;
    }

    // Luôn cập nhật vận tốc khi isHolding, bỏ qua isBlinking/isAppearing
    if (!isLocked) // Chỉ khóa khi thực sự cần (ví dụ: chuyển cảnh)
    {
        vy += ay * dt;

        // Update runProgress
        if (isRunning && (state == MARIO_STATE_RUNNING_RIGHT || state == MARIO_STATE_RUNNING_LEFT))
        {
            runProgress += MARIO_RUN_PROGRESS_ACCEL * dt;
            if (runProgress > 1.0f) runProgress = 1.0f;
        }
        else
        {
            runProgress -= MARIO_RUN_PROGRESS_DECAY * dt;
            if (runProgress < 0.0f) runProgress = 0.0f;
        }

        // Update maxVx based on runProgress
        float speedRange = MARIO_RUNNING_SPEED - MARIO_WALKING_SPEED;
        maxVx = (nx > 0) ?
            MARIO_WALKING_SPEED + speedRange * runProgress :
            -(MARIO_WALKING_SPEED + speedRange * runProgress);

        // Apply acceleration or friction
        LPGAME game = CGame::GetInstance();
        if (game->IsKeyDown(DIK_L) || game->IsKeyDown(DIK_J))
        {
            vx += ax * dt;
            if (abs(vx) > abs(maxVx)) vx = maxVx;
        }
        else if (isOnPlatform)
        {
            if (vx > 0)
            {
                vx -= MARIO_FRICTION * dt;
                if (vx < 0) vx = 0;
            }
            else if (vx < 0)
            {
                vx += MARIO_FRICTION * dt;
                if (vx > 0) vx = 0;
            }
            ax = 0.0f;
        }
    }

    if (isHolding && heldKoopa)
    {
        float marioLeft, marioTop, marioRight, marioBottom;
        GetBoundingBox(marioLeft, marioTop, marioRight, marioBottom);

        float koopaLeft, koopaTop, koopaRight, koopaBottom;
        heldKoopa->GetBoundingBox(koopaLeft, koopaTop, koopaRight, koopaBottom);

        float koopaX, koopaY;
        if (nx > 0)
        {
            koopaX = marioRight + (koopaRight - koopaLeft) / 2;
        }
        else
        {
            koopaX = marioLeft - (koopaRight - koopaLeft) / 2;
        }
        koopaY = y;

        heldKoopa->SetPosition(koopaX, koopaY);
    }

    CCollision::GetInstance()->Process(this, dt, coObjects);
}

void CMario::OnNoCollision(DWORD dt)
{
    if (!isAppearing && !isBlinking)
    {
        x += vx * dt;
        y += vy * dt;
    }
    isOnPlatform = false;
}

void CMario::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (e->ny != 0 && e->obj->IsBlocking())
    {
        vy = 0;
        if (e->ny < 0) isOnPlatform = true;
    }
    else if (e->nx != 0 && e->obj->IsBlocking())
    {
        if (isOnPlatform) vx = 0;
    }

    if (dynamic_cast<CGoomba*>(e->obj))
        OnCollisionWithGoomba(e);
    else if (dynamic_cast<CCoin*>(e->obj))
        OnCollisionWithCoin(e);
    else if (dynamic_cast<CPortal*>(e->obj))
        OnCollisionWithPortal(e);
    else if (dynamic_cast<VenusFire*>(e->obj))
        OnCollisionWithVenusFire(e);
    else if (dynamic_cast<Fireball*>(e->obj))
        OnCollisionWithFireball(e);
    else if (dynamic_cast<KoopaTroopa*>(e->obj))
        OnCollisionWithKoopaTroopa(e);
    else if (dynamic_cast<RedParaGoomba*>(e->obj))
        OnCollisionWithRedParaGoomba(e);
    else if (dynamic_cast<GreenParaKoopa*>(e->obj))
        OnCollisionWithGreenParaKoopa(e);
    else if (dynamic_cast<Piranha*>(e->obj))
        OnCollisionWithPiranha(e);
    else if (dynamic_cast<PButton*>(e->obj))
        OnCollisionWithPButton(e);
    else if (dynamic_cast<StaticCoin*>(e->obj))
        OnCollisionWithStaticCoin(e);
    else if (dynamic_cast<CMushroom*>(e->obj))
        OnCollisionWithMushroom(e);
    else if (dynamic_cast<CLeaf*>(e->obj))
        OnCollisionWithLeaf(e);
}

void CMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
    CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

    if (e->ny < 0)
    {
        if (goomba->GetState() != ENEMY_STATE_DIE)
        {
            goomba->SetState(ENEMY_STATE_DIE);
            vy = -MARIO_JUMP_DEFLECT_SPEED;
        }
    }
    else
    {
        if (untouchable == 0)
        {
            if (goomba->GetState() != ENEMY_STATE_DIE)
            {
                if (level > MARIO_LEVEL_SMALL)
                {
                    level = MARIO_LEVEL_SMALL;
                    StartUntouchable();
                }
                else
                {
                    SetState(MARIO_STATE_DIE);
                }
            }
        }
    }
}

void CMario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
    e->obj->Delete();
    CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
    CHUD* hud = scene->GetHUD();
    if (hud) hud->AddCoin();
}

void CMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
    CPortal* p = (CPortal*)e->obj;
    CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}

void CMario::OnCollisionWithVenusFire(LPCOLLISIONEVENT e)
{
    VenusFire* venusFire = dynamic_cast<VenusFire*>(e->obj);

    if (untouchable == 0 && venusFire->GetState() != VENUS_FIRE_STATE_HIDDEN)
    {
        if (level > MARIO_LEVEL_SMALL)
        {
            level = MARIO_LEVEL_SMALL;
            StartUntouchable();
        }
        else
        {
            SetState(MARIO_STATE_DIE);
        }
    }
}

void CMario::OnCollisionWithFireball(LPCOLLISIONEVENT e)
{
    Fireball* fireball = dynamic_cast<Fireball*>(e->obj);
    fireball->Delete();

    if (untouchable == 0)
    {
        if (level > MARIO_LEVEL_SMALL)
        {
            level = MARIO_LEVEL_SMALL;
            StartUntouchable();
        }
        else
        {
            SetState(MARIO_STATE_DIE);
        }
    }
}

void CMario::OnCollisionWithKoopaTroopa(LPCOLLISIONEVENT e)
{
    KoopaTroopa* koopa = dynamic_cast<KoopaTroopa*>(e->obj);

    if (e->ny < 0)
    {
        if (koopa->GetState() == KOOPA_STATE_WALKING)
        {
            koopa->SetState(KOOPA_STATE_SHELL_IDLE);
            vy = -MARIO_JUMP_DEFLECT_SPEED;
        }
        else if (koopa->GetState() == KOOPA_STATE_SHELL_RUNNING)
        {
            koopa->SetState(KOOPA_STATE_SHELL_IDLE);
            vy = -MARIO_JUMP_DEFLECT_SPEED;
        }
        else if (koopa->GetState() == KOOPA_STATE_SHELL_IDLE && !koopa->IsIdleCooldownActive())
        {
            if (IsKeyDown(DIK_A))
            {
                if (!isHolding)
                {
                    koopa->SetState(KOOPA_STATE_CARRIED);
                    SetHeldKoopa(koopa);
                }
            }
            else
            {
                float koopaX, koopaY;
                koopa->GetPosition(koopaX, koopaY);
                koopa->SetDirection(x > koopaX ? -1 : 1);
                koopa->SetState(KOOPA_STATE_SHELL_RUNNING);
                vy = -MARIO_JUMP_DEFLECT_SPEED;
            }
        }
    }
    else
    {
        if (koopa->GetState() == KOOPA_STATE_WALKING ||
            (koopa->GetState() == KOOPA_STATE_SHELL_RUNNING && !koopa->IsKickCooldownActive()))
        {
            if (untouchable == 0)
            {
                if (level > MARIO_LEVEL_SMALL)
                {
                    level = MARIO_LEVEL_SMALL;
                    StartUntouchable();
                }
                else
                {
                    SetState(MARIO_STATE_DIE);
                }
            }
        }
        else if (koopa->GetState() == KOOPA_STATE_SHELL_IDLE && !koopa->IsIdleCooldownActive())
        {
            if (IsKeyDown(DIK_A))
            {
                if (!isHolding)
                {
                    koopa->SetState(KOOPA_STATE_CARRIED);
                    SetHeldKoopa(koopa);
                }
            }
            else
            {
                float koopaX, koopaY;
                koopa->GetPosition(koopaX, koopaY);
                koopa->SetDirection(x > koopaX ? -1 : 1);
                koopa->SetState(KOOPA_STATE_SHELL_RUNNING);
            }
        }
    }
}

void CMario::OnCollisionWithRedParaGoomba(LPCOLLISIONEVENT e)
{
    RedParaGoomba* paraGoomba = dynamic_cast<RedParaGoomba*>(e->obj);

    if (e->ny < 0)
    {
        CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
        float gx, gy;
        paraGoomba->GetPosition(gx, gy);
        CGoomba* goomba = new CGoomba(gx, gy - RED_PARAGOOMBA_BBOX_HEIGHT / 2, GOOMBA_TYPE_RED);
        goomba->SetState(ENEMY_STATE_WALKING);
        scene->AddObject(goomba);
        paraGoomba->Delete();
        vy = -MARIO_JUMP_DEFLECT_SPEED;
    }
    else
    {
        if (untouchable == 0 && paraGoomba->GetState() != RED_PARAGOOMBA_STATE_DIE)
        {
            if (level > MARIO_LEVEL_SMALL)
            {
                level = MARIO_LEVEL_SMALL;
                StartUntouchable();
            }
            else
            {
                SetState(MARIO_STATE_DIE);
            }
        }
    }
}

void CMario::OnCollisionWithGreenParaKoopa(LPCOLLISIONEVENT e)
{
    GreenParaKoopa* paraKoopa = dynamic_cast<GreenParaKoopa*>(e->obj);

    if (e->ny < 0)
    {
        if (paraKoopa->GetState() != GREEN_PARAKOOPA_STATE_DIE)
        {
            CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
            float kx, ky;
            paraKoopa->GetPosition(kx, ky);
            GreenKoopaTroopa* koopa = new GreenKoopaTroopa(kx, ky - GREEN_PARAKOOPA_BBOX_HEIGHT / 2);
            koopa->SetState(KOOPA_STATE_WALKING);
            scene->AddObject(koopa);
            paraKoopa->Delete();
            vy = -MARIO_JUMP_DEFLECT_SPEED;
        }
    }
    else
    {
        if (untouchable == 0 && paraKoopa->GetState() != GREEN_PARAKOOPA_STATE_DIE)
        {
            if (level > MARIO_LEVEL_SMALL)
            {
                level = MARIO_LEVEL_SMALL;
                StartUntouchable();
            }
            else
            {
                SetState(MARIO_STATE_DIE);
            }
        }
    }
}

void CMario::OnCollisionWithPiranha(LPCOLLISIONEVENT e)
{
    Piranha* piranha = dynamic_cast<Piranha*>(e->obj);

    if (untouchable == 0 && piranha->GetState() != PIRANHA_STATE_HIDDEN)
    {
        if (level > MARIO_LEVEL_SMALL)
        {
            level = MARIO_LEVEL_SMALL;
            StartUntouchable();
        }
        else
        {
            SetState(MARIO_STATE_DIE);
        }
    }
}

void CMario::OnCollisionWithPButton(LPCOLLISIONEVENT e)
{
    PButton* button = dynamic_cast<PButton*>(e->obj);
    button->SetState(P_BUTTON_STATE_PRESSED);
}

void CMario::OnCollisionWithStaticCoin(LPCOLLISIONEVENT e)
{
    e->obj->Delete();
    CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
    CHUD* hud = scene->GetHUD();
    if (hud) hud->AddCoin();
}

void CMario::OnCollisionWithMushroom(LPCOLLISIONEVENT e)
{
    CMushroom* mushroom = dynamic_cast<CMushroom*>(e->obj);
    if (level == MARIO_LEVEL_SMALL)
    {
        SetLevel(MARIO_LEVEL_BIG);
    }
    mushroom->Delete();
}

void CMario::OnCollisionWithLeaf(LPCOLLISIONEVENT e)
{
    CLeaf* leaf = dynamic_cast<CLeaf*>(e->obj);
    if (level == MARIO_LEVEL_BIG)
    {
        CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
        CRaccoonMario* raccoonMario = new CRaccoonMario(x, y);
        raccoonMario->SetState(state);
        raccoonMario->SetSpeed(vx, vy);
        raccoonMario->SetNx(nx);
        raccoonMario->StartAppearing();
        scene->AddObject(raccoonMario);
        scene->SetPlayer(raccoonMario);
        Delete();
    }
    leaf->Delete();
}

void CMario::OnHitByKoopa()
{
    if (untouchable == 0)
    {
        if (level > MARIO_LEVEL_SMALL)
        {
            level = MARIO_LEVEL_SMALL;
            StartUntouchable();
        }
        else
        {
            SetState(MARIO_STATE_DIE);
        }
    }
}

int CMario::GetAniIdSmall()
{
    int aniId = -1;

    if (isHolding)
    {
        if (vx == 0)
        {
            if (nx > 0)
                aniId = ID_ANI_MARIO_SMALL_HOLD_IDLE_RIGHT;
            else
                aniId = ID_ANI_MARIO_SMALL_HOLD_IDLE_LEFT;
        }
        else if (abs(ax) == MARIO_ACCEL_RUN_X)
        {
            if (nx >= 0)
                aniId = ID_ANI_MARIO_SMALL_HOLD_RUN_RIGHT;
            else
                aniId = ID_ANI_MARIO_SMALL_HOLD_RUN_LEFT;
        }
        else
        {
            if (nx > 0)
                aniId = ID_ANI_MARIO_SMALL_HOLD_IDLE_RIGHT;
            else
                aniId = ID_ANI_MARIO_SMALL_HOLD_IDLE_LEFT;
        }
    }
    else
    {
        if (!isOnPlatform)
        {
            if (abs(ax) == MARIO_ACCEL_RUN_X)
            {
                if (nx >= 0)
                    aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_RIGHT;
                else
                    aniId = ID_ANI_MARIO_SMALL_JUMP_RUN_LEFT;
            }
            else
            {
                if (nx >= 0)
                    aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_RIGHT;
                else
                    aniId = ID_ANI_MARIO_SMALL_JUMP_WALK_LEFT;
            }
        }
        else if (isSitting)
        {
            if (nx > 0)
                aniId = ID_ANI_MARIO_SIT_RIGHT;
            else
                aniId = ID_ANI_MARIO_SIT_LEFT;
        }
        else if (vx == 0)
        {
            if (nx > 0) aniId = ID_ANI_MARIO_SMALL_IDLE_RIGHT;
            else aniId = ID_ANI_MARIO_SMALL_IDLE_LEFT;
        }
        else if (vx > 0)
        {
            if (ax < 0)
                aniId = ID_ANI_MARIO_SMALL_BRACE_RIGHT;
            else if (abs(vx) >= MARIO_RUNNING_SPEED * 0.8f)
                aniId = ID_ANI_MARIO_SMALL_RUNNING_RIGHT;
            else
                aniId = ID_ANI_MARIO_SMALL_WALKING_RIGHT;
        }
        else
        {
            if (ax > 0)
                aniId = ID_ANI_MARIO_SMALL_BRACE_LEFT;
            else if (abs(vx) >= MARIO_RUNNING_SPEED * 0.8f)
                aniId = ID_ANI_MARIO_SMALL_RUNNING_LEFT;
            else
                aniId = ID_ANI_MARIO_SMALL_WALKING_LEFT;
        }
    }

    if (aniId == -1) aniId = nx > 0 ? ID_ANI_MARIO_SMALL_IDLE_RIGHT : ID_ANI_MARIO_SMALL_IDLE_LEFT;

    return aniId;
}

int CMario::GetAniIdBig()
{
    int aniId = -1;

    if (isHolding)
    {
        if (vx == 0)
        {
            if (nx > 0)
                aniId = ID_ANI_MARIO_BIG_HOLD_IDLE_RIGHT;
            else
                aniId = ID_ANI_MARIO_BIG_HOLD_IDLE_LEFT;
        }
        else if (abs(ax) == MARIO_ACCEL_RUN_X)
        {
            if (nx >= 0)
                aniId = ID_ANI_MARIO_BIG_HOLD_RUN_RIGHT;
            else
                aniId = ID_ANI_MARIO_BIG_HOLD_RUN_LEFT;
        }
        else
        {
            if (nx > 0)
                aniId = ID_ANI_MARIO_BIG_HOLD_IDLE_RIGHT;
            else
                aniId = ID_ANI_MARIO_BIG_HOLD_IDLE_LEFT;
        }
    }
    else
    {
        if (!isOnPlatform)
        {
            if (abs(ax) == MARIO_ACCEL_RUN_X)
            {
                if (nx >= 0)
                    aniId = ID_ANI_MARIO_JUMP_RUN_RIGHT;
                else
                    aniId = ID_ANI_MARIO_JUMP_RUN_LEFT;
            }
            else
            {
                if (nx >= 0)
                    aniId = ID_ANI_MARIO_JUMP_WALK_RIGHT;
                else
                    aniId = ID_ANI_MARIO_JUMP_WALK_LEFT;
            }
        }
        else if (isSitting)
        {
            if (nx > 0)
                aniId = ID_ANI_MARIO_SIT_RIGHT;
            else
                aniId = ID_ANI_MARIO_SIT_LEFT;
        }
        else if (vx == 0)
        {
            if (nx > 0) aniId = ID_ANI_MARIO_IDLE_RIGHT;
            else aniId = ID_ANI_MARIO_IDLE_LEFT;
        }
        else if (vx > 0)
        {
            if (ax < 0)
                aniId = ID_ANI_MARIO_BRACE_RIGHT;
            else if (abs(vx) >= MARIO_RUNNING_SPEED * 0.8f)
                aniId = ID_ANI_MARIO_RUNNING_RIGHT;
            else
                aniId = ID_ANI_MARIO_WALKING_RIGHT;
        }
        else
        {
            if (ax > 0)
                aniId = ID_ANI_MARIO_BRACE_LEFT;
            else if (abs(vx) >= MARIO_RUNNING_SPEED * 0.8f)
                aniId = ID_ANI_MARIO_RUNNING_LEFT;
            else
                aniId = ID_ANI_MARIO_WALKING_LEFT;
        }
    }

    if (aniId == -1) aniId = nx > 0 ? ID_ANI_MARIO_IDLE_RIGHT : ID_ANI_MARIO_IDLE_LEFT;

    return aniId;
}

void CMario::Render()
{
    if (isDeleted) return;

    CAnimations* animations = CAnimations::GetInstance();
    int aniId = -1;

    bool shouldRender = true;
    if (untouchable == 1 && untouchable_start != -1)
    {
        ULONGLONG timeSinceUntouchable = GetTickCount64() - untouchable_start;
        ULONGLONG blinkPhase = timeSinceUntouchable % (2 * MARIO_UNTOUCHABLE_BLINK_INTERVAL);
        shouldRender = (blinkPhase < MARIO_UNTOUCHABLE_BLINK_INTERVAL);
    }

    if (state == MARIO_STATE_DIE)
    {
        aniId = ID_ANI_MARIO_DIE;
    }
    else if (isBlinking && blinkStart != 0)
    {
        ULONGLONG timeSinceBlink = GetTickCount64() - blinkStart;
        if (timeSinceBlink < MARIO_BLINK_TIME)
        {
            ULONGLONG blinkPhase = timeSinceBlink % (2 * MARIO_BLINK_INTERVAL);
            if (blinkPhase < MARIO_BLINK_INTERVAL)
            {
                aniId = (nx > 0) ? ID_ANI_MARIO_SMALL_IDLE_RIGHT : ID_ANI_MARIO_SMALL_IDLE_LEFT;
            }
            else
            {
                aniId = (nx > 0) ? ID_ANI_MARIO_IDLE_RIGHT : ID_ANI_MARIO_IDLE_LEFT;
            }
        }
        else
        {
            isBlinking = false;
            isLocked = false;
            blinkStart = 0;
            aniId = (level == MARIO_LEVEL_BIG) ? GetAniIdBig() : GetAniIdSmall();
        }
    }
    else
    {
        aniId = (level == MARIO_LEVEL_BIG) ? GetAniIdBig() : GetAniIdSmall();
    }

    if (shouldRender && aniId != -1)
    {
        LPANIMATION animation = animations->Get(aniId);
        if (animation != nullptr)
        {
            animation->Render(x, y);
        }
    }
}

void CMario::SetState(int state)
{
    if (this->state == MARIO_STATE_DIE || isLocked) return;

    int prevNx = nx;
    LPGAME game = CGame::GetInstance();

    switch (state)
    {
    case MARIO_STATE_RUNNING_RIGHT:
        if (isSitting) break;
        ax = MARIO_ACCEL_RUN_X;
        nx = 1;
        isRunning = true;
        break;
    case MARIO_STATE_RUNNING_LEFT:
        if (isSitting) break;
        ax = -MARIO_ACCEL_RUN_X;
        nx = -1;
        isRunning = true;
        break;
    case MARIO_STATE_WALKING_RIGHT:
        if (isSitting) break;
        ax = MARIO_ACCEL_WALK_X;
        nx = 1;
        isRunning = false;
        if (isHolding && heldKoopa && !IsKeyDown(DIK_A))
        {
            heldKoopa->SetState(KOOPA_STATE_SHELL_RUNNING);
            heldKoopa->SetDirection(nx);
            SetHeldKoopa(nullptr);
        }
        break;
    case MARIO_STATE_WALKING_LEFT:
        if (isSitting) break;
        ax = -MARIO_ACCEL_WALK_X;
        nx = -1;
        isRunning = false;
        if (isHolding && heldKoopa && !IsKeyDown(DIK_A))
        {
            heldKoopa->SetState(KOOPA_STATE_SHELL_RUNNING);
            heldKoopa->SetDirection(nx);
            SetHeldKoopa(nullptr);
        }
        break;
    case MARIO_STATE_JUMP:
        if (isSitting) break;
        if (isOnPlatform)
        {
            if (abs(this->vx) >= MARIO_RUNNING_SPEED * 0.8f)
                vy = -MARIO_JUMP_RUN_SPEED_Y;
            else
                vy = -MARIO_JUMP_SPEED_Y;
            if (game->IsKeyDown(DIK_L))
            {
                ax = isRunning ? MARIO_ACCEL_RUN_X : MARIO_ACCEL_WALK_X;
                nx = 1;
            }
            else if (game->IsKeyDown(DIK_J))
            {
                ax = isRunning ? -MARIO_ACCEL_RUN_X : -MARIO_ACCEL_WALK_X;
                nx = -1;
            }
            else
            {
                ax = 0.0f;
                nx = prevNx;
            }
        }
        break;
    case MARIO_STATE_RELEASE_JUMP:
        if (vy < 0) vy += MARIO_JUMP_SPEED_Y / 9.4f;
        break;
    case MARIO_STATE_SIT:
        if (isOnPlatform && level != MARIO_LEVEL_SMALL)
        {
            state = MARIO_STATE_IDLE;
            isSitting = true;
            vx = 0; vy = 0.0f;
            y += MARIO_SIT_HEIGHT_ADJUST;
        }
        break;
    case MARIO_STATE_SIT_RELEASE:
        if (isSitting)
        {
            isSitting = false;
            state = MARIO_STATE_IDLE;
            y -= MARIO_SIT_HEIGHT_ADJUST;
        }
        break;
    case MARIO_STATE_TAIL_ATTACK:
        if (dynamic_cast<CRaccoonMario*>(this))
        {
            CRaccoonMario* raccoonMario = dynamic_cast<CRaccoonMario*>(this);
            raccoonMario->StartTailAttack();
            ax = 0.0f;
            vx = 0.0f;
        }
        break;
    case MARIO_STATE_IDLE:
        ax = 0.0f;
        isRunning = false;
        if (isHolding && heldKoopa && !IsKeyDown(DIK_A))
        {
            heldKoopa->SetState(KOOPA_STATE_SHELL_RUNNING);
            heldKoopa->SetDirection(nx);
            SetHeldKoopa(nullptr);
        }
        break;
    case MARIO_STATE_DIE:
        vy = -MARIO_JUMP_DEFLECT_SPEED;
        vx = 0;
        ax = 0;
        if (isHolding && heldKoopa)
        {
            heldKoopa->SetState(KOOPA_STATE_SHELL_RUNNING);
            heldKoopa->SetDirection(nx);
            SetHeldKoopa(nullptr);
        }
        break;
    }

    CGameObject::SetState(state);
}

void CMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    float bottomY;
    if (isOnPlatform)
    {
        bottomY = y + (level == MARIO_LEVEL_SMALL ? MARIO_SMALL_BBOX_HEIGHT : MARIO_BIG_BBOX_HEIGHT) / 2;
    }
    else
    {
        bottomY = y + (level == MARIO_LEVEL_SMALL ? MARIO_SMALL_BBOX_HEIGHT : MARIO_BIG_BBOX_HEIGHT) / 2;
    }

    if (level == MARIO_LEVEL_BIG || level == MARIO_LEVEL_RACCOON)
    {
        if (isSitting)
        {
            left = x - MARIO_BIG_SITTING_BBOX_WIDTH / 2;
            right = left + MARIO_BIG_SITTING_BBOX_WIDTH;
            bottom = bottomY;
            top = bottom - MARIO_BIG_SITTING_BBOX_HEIGHT;
        }
        else
        {
            left = x - MARIO_BIG_BBOX_WIDTH / 2;
            right = left + MARIO_BIG_BBOX_WIDTH;
            bottom = bottomY;
            top = bottom - MARIO_BIG_BBOX_HEIGHT;
        }
    }
    else
    {
        left = x - MARIO_SMALL_BBOX_WIDTH / 2;
        right = left + MARIO_SMALL_BBOX_WIDTH;
        bottom = bottomY;
        top = bottom - MARIO_SMALL_BBOX_HEIGHT;
    }
}

void CMario::SetLevel(int l)
{
    if (l != level)
    {
        isBlinking = true;
        isLocked = true;
        blinkStart = GetTickCount64();
        originalX = x;
        originalY = y;

        if (level == MARIO_LEVEL_SMALL && l == MARIO_LEVEL_BIG && isOnPlatform)
        {
            y -= 8.0f;
        }

        vx = 0.0f;
        ax = 0.0f;
        SetState(MARIO_STATE_IDLE);
    }

    if (isHolding && heldKoopa)
    {
        heldKoopa->SetState(KOOPA_STATE_SHELL_RUNNING);
        heldKoopa->SetDirection(nx);
        SetHeldKoopa(nullptr);
    }
    level = l;
}