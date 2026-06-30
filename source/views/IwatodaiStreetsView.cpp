#include "IwatodaiStreetsView.h"
#include "core/globals.h"
#include "math.h"
#include <malloc.h>
#include <nds.h>
#include <stdio.h>

// map
#include "maps/iwatodai_streets.h"
// environment
#include "environments/iwatodai_streets.h"
#include <string>
// model
#include "models/kotone.h"
#include "models/makoto.h"

static const unsigned int* loadEnvironmentBitmap(const std::string& path, GraphicAsset& asset)
{
    asset = graphicsCtrl.loadGrit(path);
    return reinterpret_cast<const unsigned int*>(asset.tiles);
}

void IwatodaiStreetsView::setMusic()
{
    std::string streetsMusicPath;
    float start;
    float end;
    if (saveData.femcMode)
    {
        start = 34.001f;
        end = 93.032f;
        streetsMusicPath = "music/locations/iwatodaiStreets/sun.pcm";
    }
    else
    {
        start = 31.0f;
        end = 177.587f;
        streetsMusicPath = "music/locations/iwatodaiStreets/changing_seasons.pcm";
    }

    musicCtrl.init((fatBasePath + streetsMusicPath).c_str(), start, end);
}

void IwatodaiStreetsView::setupEnvironment()
{
    GraphicAsset envTextures[IWATODAI_STREETS_TEX_COUNT] = {};
    const unsigned int* bitmaps[IWATODAI_STREETS_TEX_COUNT] = {nullptr};

    bitmaps[IWATODAI_STREETS_TEX_F007_009_07] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_07", envTextures[IWATODAI_STREETS_TEX_F007_009_07]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_16] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_16", envTextures[IWATODAI_STREETS_TEX_F007_009_16]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_30] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_30", envTextures[IWATODAI_STREETS_TEX_F007_009_30]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_25] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_25", envTextures[IWATODAI_STREETS_TEX_F007_009_25]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_08] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_08", envTextures[IWATODAI_STREETS_TEX_F007_009_08]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_05] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_05", envTextures[IWATODAI_STREETS_TEX_F007_009_05]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_02] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_02", envTextures[IWATODAI_STREETS_TEX_F007_009_02]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_04] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_04", envTextures[IWATODAI_STREETS_TEX_F007_009_04]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_28] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_28", envTextures[IWATODAI_STREETS_TEX_F007_009_28]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_24] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_24", envTextures[IWATODAI_STREETS_TEX_F007_009_24]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_09] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_09", envTextures[IWATODAI_STREETS_TEX_F007_009_09]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_11] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_11", envTextures[IWATODAI_STREETS_TEX_F007_009_11]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_10] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_10", envTextures[IWATODAI_STREETS_TEX_F007_009_10]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_13] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_13", envTextures[IWATODAI_STREETS_TEX_F007_009_13]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_12] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_12", envTextures[IWATODAI_STREETS_TEX_F007_009_12]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_17] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_17", envTextures[IWATODAI_STREETS_TEX_F007_009_17]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_18] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_18", envTextures[IWATODAI_STREETS_TEX_F007_009_18]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_27] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_27", envTextures[IWATODAI_STREETS_TEX_F007_009_27]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_03] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_03", envTextures[IWATODAI_STREETS_TEX_F007_009_03]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_01] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_01", envTextures[IWATODAI_STREETS_TEX_F007_009_01]);
    bitmaps[IWATODAI_STREETS_TEX_F007_009_23] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_streets/f007_009_23", envTextures[IWATODAI_STREETS_TEX_F007_009_23]);
    bitmaps[IWATODAI_STREETS_TEX_ENEMY] = loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_streets/enemy",
                                                                envTextures[IWATODAI_STREETS_TEX_ENEMY]);
    iwatodaiStreetsEnv.load((fatBasePath + "environments/iwatodai_streets/iwatodai_streets.bin").c_str(), bitmaps);
    for (int i = 0; i < IWATODAI_STREETS_TEX_COUNT; ++i)
    {
        graphicsCtrl.unloadGrit(envTextures[i]);
    }
}

