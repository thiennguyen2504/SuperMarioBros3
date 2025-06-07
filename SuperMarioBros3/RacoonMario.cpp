#include "RacoonMario.h"
#include "Goomba.h"
#include "Coin.h"
#include "VenusFire.h"
#include "Fireball.h"
#include "KoopaTroopa.h"
#include "RedParaGoomba.h"
#include "GreenParaKoopa.h"
#include "Piranha.h"
#include "StaticCoin.h"
#include "Mushroom.h"
#include "Leaf.h"
#include "PlayScene.h"
#include "Mario.h"
#include "Portal.h"
#include "GreenKoopaTroopa.h"
#include "PButton.h"
#include "HUD.h"

void CRaccoonMario::OnHit()
{
    CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
    CMario* bigMario = new CMario(x, y);
    bigMario->SetLevel(MARIO_LEVEL_BIG);
    bigMario->SetState(state);
    bigMario->SetSpeed(vx, vy);
    bigMario->SetNx(nx);
    bigMario->StartUntouchable();
    scene->AddObject(bigMario);
    scene->SetPlayer(bigMario);
    Delete();
}

void CRaccoonMario::Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects)
{
    if (!isAppearing && !isBlinking)
    {
        vy += ay * dt;

        if (isRunning && (state == MARIO_STATE_RUNNING_RIGHT || state == MARIO_STATE_RUNNING_LEFT))
        {
            runProgress += MARIO_RUN_PROGRESS_ACCEL * dt;
            if (runProgress > 1.0f) runProgress = 1.0f;
        }
        else if (isFlying && GetTickCount64() - flyStart <= 4000)
        {
            // Giữ runProgress không đổi
        }
        else if (GetTickCount64() - lastFlapTime <= 2000)
        {
            // Giữ runProgress không đổi
        }
        else if (GetTickCount64() - jumpStartTime > 500)
        {
            runProgress -= MARIO_RUN_PROGRESS_DECAY * dt;
            if (runProgress < 0.0f) runProgress = 0.0f;
            if (isFlying && GetTickCount64() - lastFlapTime > 2000)
            {
                SetState(MARIO_STATE_FLY_DROP);
            }
        }

        float speedRange = MARIO_RUNNING_SPEED - MARIO_WALKING_SPEED;
        maxVx = (nx > 0) ?
            MARIO_WALKING_SPEED + speedRange * runProgress :
            -(MARIO_WALKING_SPEED + speedRange * runProgress);

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

        // Kiểm tra gần mặt đất để reset trạng thái
        if (y >= GROUND_Y - MARIO_RACCOON_BBOX_HEIGHT / 2 && abs(vy) < 0.01f && !isOnPlatform)
        {
            vy = 0;
            ay = MARIO_GRAVITY;
            isFlying = false;
            canFlap = false;
            flyStart = 0;
            flapCooldown = 0;
            jumpStartTime = 0;
            lastFlapTime = 0;
            isFlapping = false;
            isOnPlatform = true;
            SetState(MARIO_STATE_IDLE);
            DebugOut(L"[DEBUG] Reset state near ground: y=%f, vy=%f, isOnPlatform=%d\n", y, vy, isOnPlatform);
        }

        UpdateFlying(dt);

        if (isFlapping && GetTickCount64() - flapCooldown > 200)
        {
            isFlapping = false;
        }
    }

    if (isTailAttacking && GetTickCount64() - tailAttackStart > MARIO_TAIL_ATTACK_DURATION)
    {
        isTailAttacking = false;
        tailAttackCooldown = GetTickCount64();
        SetState(MARIO_STATE_IDLE);
    }

    if (GetTickCount64() - untouchable_start > MARIO_UNTOUCHABLE_TIME)
    {
        untouchable_start = 0;
        untouchable = 0;
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

BOOLEAN CRaccoonMario::CanStartFlying()
{
    return runProgress >= 1.0f && !isTailAttacking && !isHolding;
}

void CRaccoonMario::StartFlying()
{
    isFlying = true;
    canFlap = true;
    flyStart = GetTickCount64();
    ay = MARIO_FLY_GRAVITY;
}

void CRaccoonMario::StopFlying()
{
    isFlying = false;
    canFlap = true;
    flyStart = 0;
    ay = MARIO_FLY_DROP_GRAVITY;
}

void CRaccoonMario::UpdateFlying(DWORD dt)
{
    if (isFlying && GetTickCount64() - flyStart > MARIO_FLY_DURATION)
    {
        SetState(MARIO_STATE_FLY_DROP);
    }
}

void CRaccoonMario::Flap()
{
    if (GetTickCount64() - flapCooldown < MARIO_FLAP_COOLDOWN) return;

    flapCooldown = GetTickCount64();
    lastFlapTime = GetTickCount64();
    isFlapping = true;

    if (!isOnPlatform)
    {
        if (state == MARIO_STATE_JUMP && CanStartFlying() && canFlap)
        {
            SetState(MARIO_STATE_FLY);
        }
        else if (isFlying && canFlap)
        {
            vy -= MARIO_FLY_FLAP_FORCE;
            if (vy < -MARIO_FLY_SPEED_Y) vy = -MARIO_FLY_SPEED_Y;
        }
        else
        {
            vy -= MARIO_FLY_DROP_FLAP_FORCE;
            if (vy < 0) vy = 0;
        }
    }
}

int CRaccoonMario::GetAniIdRaccoon()
{
    int aniId = -1;

    if (isTailAttacking)
    {
        aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_TAIL_ATTACK_RIGHT : ID_ANI_RACCOON_MARIO_TAIL_ATTACK_LEFT;
    }
    else if (isFlying)
    {
        aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_FLY_RIGHT : ID_ANI_RACCOON_MARIO_FLY_LEFT;
    }
    else if (state == MARIO_STATE_FLY_DROP || (isFlapping && !isOnPlatform))
    {
        aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_FLY_DROP_RIGHT : ID_ANI_RACCOON_MARIO_FLY_DROP_LEFT;
    }
    else if (isHolding)
    {
        if (vx == 0)
        {
            aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_HOLD_RIGHT : ID_ANI_RACCOON_MARIO_HOLD_LEFT;
        }
        else if (abs(ax) == MARIO_ACCEL_RUN_X || runProgress >= 0.8f)
        {
            aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_HOLD_RIGHT : ID_ANI_RACCOON_MARIO_HOLD_LEFT;
        }
        else
        {
            aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_HOLD_RIGHT : ID_ANI_RACCOON_MARIO_HOLD_LEFT;
        }
    }
    else
    {
        if (!isOnPlatform)
        {
            if (abs(ax) == MARIO_ACCEL_RUN_X)
            {
                aniId = (nx >= 0) ? ID_ANI_RACCOON_MARIO_JUMP_RIGHT : ID_ANI_RACCOON_MARIO_JUMP_LEFT;
            }
            else
            {
                if (vy < 0)
                    aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_JUMP_RIGHT : ID_ANI_RACCOON_MARIO_JUMP_LEFT;
                else
                    aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_FALL_RIGHT : ID_ANI_RACCOON_MARIO_FALL_LEFT;
            }
        }
        else if (isSitting)
        {
            aniId = (nx > 0) ? ID_ANI_MARIO_SIT_RIGHT : ID_ANI_MARIO_SIT_LEFT;
        }
        else if (vx == 0)
        {
            aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_IDLE_RIGHT : ID_ANI_RACCOON_MARIO_IDLE_LEFT;
        }
        else if (vx > 0)
        {
            if (ax < 0)
                aniId = ID_ANI_RACCOON_MARIO_BRACE_RIGHT;
            else if (ax == MARIO_ACCEL_RUN_X || runProgress >= 0.8f)
                aniId = ID_ANI_RACCOON_MARIO_RUN_RIGHT;
            else if (ax == MARIO_ACCEL_WALK_X)
                aniId = ID_ANI_RACCOON_MARIO_WALK_RIGHT;
        }
        else
        {
            if (ax > 0)
                aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_BRACE_LEFT : ID_ANI_RACCOON_MARIO_BRACE_LEFT;
            else if (ax == -MARIO_ACCEL_RUN_X || runProgress >= 0.8f)
                aniId = ID_ANI_RACCOON_MARIO_RUN_LEFT;
            else if (ax == -MARIO_ACCEL_WALK_X)
                aniId = ID_ANI_RACCOON_MARIO_WALK_LEFT;
        }
    }

    if (aniId == -1)
        aniId = (nx > 0) ? ID_ANI_RACCOON_MARIO_IDLE_RIGHT : ID_ANI_RACCOON_MARIO_IDLE_LEFT;

    return aniId;
}

void CRaccoonMario::SetState(int state)
{
    if (state == MARIO_STATE_TAIL_ATTACK)
    {
        isTailAttacking = true;
        tailAttackStart = GetTickCount64();
        ax = 0.0f;
        vx = 0.0f;
    }
    else if (state == MARIO_STATE_FLY)
    {
        StartFlying();
    }
    else if (state == MARIO_STATE_FLY_DROP)
    {
        StopFlying();
    }
    else if (state == MARIO_STATE_JUMP)
    {
        jumpStartTime = GetTickCount64();
        CMario::SetState(state);
    }
    else
    {
        CMario::SetState(state);
    }
}

void CRaccoonMario::OnCollisionWith(LPCOLLISIONEVENT e)
{
    if (e->ny != 0 && e->obj->IsBlocking())
    {
        vy = 0;
        if (e->ny < 0)
        {
            isOnPlatform = true;
            ay = MARIO_GRAVITY;
            // Reset trạng thái bay bất kể vy
            if (isFlying || state == MARIO_STATE_FLY || state == MARIO_STATE_FLY_DROP)
            {
                isFlying = false;
                canFlap = false;
                flyStart = 0;
                flapCooldown = 0;
                jumpStartTime = 0;
                lastFlapTime = 0;
                isFlapping = false;
                SetState(MARIO_STATE_IDLE);
                DebugOut(L"[DEBUG] Reset flying state on collision: vy=%f, ay=%f, isOnPlatform=%d, state=%d\n", vy, ay, isOnPlatform, state);
            }
        }
    }
    else if (e->nx != 0 && e->obj->IsBlocking())
    {
        vx = 0;
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

void CRaccoonMario::OnCollisionWithGoomba(LPCOLLISIONEVENT e)
{
    CGoomba* goomba = dynamic_cast<CGoomba*>(e->obj);

    if (isTailAttacking && goomba->GetState() != GOOMBA_STATE_DIE && goomba->GetState() != GOOMBA_STATE_HEADSHOT)
    {
        goomba->SetState(GOOMBA_STATE_HEADSHOT);
    }
    else if (e->ny < 0)
    {
        if (goomba->GetState() != GOOMBA_STATE_DIE && goomba->GetState() != GOOMBA_STATE_HEADSHOT)
        {
            goomba->SetState(GOOMBA_STATE_DIE);
            vy = -MARIO_JUMP_DEFLECT_SPEED;
        }
    }
    else
    {
        if (untouchable == 0 && goomba->GetState() != GOOMBA_STATE_DIE && goomba->GetState() != GOOMBA_STATE_HEADSHOT)
        {
            OnHit();
        }
    }
}

void CRaccoonMario::OnCollisionWithCoin(LPCOLLISIONEVENT e)
{
    e->obj->Delete();
    CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
    CHUD* hud = scene->GetHUD();
    if (hud) hud->AddCoin();
}

void CRaccoonMario::OnCollisionWithVenusFire(LPCOLLISIONEVENT e)
{
    VenusFire* venusFire = dynamic_cast<VenusFire*>(e->obj);

    if (untouchable == 0 && venusFire->GetState() != VENUS_FIRE_STATE_HIDDEN)
    {
        OnHit();
    }
}

void CRaccoonMario::OnCollisionWithFireball(LPCOLLISIONEVENT e)
{
    Fireball* fireball = dynamic_cast<Fireball*>(e->obj);
    fireball->Delete();

    if (untouchable == 0)
    {
        OnHit();
    }
}

void CRaccoonMario::OnCollisionWithKoopaTroopa(LPCOLLISIONEVENT e)
{
    KoopaTroopa* koopa = dynamic_cast<KoopaTroopa*>(e->obj);

    if (isTailAttacking && koopa->GetState() != KOOPA_STATE_HEADSHOT)
    {
        koopa->SetState(KOOPA_STATE_HEADSHOT);
    }
    else if (e->ny < 0)
    {
        if (koopa->GetState() == KOOPA_STATE_WALKING)
        {
            koopa->SetState(KOOPA_STATE_SHELL_IDLE);
            vy = -MARIO_JUMP_DEFLECT_SPEED;
        }
        else if (koopa->GetState() == KOOPA_STATE_SHELL_RUNNING || koopa->GetState() == KOOPA_STATE_HEADSHOT)
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
                koopa->SetDirection(koopa->IsFlipped() ? (x > koopaX ? -1 : 1) : (x > koopaX ? 1 : -1));
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
                OnHit();
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
                koopa->SetDirection(koopa->IsFlipped() ? (x > koopaX ? -1 : 1) : (x > koopaX ? 1 : -1));
                koopa->SetState(KOOPA_STATE_SHELL_RUNNING);
            }
        }
        else if (koopa->GetState() == KOOPA_STATE_HEADSHOT)
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
                koopa->SetDirection(koopa->IsFlipped() ? (x > koopaX ? -1 : 1) : (x > koopaX ? 1 : -1));
                koopa->SetState(KOOPA_STATE_SHELL_RUNNING);
            }
        }
    }
}

