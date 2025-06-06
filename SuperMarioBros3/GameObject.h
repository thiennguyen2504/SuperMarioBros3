#pragma once
#include <Windows.h>
#include <d3dx10.h>
#include <vector>
#include "Animation.h"
#include "Animations.h"
#include "Sprites.h"
#include "Collision.h"

using namespace std;

#define ID_TEX_BBOX -100		// special texture to draw object bounding box
#define BBOX_ALPHA 0.25f		// Bounding box transparency

class CGameObject
{
protected:
    float x;
    float y;
    float vx;
    float vy;
    int nx;
    int state;
    bool isDeleted;
    bool isActive;

public:
    void SetPosition(float x, float y) { this->x = x, this->y = y; }
    void SetSpeed(float vx, float vy) { this->vx = vx, this->vy = vy; }
    void GetPosition(float& x, float& y) { x = this->x; y = this->y; }
    void GetSpeed(float& vx, float& vy) { vx = this->vx; vy = this->vy; }
    int GetState() { return this->state; }
    virtual void Delete() { isDeleted = true; }
    bool IsDeleted() { return isDeleted; }
    bool IsActive() { return isActive; }
    void SetActive(bool active) { isActive = active; }
    virtual bool IsEnemy() { return false; }

    void RenderBoundingBox();
    CGameObject() : CGameObject(0, 0) {}
    CGameObject(float x, float y)
    {
        this->x = x;
        this->y = y;
        vx = vy = 0;
        nx = 1;
        state = 0;
        isDeleted = false;
        isActive = true;
    }
    void SetDeleted(bool isDeleted) { this->isDeleted = isDeleted; }
    virtual void GetBoundingBox(float& left, float& top, float& right, float& bottom) = 0;
    virtual void Update(DWORD dt, vector<LPGAMEOBJECT>* coObjects = nullptr);
    virtual void Render() = 0;
    virtual void SetState(int state) { this->state = state; }
    virtual int IsCollidable() { return !isDeleted; }
    virtual void OnNoCollision(DWORD dt);
    virtual void OnCollisionWith(LPCOLLISIONEVENT e) {}
    virtual int IsBlocking() { return 1; }
    virtual int IsDirectionColliable(float nx, float ny) { return 1; }
    ~CGameObject();
    static bool IsDeleted(const LPGAMEOBJECT& o) { return o->isDeleted; }
};

typedef CGameObject* LPGAMEOBJECT;