void IwatodaiStreetsView::init()
{
    BaseView3D::init();

    // setup sub screen
    bgSharedSub1 = bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 2, 1);
    bgSharedSub2 = bgInitSub(2, BgType_Text8bpp, BgSize_T_256x256, 3, 3);
    bgSharedSub3 = bgInitSub(3, BgType_Text8bpp, BgSize_T_256x256, 4, 5);

    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub1), 2048);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub2), 2048);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub3), 2048);

    // setup console
    consoleInit(&console, 1, BgType_Text4bpp, BgSize_T_256x256, 5, 0, false, true);
    consoleSelect(&console);

    bgSetPriority(console.bgId, 0);
    bgSetPriority(bgSharedSub1, 1);
    bgSetPriority(bgSharedSub2, 2);
    bgSetPriority(bgSharedSub3, 3);
    bgUpdate();

    playerCtrl = new CharacterController(IWATODAI_STREETS_MAP_WIDTH,
                                         IWATODAI_STREETS_MAP_HEIGHT,
                                         &iwatodai_streets_map[0][0],
                                         tileSize,
                                         worldOffsetX,
                                         worldOffsetZ,
                                         characterSize,
                                         speed,
                                         angleIncrement,
                                         distance,
                                         lookAhead,
                                         angle,
                                         height,
                                         characterTranslate,
                                         characterFacingAngle,
                                         true);

    // setup music
    setMusic();

    // setup character model
    std::string modelPath = fatBasePath + "models/";
    characterAnimationCtrl.loadModel(
        (modelPath + (saveData.femcMode ? "kotone/kotone.bin" : "makoto/makoto.bin")).c_str());

    if (saveData.femcMode)
    {
        kotone_loadTextures(characterAnimationCtrl, (const unsigned int**)bitmapsCharacter);
    }
    else
    {
        makoto_loadTextures(characterAnimationCtrl, (const unsigned int**)bitmapsCharacter);
    }

    // setup environment
    IwatodaiStreetsView::setupEnvironment();

    // pause menu
    pauseMenuCmpt.init(bgSharedSub1, &isPauseMenuActive);

    // setup battle menu
    // TODO: check if isBattleMenuActive is just a dummy value
    battleMenuCmpt.init(-1, &isBattleMenuActive);

    // setup UI
    int bgMain[3] = {1, 2, 3};
    int bgSub[4] = {bgSharedSub2, bgSharedSub3, 2, 3};

    // initialize sub sprite engine with 1D mapping, 128 byte boundry, external palette support
    oamInit(&oamSub, SpriteMapping_1D_128, true);

    uiCtrl.setGraphics(bgSub, bgMain, &oamSub, nullptr);
    uiCtrl.registerScreen(&menuHUDScreen, false);
    uiCtrl.show(&menuHUDScreen, false);

    // setup view phases
    prevBattleState = false;
    prevPauseState = false;
    prevEnvironmentState = false;
    phase = ViewPhase::Environment;
}

