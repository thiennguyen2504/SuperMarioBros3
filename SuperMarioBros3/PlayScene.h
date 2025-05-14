#pragma once
#include "debug.h"
#include "Game.h"
#include "Textures.h"
#include "Scene.h"
#include "GameObject.h"
#include "Brick.h"
#include "Mario.h"
#include "Goomba.h"
#include "Coin.h"
#include "Platform.h"
#include "Pipe.h"
#include "Bush.h"
#include "RectPlatform.h"
#include "BrickPlatform.h"
#include "VenusFire.h"
#include "Fireball.h"
#include "RedKoopaTroopa.h"
#include "SampleKeyEventHandler.h"
#include "Mushroom.h"
#include "QuestionBlock.h"
#include "Leaf.h"

class CPlayScene : public CScene
{
protected:
    LPGAMEOBJECT player;
    vector<LPGAMEOBJECT> objects;
    vector<LPGAMEOBJECT> newObjects; 

    void _ParseSection_SPRITES(string line);
    void _ParseSection_ANIMATIONS(string line);
    void _ParseSection_ASSETS(string line);
    void _ParseSection_OBJECTS(string line);

    void LoadAssets(LPCWSTR assetFile);

public:
    CPlayScene(int id, LPCWSTR filePath);

    virtual void Load();
    virtual void Update(DWORD dt);
    virtual void Render();
    virtual void Unload();

    LPGAMEOBJECT GetPlayer() { return player; }

    void AddObject(LPGAMEOBJECT obj) { newObjects.push_back(obj); }

    void Clear();
    void PurgeDeletedObjects();

    static bool IsGameObjectDeleted(const LPGAMEOBJECT& o);
};

typedef CPlayScene* LPPLAYSCENE;