#include <iostream>
#include <fstream>
#include "AssetIDs.h"
#include "PlayScene.h"
#include "Utils.h"
#include "Textures.h"
#include "Sprites.h"
#include "Portal.h"
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
#include "SampleKeyEventHandler.h"
#include "Grass.h"
#include "MapObjects.h"
#include "RacoonMario.h"
#include "GreenKoopaTroopa.h"
#include "debug.h"

using namespace std;

CPlayScene::CPlayScene(int id, LPCWSTR filePath) : CScene(id, filePath)
{
    player = nullptr;
    key_handler = new CSampleKeyHandler(this);
    hud = nullptr;
    blackBackground = nullptr;
}

#define SCENE_SECTION_UNKNOWN -1
#define SCENE_SECTION_ASSETS 1
#define SCENE_SECTION_OBJECTS 2

#define ASSETS_SECTION_UNKNOWN -1
#define ASSETS_SECTION_SPRITES 1
#define ASSETS_SECTION_ANIMATIONS 2
#define MAX_SCENE_LINE 1024

void CPlayScene::_ParseSection_SPRITES(string line)
{
    vector<string> tokens = split(line);
    if (tokens.size() < 6) return;

    int ID = atoi(tokens[0].c_str());
    int l = atoi(tokens[1].c_str());
    int t = atoi(tokens[2].c_str());
    int r = atoi(tokens[3].c_str());
    int b = atoi(tokens[4].c_str());
    int texID = atoi(tokens[5].c_str());

    LPTEXTURE tex = CTextures::GetInstance()->Get(texID);
    if (tex == nullptr)
    {
        DebugOut(L"[ERROR] Texture ID %d not found!\n", texID);
        return;
    }

    CSprites::GetInstance()->Add(ID, l, t, r, b, tex);
}

void CPlayScene::_ParseSection_ASSETS(string line)
{
    vector<string> tokens = split(line);
    if (tokens.size() < 1) return;

    wstring path = ToWSTR(tokens[0]);
    LoadAssets(path.c_str());
}

void CPlayScene::_ParseSection_ANIMATIONS(string line)
{
    vector<string> tokens = split(line);
    if (tokens.size() < 3) return;

    LPANIMATION ani = new CAnimation();
    int ani_id = atoi(tokens[0].c_str());
    for (size_t i = 1; i < tokens.size(); i += 2)
    {
        int sprite_id = atoi(tokens[i].c_str());
        int frame_time = atoi(tokens[i + 1].c_str());
        ani->Add(sprite_id, frame_time);
    }

    CAnimations::GetInstance()->Add(ani_id, ani);
}

