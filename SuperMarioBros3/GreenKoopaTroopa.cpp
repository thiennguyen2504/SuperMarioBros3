#include "GreenKoopaTroopa.h"
#include "Game.h"
#include "Collision.h"
#include "Mario.h"
#include "PlayScene.h"

GreenKoopaTroopa::GreenKoopaTroopa(float x, float y) : KoopaTroopa(x, y)
{
    this->direction = -1;
    this->vx = direction * KOOPA_WALKING_SPEED;
}

void GreenKoopaTroopa::Render()
{
    int aniId = -1;
    if (state == KOOPA_STATE_WALKING)
    {
        aniId = ID_ANI_GREEN_KOOPA_WALK_LEFT;
    }
    else if (state == KOOPA_STATE_SHELL_IDLE || state == KOOPA_STATE_CARRIED)
    {
        aniId = ID_ANI_GREEN_KOOPA_SHELL_IDLE;
    }
    else if (state == KOOPA_STATE_SHELL_RUNNING)
    {
        aniId = ID_ANI_GREEN_KOOPA_SHELL_RUN;
    }
    else if (state == KOOPA_STATE_HEADSHOT)
    {
        aniId = (vx == 0) ? ID_ANI_GREEN_KOOPA_SHELL_IDLE : ID_ANI_GREEN_KOOPA_SHELL_RUN;
    }

    bool shouldRender = true;
    if (state == KOOPA_STATE_SHELL_IDLE || state == KOOPA_STATE_CARRIED)
    {
        if (shellIdleStart > 0)
        {
            ULONGLONG timeSinceIdle = GetTickCount64() - shellIdleStart;
            ULONGLONG timeUntilRevive = KOOPA_SHELL_IDLE_TIMEOUT - timeSinceIdle;
            if (timeUntilRevive <= KOOPA_BLINK_DURATION)
            {
                ULONGLONG blinkPhase = timeSinceIdle % (2 * KOOPA_BLINK_INTERVAL);
                shouldRender = (blinkPhase < KOOPA_BLINK_INTERVAL); 
            }
        }
    }

    CAnimations* animations = CAnimations::GetInstance();
    if (shouldRender && aniId != -1)
    {
        LPANIMATION animation = animations->Get(aniId);
        if (animation != nullptr)
        {
            if (state == KOOPA_STATE_HEADSHOT)
                animation->RenderFlipped180(x, y);
            else
                animation->Render(x, y);
        }
    }
}