void CRaccoonMario::OnCollisionWithRedParaGoomba(LPCOLLISIONEVENT e)
{
    RedParaGoomba* paraGoomba = dynamic_cast<RedParaGoomba*>(e->obj);

    if (isTailAttacking && paraGoomba->GetState() != RED_PARAGOOMBA_STATE_DIE && paraGoomba->GetState() != RED_PARAGOOMBA_STATE_HEADSHOT)
    {
        paraGoomba->SetState(RED_PARAGOOMBA_STATE_HEADSHOT);
    }
    else if (e->ny < 0)
    {
        if (paraGoomba->GetState() != RED_PARAGOOMBA_STATE_DIE && paraGoomba->GetState() != RED_PARAGOOMBA_STATE_HEADSHOT)
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
    }
    else
    {
        if (untouchable == 0 && paraGoomba->GetState() != RED_PARAGOOMBA_STATE_DIE && paraGoomba->GetState() != RED_PARAGOOMBA_STATE_HEADSHOT)
        {
            OnHit();
        }
    }
}

void CRaccoonMario::OnCollisionWithGreenParaKoopa(LPCOLLISIONEVENT e)
{
    GreenParaKoopa* paraKoopa = dynamic_cast<GreenParaKoopa*>(e->obj);

    if (isTailAttacking && paraKoopa->GetState() != GREEN_PARAKOOPA_STATE_DIE && paraKoopa->GetState() != GREEN_PARAKOOPA_STATE_HEADSHOT)
    {
        paraKoopa->SetState(GREEN_PARAKOOPA_STATE_HEADSHOT);
    }
    else if (e->ny < 0)
    {
        if (paraKoopa->GetState() != GREEN_PARAKOOPA_STATE_DIE && paraKoopa->GetState() != GREEN_PARAKOOPA_STATE_HEADSHOT)
        {
            CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
            float kx, ky;
            paraKoopa->GetPosition(kx, ky);
            GreenKoopaTroopa* koopa = new GreenKoopaTroopa(kx, ky - GREEN_PARAKOOPA_BBOX_HEIGHT / 2);
            koopa->SetState(KOOPA_STATE_SHELL_IDLE);
            scene->AddObject(koopa);
            paraKoopa->SetState(GREEN_PARAKOOPA_STATE_DIE);
            vy = -MARIO_JUMP_DEFLECT_SPEED;
        }
        else if (paraKoopa->GetState() == GREEN_PARAKOOPA_STATE_HEADSHOT)
        {
            CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
            float kx, ky;
            paraKoopa->GetPosition(kx, ky);
            GreenKoopaTroopa* koopa = new GreenKoopaTroopa(kx, ky - GREEN_PARAKOOPA_BBOX_HEIGHT / 2);
            koopa->SetState(KOOPA_STATE_SHELL_RUNNING);
            koopa->SetDirection(x > kx ? 1 : -1);
            koopa->SetFlipped(true);
            scene->AddObject(koopa);
            paraKoopa->SetState(GREEN_PARAKOOPA_STATE_DIE);
            vy = -MARIO_JUMP_DEFLECT_SPEED;
        }
    }
    else
    {
        if (paraKoopa->GetState() == GREEN_PARAKOOPA_STATE_HEADSHOT)
        {
            if (IsKeyDown(DIK_A))
            {
                if (!isHolding)
                {
                    CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
                    float kx, ky;
                    paraKoopa->GetPosition(kx, ky);
                    GreenKoopaTroopa* koopa = new GreenKoopaTroopa(kx, ky - GREEN_PARAKOOPA_BBOX_HEIGHT / 2);
                    koopa->SetState(KOOPA_STATE_CARRIED);
                    koopa->SetFlipped(true);
                    SetHeldKoopa(koopa);
                    scene->AddObject(koopa);
                    paraKoopa->SetState(GREEN_PARAKOOPA_STATE_DIE);
                }
            }
            else
            {
                CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
                float kx, ky;
                paraKoopa->GetPosition(kx, ky);
                GreenKoopaTroopa* koopa = new GreenKoopaTroopa(kx, ky - GREEN_PARAKOOPA_BBOX_HEIGHT / 2);
                koopa->SetState(KOOPA_STATE_SHELL_RUNNING);
                koopa->SetDirection(x > kx ? 1 : -1);
                koopa->SetFlipped(true);
                scene->AddObject(koopa);
                paraKoopa->SetState(GREEN_PARAKOOPA_STATE_DIE);
            }
        }
        else if (untouchable == 0 && paraKoopa->GetState() != GREEN_PARAKOOPA_STATE_DIE)
        {
            OnHit();
        }
    }
}