void CPlayScene::_ParseSection_OBJECTS(string line)
{
    vector<string> tokens = split(line);
    if (tokens.size() < 2) return;

    int object_type = atoi(tokens[0].c_str());
    float x = (float)atof(tokens[1].c_str());
    float y = (float)atof(tokens[2].c_str());

    CGameObject* obj = nullptr;

    switch (object_type)
    {
    case OBJECT_TYPE_MARIO:
        if (player != nullptr)
        {
            DebugOut(L"[ERROR] MARIO object was created before!\n");
            return;
        }
        obj = new CMario(x, y);
        player = (CMario*)obj;
        DebugOut(L"[INFO] Player object has been created!\n");
        break;
    case OBJECT_TYPE_GOOMBA: obj = new CGoomba(x, y); break;
    case OBJECT_TYPE_BRICK: obj = new CBrick(x, y); break;
    case OBJECT_TYPE_COIN: obj = new CCoin(x, y, false); break;
    case OBJECT_TYPE_GRASS:
    {
        int height = atoi(tokens[3].c_str());
        int type = atoi(tokens[4].c_str());
        obj = new CGrass(x, y, height, type);
        break;
    }
    case OBJECT_TYPE_RED_PARAGOOMBA:
    {
        obj = new RedParaGoomba(x, y);
        break;
    }
    case OBJECT_TYPE_GREEN_PARAKOOPA:
    {
        obj = new GreenParaKoopa(x, y);
        break;
    }
    case OBJECT_TYPE_PIRANHA:
    {
        obj = new Piranha(x, y);
        break;
    }
    case OBJECT_TYPE_GOLD_BRICK:
    {
        int type = atoi(tokens[3].c_str());
        obj = new GoldBrick(x, y, type);
        goldBricks.push_back(obj);
        break;
    }
    case OBJECT_TYPE_P_BUTTON:
    {
        obj = new PButton(x, y);
        break;
    }
    case OBJECT_TYPE_PLATFORM:
    {
        float cell_width = (float)atof(tokens[3].c_str());
        float cell_height = (float)atof(tokens[4].c_str());
        int length = atoi(tokens[5].c_str());
        int sprite_begin = atoi(tokens[6].c_str());
        int sprite_middle = atoi(tokens[7].c_str());
        int sprite_end = atoi(tokens[8].c_str());
        obj = new CPlatform(x, y, cell_width, cell_height, length, sprite_begin, sprite_middle, sprite_end);
        break;
    }
    case OBJECT_TYPE_RECT_PLATFORM:
    {
        float cell_width = (float)atof(tokens[3].c_str());
        float cell_height = (float)atof(tokens[4].c_str());
        int width = atoi(tokens[5].c_str());
        int height = atoi(tokens[6].c_str());
        int sprite_id_top_left = atoi(tokens[7].c_str());
        int sprite_id_top_right = atoi(tokens[8].c_str());
        int sprite_id_bottom_left = atoi(tokens[9].c_str());
        int sprite_id_bottom_right = atoi(tokens[10].c_str());
        int sprite_id_top = atoi(tokens[11].c_str());
        int sprite_id_bottom = atoi(tokens[12].c_str());
        int sprite_id_left = atoi(tokens[13].c_str());
        int sprite_id_right = atoi(tokens[14].c_str());
        int sprite_id_middle = atoi(tokens[15].c_str());
        obj = new CRectPlatform(x, y, cell_width, cell_height, width, height, 0.33f,
            sprite_id_top_left, sprite_id_top_right, sprite_id_bottom_left, sprite_id_bottom_right,
            sprite_id_top, sprite_id_bottom, sprite_id_left, sprite_id_right, sprite_id_middle);
        break;
    }
    case OBJECT_TYPE_BRICK_PLATFORM:
    {
        float cell_width = (float)atof(tokens[3].c_str());
        float cell_height = (float)atof(tokens[4].c_str());
        int length = atoi(tokens[5].c_str());
        int sprite_begin = atoi(tokens[6].c_str());
        int sprite_middle = atoi(tokens[7].c_str());
        int sprite_end = atoi(tokens[8].c_str());
        float scale = tokens.size() > 9 ? (float)atof(tokens[9].c_str()) : 0.33f;
        obj = new CBrickPlatform(x, y, cell_width, cell_height, length, sprite_begin, sprite_middle, sprite_end, scale);
        break;
    }
    case OBJECT_TYPE_VENUS_FIRE:
    {
        int color = atoi(tokens[3].c_str());
        int sprite_id_left_down = atoi(tokens[4].c_str());
        int sprite_id_left_up = atoi(tokens[5].c_str());
        int sprite_id_right_down = atoi(tokens[6].c_str());
        int sprite_id_right_up = atoi(tokens[7].c_str());
        obj = new VenusFire(x, y, color, sprite_id_left_down, sprite_id_left_up, sprite_id_right_down, sprite_id_right_up);
        DebugOut(L"[INFO] VenusFire object created at (%f, %f)\n", x, y);
        break;
    }
    case OBJECT_TYPE_FIREBALL:
    {
        float targetX = (float)atof(tokens[3].c_str());
        float targetY = (float)atof(tokens[4].c_str());
        obj = new Fireball(x, y, targetX, targetY);
        break;
    }
    case OBJECT_TYPE_RED_KOOPA_TROOPA:
    {
        obj = new RedKoopaTroopa(x, y);
        deletedKoopaSpawns.push_back({ x, y, OBJECT_TYPE_RED_KOOPA_TROOPA });
        break;
    }
    case OBJECT_TYPE_GREEN_KOOPA_TROOPA:
    {
        obj = new GreenKoopaTroopa(x, y);
        deletedKoopaSpawns.push_back({ x, y, OBJECT_TYPE_GREEN_KOOPA_TROOPA });
        break;
    }
    case OBJECT_TYPE_PORTAL:
    {
        float r = (float)atof(tokens[3].c_str());
        float b = (float)atof(tokens[4].c_str());
        int scene_id = atoi(tokens[5].c_str());
        obj = new CPortal(x, y, r, b, scene_id);
        break;
    }
    case OBJECT_TYPE_PIPE:
    {
        int height = atoi(tokens[3].c_str());
        int sprite_id_head = atoi(tokens[4].c_str());
        int sprite_id_body = atoi(tokens[5].c_str());
        obj = new CPipe(x, y, height, sprite_id_head, sprite_id_body);
        break;
    }
    case OBJECT_TYPE_QUESTION_BLOCK_COIN:
    {
        obj = new CQuestionBlock(x, y, QUESTION_BLOCK_TYPE_COIN);
        break;
    }
    case OBJECT_TYPE_QUESTION_BLOCK_ITEM:
    {
        obj = new CQuestionBlock(x, y, QUESTION_BLOCK_TYPE_ITEM);
        break;
    }
    case OBJECT_TYPE_QUESTION_BLOCK_GREEN:
    {
        obj = new CQuestionBlock(x, y, QUESTION_BLOCK_TYPE_GREEN_MUSHROOM);
        break;
    }
    case OBJECT_TYPE_MAPOBJECTS:
    {
        int sprite_id = atoi(tokens[3].c_str());
        obj = new CMapObjects(x, y, sprite_id);
        break;
    }
    case OBJECT_TYPE_BLACKBACKGROUND:
    {
        float mapWidth = (float)atof(tokens[3].c_str());
        float mapHeight = (float)atof(tokens[4].c_str());
        obj = new BlackBackground(x, y, mapWidth, mapHeight);
        blackBackground = obj;
        break;
    }
    case OBJECT_TYPE_CARD:
    {
        obj = new Card(x, y);
        break;
    }
    default:
        DebugOut(L"[ERROR] Invalid object type: %d\n", object_type);
        return;
    }

    if (obj != nullptr)
    {
        obj->SetPosition(x, y);
        objects.push_back(obj);
    }
}

