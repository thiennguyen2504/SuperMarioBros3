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
#include "SampleKeyEventHandler.h"
#include "Mushroom.h"
#include "QuestionBlock.h"
#include "Leaf.h"
#include "HUD.h"

#define GAME_MARGIN 10.0f
#define HUD_HEIGHT 26.0f
#define BACKGROUND_COLOR D3DXCOLOR(156.0f/255, 252.0f/255, 240.0f/255, 100.0f)

class CPlayScene : public CScene
{
protected:
	LPGAMEOBJECT player;
	vector<LPGAMEOBJECT> objects;
	vector<LPGAMEOBJECT> newObjects;
	//lLPHUD hud;

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


	void SetPlayer(LPGAMEOBJECT player) { this->player = player; }
	LPGAMEOBJECT GetPlayer() { return player; }

	void AddObject(LPGAMEOBJECT obj) { newObjects.push_back(obj); }

	void Clear();
	void PurgeDeletedObjects();

	static bool IsGameObjectDeleted(const LPGAMEOBJECT& o);
};

typedef CPlayScene* LPPLAYSCENE;