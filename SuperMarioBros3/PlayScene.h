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
#include "RectPlatform.h"
#include "BrickPlatform.h"
#include "VenusFire.h"
#include "Fireball.h"
#include "RedKoopaTroopa.h"
#include "RedParaGoomba.h"
#include "GreenParaKoopa.h"
#include "Piranha.h"
#include "PButton.h"
#include "GoldBrick.h"
#include "Mushroom.h"
#include "QuestionBlock.h"
#include "Leaf.h"
#include "HUD.h"
#include "Effect.h"
#include "BlackBackground.h"
#include "Card.h"
#include "HiddenBrick.h"

#define GAME_MARGIN 0.1f
#define HUD_HEIGHT 26.0f
#define BACKGROUND_COLOR D3DXCOLOR(156.0f/255, 252.0f/255, 240.0f/255, 100.0f)
#define ENEMY_ACTIVE_THRESHOLD_X 20.0f

struct KoopaSpawnInfo
{
    float startX, startY;
    int type;
};

class CPlayScene : public CScene
{
protected:
    LPGAMEOBJECT player;
    vector<LPGAMEOBJECT> objects;
    vector<LPGAMEOBJECT> newObjects;
    vector<LPGAMEOBJECT> goldBricks;
    vector<KoopaSpawnInfo> deletedKoopaSpawns;
    LPHUD hud;
    LPGAMEOBJECT blackBackground;

    void _ParseSection_SPRITES(string line);
    void _ParseSection_ANIMATIONS(string line);
    void _ParseSection_ASSETS(string line);
    void _ParseSection_OBJECTS(string line);
    void LoadAssets(LPCWSTR assetsPath);

public:
    CPlayScene(int id, LPCWSTR filePath);
    virtual void Load();
    virtual void Update(DWORD dt);
    virtual void Render();
    virtual void Unload();

    void SetPlayer(LPGAMEOBJECT player) { this->player = player; }
    LPGAMEOBJECT GetPlayer() { return player; }
    LPHUD GetHUD() { return hud; }
    vector<LPGAMEOBJECT> GetGoldBricks() { return goldBricks; }

    void AddObject(LPGAMEOBJECT obj);
    void Clear();
    void PurgeDeletedObjects();
    static bool IsGameObjectDeleted(const LPGAMEOBJECT& o);
};

typedef CPlayScene* LPPLAYSCENE;