void CPlayScene::LoadAssets(LPCWSTR assetFile)
{
    DebugOut(L"[INFO] Start loading assets from : %s \n", assetFile);
    ifstream f;
    f.open(assetFile);
    int section = ASSETS_SECTION_UNKNOWN;
    char str[MAX_SCENE_LINE];

    while (f.getline(str, MAX_SCENE_LINE))
    {
        string line(str);
        if (line[0] == '#') continue;
        if (line == "[SPRITES]") { section = ASSETS_SECTION_SPRITES; continue; }
        if (line == "[ANIMATIONS]") { section = ASSETS_SECTION_ANIMATIONS; continue; }
        if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

        switch (section)
        {
        case ASSETS_SECTION_SPRITES: _ParseSection_SPRITES(line); break;
        case ASSETS_SECTION_ANIMATIONS: _ParseSection_ANIMATIONS(line); break;
        }
    }

    f.close();
    DebugOut(L"[INFO] Done loading assets from %s\n", assetFile);
}

void CPlayScene::Load()
{
    DebugOut(L"[INFO] Start loading scene from : %s \n", sceneFilePath);
    ifstream f;
    f.open(sceneFilePath);
    int section = SCENE_SECTION_UNKNOWN;
    char str[MAX_SCENE_LINE];

    while (f.getline(str, MAX_SCENE_LINE))
    {
        string line(str);
        if (line[0] == '#') continue;
        if (line == "[ASSETS]") { section = SCENE_SECTION_ASSETS; continue; }
        if (line == "[OBJECTS]") { section = SCENE_SECTION_OBJECTS; continue; }
        if (line[0] == '[') { section = SCENE_SECTION_UNKNOWN; continue; }

        switch (section)
        {
        case SCENE_SECTION_ASSETS: _ParseSection_ASSETS(line); break;
        case SCENE_SECTION_OBJECTS: _ParseSection_OBJECTS(line); break;
        }
    }

    f.close();
    hud = new CHUD();
    DebugOut(L"[INFO] Done loading scene  %s\n", sceneFilePath);
}