void CRaccoonMario::OnCollisionWithPiranha(LPCOLLISIONEVENT e)
{
    Piranha* piranha = dynamic_cast<Piranha*>(e->obj);

    if (untouchable == 0 && piranha->GetState() != PIRANHA_STATE_HIDDEN)
    {
        OnHit();
    }
}

void CRaccoonMario::OnCollisionWithStaticCoin(LPCOLLISIONEVENT e)
{
    e->obj->Delete();
    CPlayScene* scene = (CPlayScene*)CGame::GetInstance()->GetCurrentScene();
    CHUD* hud = scene->GetHUD();
    if (hud) hud->AddCoin();
}

void CRaccoonMario::OnCollisionWithPButton(LPCOLLISIONEVENT e)
{
    PButton* button = dynamic_cast<PButton*>(e->obj);
    button->SetState(P_BUTTON_STATE_PRESSED);
}

void CRaccoonMario::OnCollisionWithPortal(LPCOLLISIONEVENT e)
{
    CPortal* p = (CPortal*)e->obj;
    CGame::GetInstance()->InitiateSwitchScene(p->GetSceneId());
}

void CRaccoonMario::OnCollisionWithMushroom(LPCOLLISIONEVENT e)
{
    CMushroom* mushroom = dynamic_cast<CMushroom*>(e->obj);
    mushroom->Delete();
}