ViewState IwatodaiStreetsView::update()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    bgUpdate();
    oamUpdate(&oamSub);

    scanKeys();
    u32 keys = keysHeld();
    u32 pressed = keysDown();

    switch (phase)
    {
    case ViewPhase::Battle:
    {
        bool isActive = battleController.isActive();
        // set
        if (!isActive && !prevBattleState)
        {
            // TODO: display battle menu UI
            uiCtrl.hideAll();
            battleController.execute(player, &partyMembers, &enemies, &battleParticipants, battleStartCondition);
            prevBattleState = true;
        }
        //exit
        else if (!isActive && prevBattleState)
        {
            //battle cleanup
            for (BattleParticipant* participant : battleParticipants)
            {
                delete participant;
            }

            battleParticipants.clear();
            enemies.clear();
            partyMembers.clear();

            //Reset battle
            mercilessMaya = new Enemy(EnemyDb::mercilessMaya);
            cowardlyMaya = new Enemy(EnemyDb::cowardlyMaya);
            player = new Player(CharacterProfileDb::player);
            yukari = new PartyMember(CharacterProfileDb::yukari);
            junpei = new PartyMember(CharacterProfileDb::junpei);

            battleParticipants = {mercilessMaya, cowardlyMaya, player, yukari, junpei};
            enemies = {mercilessMaya, cowardlyMaya};
            partyMembers = {player, yukari, junpei};

            IwatodaiStreetsView::setMusic();
            prevBattleState = false;

            phase = ViewPhase::Environment;
        }
        break;
    }
    case ViewPhase::Pause:
    {
        // set
        if (!prevPauseState)
        {
            // TODO: display pause menu UI
            uiCtrl.hideAll();
            prevPauseState = true;
        }

        // run
        ViewState menuResult = pauseMenuCmpt.update(pressed);
        if (menuResult != ViewState::KEEP_CURRENT)
        {
            musicCtrl.pause();
            return menuResult;
        }

        // exit
        if (pressed & KEY_START)
        {
            consoleClear();
            prevPauseState = false;
            phase = ViewPhase::Environment;
        }
        break;
    }

    case ViewPhase::Environment:
    {
        if (!prevEnvironmentState)
        {
            // render HUD
            uiCtrl.show(&menuHUDScreen, false);
            prevEnvironmentState = true;
        }

        camPos = playerCtrl->update(keys);

        // start pause menu
        if (pressed & KEY_START)
        {
            prevEnvironmentState = false;
            phase = ViewPhase::Pause;
        }

        // start pause menu
        if (pressed & KEY_TOUCH)
        {
            touchRead(&touch);
            if (menuHUDScreen.onTouch(&touch) == 1)
            {
                prevEnvironmentState = false;
                phase = ViewPhase::Pause;
            }
        }

        switch (playerCtrl->isTileAt())
        {
        case TileType::SCENE_0:
            musicCtrl.pause();
            return ViewState::IWATODAI_DORM;
        case TileType::SCENE_1:
            musicCtrl.pause();
            return ViewState::PAULOWNIA_MALL;
        case TileType::SCENE_2:
            musicCtrl.pause();
            return ViewState::STATION;
        case TileType::SHD_W:
            // start battle
            iprintf("\x1b[0;0HStart Battle");
            if (pressed & KEY_A)
            {
                prevEnvironmentState = false;
                phase = ViewPhase::Battle;
            }
            break;
        default:
            consoleClear();
            break;
        }

        gluLookAt(camPos.cameraX,
                  camPos.cameraY + 0.3f,
                  camPos.cameraZ,
                  camPos.targetX,
                  camPos.targetY,
                  camPos.targetZ,
                  camPos.upX,
                  camPos.upY,
                  camPos.upZ);

        glPushMatrix();
        iwatodaiStreetsEnv.draw();
        iwatodaiStreetsEnv.drawBillboards(enableBillboards, // billboards face camera
                                          camPos.cameraX,
                                          camPos.cameraY,
                                          camPos.cameraZ);
        glPopMatrix(1);

        glPushMatrix();
        CharacterPosition charPos = playerCtrl->isCharacterAt();
        glTranslatef(charPos.x, charPos.y, charPos.z);
        glRotatef(charPos.facingAngle, 0.0f, 1.0f, 0.0f);
        characterAnimationCtrl.render();
        glPopMatrix(1);

        glFlush(0);

        // print coordinates (64x64 area from 0,0 to 64,64)
        if (enableDebugPrint)
        {
            iprintf("\x1b[19;0H\033[31mTouch x = %04X, %04X\n", touch.rawx, touch.px);
            iprintf("\x1b[20;0HTouch y = %04X, %04X\n", touch.rawy, touch.py);
            iprintf("\x1b[21;0Htile(x,z): %d, %d",
                    (int)((charPos.x + worldOffsetX) / tileSize),
                    (int)((charPos.z + worldOffsetZ) / tileSize));
            iprintf("\x1b[22;0Htranslate(x,z): %d, %d", (int)(charPos.x * 100), (int)(charPos.z * 100));
            iprintf(
                "\x1b[23;0Hangle(w,c): %d, %d\033[37;1m", (int)(charPos.angle * 100), (int)(charPos.facingAngle * 100));
        }
        break;
    }

    default:
    {
        phase = ViewPhase::Environment;
        break;
    }
    }

    musicCtrl.update();
    battleController.update(pressed);
    characterAnimationCtrl.update();

    return ViewState::KEEP_CURRENT;
}

void IwatodaiStreetsView::cleanup()
{
    BaseView::cleanup();

    iwatodaiStreetsEnv.cleanup();
    uiCtrl.cleanup();

    delete playerCtrl;
    playerCtrl = nullptr;
}