void CPlayScene::Update(DWORD dt)
{
    newObjects.clear();
    PurgeDeletedObjects();

    // Tính phạm vi kích hoạt dựa trên camera
    float cx, cy, playerX, playerY;
    CGame* game = CGame::GetInstance();
    float backBufferWidth = game->GetBackBufferWidth();
    float backBufferHeight = game->GetBackBufferHeight();
    const float CAMERA_RIGHT_LIMIT = 2800.0f;
    const float CAMERA_BOTTOM_EDGE = 0.0f; 

    if (!player)
    {
        cx = 0;
        cy = 0;
    }
    else
    {
        player->GetPosition(playerX, playerY);
        DebugOut(L"[DEBUG] Player position: (%f, %f)\n", playerX, playerY);
        cx = playerX - backBufferWidth / 2;
        cy = playerY - backBufferHeight / 2;
        if (cx < 0) cx = 0; 
        if (cx > CAMERA_RIGHT_LIMIT - backBufferWidth) cx = CAMERA_RIGHT_LIMIT - backBufferWidth; 
        if (cy > CAMERA_BOTTOM_EDGE) cy = CAMERA_BOTTOM_EDGE;
        game->SetCamPos(cx, cy);
    }
    const float ENEMY_UPDATE_MARGIN = 20.0f;
    float activeLeft = cx - ENEMY_UPDATE_MARGIN;
    float activeRight = cx + backBufferWidth + ENEMY_UPDATE_MARGIN;

    for (size_t i = 0; i < objects.size(); i++)
    {
        if (objects[i] && !objects[i]->IsDeleted())
        {
            KoopaTroopa* koopa = dynamic_cast<KoopaTroopa*>(objects[i]);
            if (koopa)
            {
                float ex, ey;
                koopa->GetPosition(ex, ey);
                if (ex < activeLeft || ex > activeRight)
                {
                    koopa->Delete();
                }
            }
        }
    }

    for (size_t i = 0; i < objects.size(); i++)
    {
        if (objects[i] && objects[i]->IsDeleted())
        {
            KoopaTroopa* koopa = dynamic_cast<KoopaTroopa*>(objects[i]);
            if (koopa)
            {
                bool alreadyTracked = false;
                for (const auto& spawn : deletedKoopaSpawns)
                {
                    if (spawn.startX == koopa->GetStartX() && spawn.startY == koopa->GetStartY())
                    {
                        alreadyTracked = true;
                        break;
                    }
                }
                if (!alreadyTracked)
                {
                    int type = dynamic_cast<RedKoopaTroopa*>(koopa) ? OBJECT_TYPE_RED_KOOPA_TROOPA : OBJECT_TYPE_GREEN_KOOPA_TROOPA;
                    deletedKoopaSpawns.push_back({ koopa->GetStartX(), koopa->GetStartY(), type });
                }
            }
        }
    }

    for (auto it = deletedKoopaSpawns.begin(); it != deletedKoopaSpawns.end(); )
    {
        if (it->startX < activeLeft || it->startX > activeRight)
        {
            bool koopaExists = false;
            for (const auto& obj : objects)
            {
                KoopaTroopa* koopa = dynamic_cast<KoopaTroopa*>(obj);
                if (koopa && koopa->GetStartX() == it->startX && koopa->GetStartY() == it->startY)
                {
                    koopaExists = true;
                    break;
                }
            }
            if (!koopaExists)
            {
                CGameObject* obj = nullptr;
                if (it->type == OBJECT_TYPE_RED_KOOPA_TROOPA)
                    obj = new RedKoopaTroopa(it->startX, it->startY);
                else if (it->type == OBJECT_TYPE_GREEN_KOOPA_TROOPA)
                    obj = new GreenKoopaTroopa(it->startX, it->startY);

                if (obj)
                {
                    obj->SetPosition(it->startX, it->startY);
                    AddObject(obj);
                    DebugOut(L"[INFO] Respawned KoopaTroopa type=%d at (%f, %f)\n", it->type, it->startX, it->startY);
                    it = deletedKoopaSpawns.erase(it);
                    continue;
                }
            }
        }
        ++it;
    }

    for (size_t i = 0; i < objects.size(); i++)
    {
        if (objects[i] && !objects[i]->IsDeleted() && dynamic_cast<Enemy*>(objects[i]))
        {
            float ex, ey;
            objects[i]->GetPosition(ex, ey);
            bool active = ex >= activeLeft && ex <= activeRight;
            objects[i]->SetActive(active);
        }
    }

    vector<LPGAMEOBJECT> coObjects;
    for (size_t i = 0; i < objects.size(); i++)
    {
        if (objects[i] && !objects[i]->IsDeleted())
        {
            coObjects.push_back(objects[i]);
        }
    }

    for (size_t i = 0; i < objects.size(); i++)
    {
        if (objects[i] && !objects[i]->IsDeleted() && objects[i]->GetState() != -1)
        {
            if (dynamic_cast<Enemy*>(objects[i]))
            {
                float ex, ey;
                objects[i]->GetPosition(ex, ey);
                if (ex >= activeLeft && ex <= activeRight)
                {
                    objects[i]->Update(dt, &coObjects);
                }
            }
            else
            {
                objects[i]->Update(dt, &coObjects);
            }
        }
    }

    for (size_t i = 0; i < newObjects.size(); i++)
    {
        if (newObjects[i] != nullptr)
        {
            objects.push_back(newObjects[i]);
        }
    }

    PurgeDeletedObjects();
}

