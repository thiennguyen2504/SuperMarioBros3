#pragma once
#include "GameObject.h"

#define QUESTION_BLOCK_BBOX_WIDTH 16
#define QUESTION_BLOCK_BBOX_HEIGHT 16

#define QUESTION_BLOCK_STATE_ACTIVE 100
#define QUESTION_BLOCK_STATE_INACTIVE 200

#define QUESTION_BLOCK_TYPE_COIN 1
#define QUESTION_BLOCK_TYPE_ITEM 2

#define QUESTION_BLOCK_BOUNCE_HEIGHT 8.0f
#define QUESTION_BLOCK_BOUNCE_TIME 200

#define ID_ANI_QUESTION_BLOCK_ACTIVE 120000
#define ID_ANI_QUESTION_BLOCK_INACTIVE 120001


class CQuestionBlock : public CGameObject
{
protected:
    int type;
    BOOLEAN hasItem;
    BOOLEAN isBouncing;
    float originalY;
    ULONGLONG bounceStart;

    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) override;
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects) override;
    virtual void Render() override;
    virtual void OnCollisionWith(LPCOLLISIONEVENT e) override;

public:
    CQuestionBlock(float x, float y, int blockType);
    virtual void SetState(int state) override;
    void StartBounce();
    virtual int IsCollidable() { return 1; }
    virtual int IsBlocking() { return 1; }
	bool HasItem() { return hasItem; }
	int GetType() { return type; }

};