void CRaccoonMario::OnCollisionWithLeaf(LPCOLLISIONEVENT e)
{
    CLeaf* leaf = dynamic_cast<CLeaf*>(e->obj);
    leaf->Delete();
}

void CRaccoonMario::OnHitByKoopa()
{
    if (untouchable == 0)
    {
        OnHit();
    }
}

void CRaccoonMario::Render()
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

    if (isAppearing && appearStart != 0)
    {
        ULONGLONG timeSinceAppear = GetTickCount64() - appearStart;
        if (timeSinceAppear < MARIO_APPEAR_TIME)
        {
            aniId = ID_ANI_RACCOON_MARIO_APPEAR;
        }
        else
        {
            isAppearing = false;
            appearStart = 0;
            aniId = GetAniIdRaccoon();
        }
    }
    else
    {
        aniId = GetAniIdRaccoon();
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

void CRaccoonMario::GetBoundingBox(float& left, float& top, float& right, float& bottom)
{
    if (isSitting)
    {
        left = x - MARIO_BIG_SITTING_BBOX_WIDTH / 2;
        top = y - MARIO_BIG_SITTING_BBOX_HEIGHT / 2;
        right = left + MARIO_BIG_SITTING_BBOX_WIDTH;
        bottom = top + MARIO_BIG_SITTING_BBOX_HEIGHT;
    }
    else if (isTailAttacking)
    {
        left = x - (MARIO_RACCOON_BBOX_WIDTH / 2 + MARIO_TAIL_ATTACK_BBOX_WIDTH_EXTEND);
        top = y - MARIO_RACCOON_BBOX_HEIGHT / 2;
        right = x + (MARIO_RACCOON_BBOX_WIDTH / 2 + MARIO_TAIL_ATTACK_BBOX_WIDTH_EXTEND);
        bottom = top + MARIO_RACCOON_BBOX_HEIGHT;
    }
    else
    {
        left = x - MARIO_RACCOON_BBOX_WIDTH / 2;
        top = y - MARIO_RACCOON_BBOX_HEIGHT / 2;
        right = left + MARIO_RACCOON_BBOX_WIDTH;
        bottom = top + MARIO_RACCOON_BBOX_HEIGHT;
    }
}