void CPlayScene::Render()
{
    CGame* game = CGame::GetInstance();
    ID3D10Device* d3dDevice = game->GetDirect3DDevice();
    ID3D10RenderTargetView* renderTargetView = game->GetRenderTargetView();
    float backBufferWidth = game->GetBackBufferWidth();
    float backBufferHeight = game->GetBackBufferHeight();

    d3dDevice->ClearRenderTargetView(renderTargetView, BACKGROUND_COLOR);

    // Render BlackBackground first
    if (blackBackground && !blackBackground->IsDeleted())
    {
        blackBackground->Render();
    }

    // Render background objects
    for (size_t i = 0; i < objects.size(); i++)
    {
        if (objects[i] &&
            !dynamic_cast<CMario*>(objects[i]) &&
            !dynamic_cast<CMushroom*>(objects[i]) &&
            !dynamic_cast<CLeaf*>(objects[i]) &&
            !dynamic_cast<CQuestionBlock*>(objects[i]) &&
            !dynamic_cast<CPipe*>(objects[i]) &&
            !dynamic_cast<CBrick*>(objects[i]) &&
            !dynamic_cast<CEffect*>(objects[i]) &&
            !dynamic_cast<BlackBackground*>(objects[i]) &&
            !dynamic_cast<Card*>(objects[i]) &&
            !objects[i]->IsDeleted())
        {
            objects[i]->Render();
        }
    }

    // Render midground objects (Mushroom, Leaf, Pipe, Card)
    for (size_t i = 0; i < objects.size(); i++)
    {
        if (objects[i] &&
            (dynamic_cast<CMushroom*>(objects[i]) ||
                dynamic_cast<CLeaf*>(objects[i]) ||
                dynamic_cast<CPipe*>(objects[i]) ||
                dynamic_cast<Card*>(objects[i])) &&
            !objects[i]->IsDeleted())
        {
            objects[i]->Render();
        }
    }

    // Render foreground objects (QuestionBlock, Brick)
    for (size_t i = 0; i < objects.size(); i++)
    {
        if (objects[i] &&
            (dynamic_cast<CQuestionBlock*>(objects[i]) ||
                dynamic_cast<CBrick*>(objects[i])) &&
            !objects[i]->IsDeleted())
        {
            objects[i]->Render();
        }
    }

    // Render Mario
    for (size_t i = 0; i < objects.size(); i++)
    {
        if (objects[i] &&
            dynamic_cast<CMario*>(objects[i]) &&
            !objects[i]->IsDeleted())
        {
            objects[i]->Render();
        }
    }

    // Render effects (CEffect)
    for (size_t i = 0; i < objects.size(); i++)
    {
        if (objects[i] &&
            dynamic_cast<CEffect*>(objects[i]) &&
            !objects[i]->IsDeleted())
        {
            objects[i]->Render();
        }
    }

    // Render black overlay
    LPSPRITE blackSprite = CSprites::GetInstance()->Get(ID_SPRITE_BLACK);
    if (blackSprite != nullptr)
    {
        blackSprite->DrawStatic(0.0f, 205.0f);
    }

    // Render HUD
    if (hud != nullptr)
    {
        hud->Render();
    }
}

