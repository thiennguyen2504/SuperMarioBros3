#pragma once
#include "KoopaTroopa.h"

#define ID_ANI_GREEN_KOOPA_WALK_LEFT 200031
#define ID_ANI_GREEN_KOOPA_SHELL_IDLE 200033
#define ID_ANI_GREEN_KOOPA_SHELL_RUN 200034

class GreenKoopaTroopa : public KoopaTroopa
{
protected:
    virtual void Render() override;

public:
    GreenKoopaTroopa(float x, float y);
};