void CPlayScene::Clear()
{
    for (auto it = objects.begin(); it != objects.end(); ++it)
    {
        if (*it != nullptr)
        {
            delete (*it);
        }
    }
    objects.clear();
    goldBricks.clear();
    deletedKoopaSpawns.clear();
    if (blackBackground != nullptr)
    {
        delete blackBackground;
        blackBackground = nullptr;
    }
}

void CPlayScene::Unload()
{
    for (size_t i = 0; i < objects.size(); i++)
    {
        if (objects[i] != nullptr)
        {
            delete objects[i];
        }
    }

    if (hud != nullptr)
    {
        delete hud;
        hud = nullptr;
    }

    if (blackBackground != nullptr)
    {
        delete blackBackground;
        blackBackground = nullptr;
    }

    objects.clear();
    goldBricks.clear();
    deletedKoopaSpawns.clear();
    player = nullptr;
    DebugOut(L"[INFO] Scene %d unloaded! \n", id);
}

bool CPlayScene::IsGameObjectDeleted(const LPGAMEOBJECT& o)
{
    return o == nullptr || o->IsDeleted();
}

void CPlayScene::PurgeDeletedObjects()
{
    for (auto it = objects.begin(); it != objects.end(); ++it)
    {
        LPGAMEOBJECT o = *it;
        if (o != nullptr && o->IsDeleted())
        {
            delete o;
            *it = nullptr;
        }
    }

    objects.erase(std::remove_if(objects.begin(), objects.end(), IsGameObjectDeleted), objects.end());

    for (auto it = goldBricks.begin(); it != goldBricks.end(); )
    {
        if (*it && (*it)->IsDeleted())
        {
            it = goldBricks.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

void CPlayScene::AddObject(LPGAMEOBJECT obj)
{
    newObjects.push_back(obj);
    if (dynamic_cast<GoldBrick*>(obj))
    {
        goldBricks.push_back(obj);
    }
}