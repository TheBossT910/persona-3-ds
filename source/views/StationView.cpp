#include "StationView.h"
#include "core/globals.h"
#include "math.h"
#include <malloc.h>
#include <nds.h>
#include <stdio.h>

// map
#include "maps/station.h"
// environment
#include "environments/station.h"
#include <string>
// model
#include "models/kotone.h"
#include "models/makoto.h"

static const unsigned int* loadEnvironmentBitmap(const std::string& path, GraphicAsset& asset)
{
    asset = graphicsCtrl.loadGrit(path);
    return reinterpret_cast<const unsigned int*>(asset.tiles);
}

void StationView::setupEnvironment()
{
    GraphicAsset envTextures[STATION_TEX_COUNT] = {};
    const unsigned int* bitmaps[STATION_TEX_COUNT] = {nullptr};

    bitmaps[STATION_TEX_F008_005OBJ04] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005obj04",
                                                               envTextures[STATION_TEX_F008_005OBJ04]);
    bitmaps[STATION_TEX_F008_005OBJ02] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005obj02",
                                                               envTextures[STATION_TEX_F008_005OBJ02]);
    bitmaps[STATION_TEX_F008_005OBJ07] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005obj07",
                                                               envTextures[STATION_TEX_F008_005OBJ07]);
    bitmaps[STATION_TEX_F008_005OBJ06] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005obj06",
                                                               envTextures[STATION_TEX_F008_005OBJ06]);
    bitmaps[STATION_TEX_F008_005OBJ08] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005obj08",
                                                               envTextures[STATION_TEX_F008_005OBJ08]);
    bitmaps[STATION_TEX_LIGHTMAP_WHITE] = loadEnvironmentBitmap(fatBasePath + "environments/station/lightmap_white",
                                                                envTextures[STATION_TEX_LIGHTMAP_WHITE]);
    bitmaps[STATION_TEX_F008_005WALL01] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005wall01",
                                                                envTextures[STATION_TEX_F008_005WALL01]);
    bitmaps[STATION_TEX_F008_005OBJ01] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005obj01",
                                                               envTextures[STATION_TEX_F008_005OBJ01]);
    bitmaps[STATION_TEX_F008_005OBJ03] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005obj03",
                                                               envTextures[STATION_TEX_F008_005OBJ03]);
    bitmaps[STATION_TEX_F008_005FLOOR01] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005floor01",
                                                                 envTextures[STATION_TEX_F008_005FLOOR01]);
    bitmaps[STATION_TEX_F008_005BOLT01] = loadEnvironmentBitmap(fatBasePath + "environments/station/f008_005bolt01",
                                                                envTextures[STATION_TEX_F008_005BOLT01]);

    stationEnv.load((fatBasePath + "environments/station/station.bin").c_str(), bitmaps);
    for (int i = 0; i < STATION_TEX_COUNT; ++i)
    {
        graphicsCtrl.unloadGrit(envTextures[i]);
    }
}

void StationView::init()
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

    playerCtrl = new CharacterController(STATION_MAP_WIDTH,
                                         STATION_MAP_HEIGHT,
                                         &station_map[0][0],
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
    musicCtrl.init((fatBasePath + "music/locations/paulowniaMall/station/paulownia_mall.pcm").c_str(), 2.002f, 73.939f);

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
    StationView::setupEnvironment();

    // pause menu
    pauseMenuCmpt.init(bgSharedSub1, &isPauseMenuActive);

    // setup UI
    int bgMain[3] = {1, 2, 3};
    int bgSub[4] = {bgSharedSub2, bgSharedSub3, 2, 3};

    // initialize sub sprite engine with 1D mapping, 128 byte boundry, external palette support
    oamInit(&oamSub, SpriteMapping_1D_128, true);

    uiCtrl.setGraphics(bgSub, bgMain, &oamSub, nullptr);
    uiCtrl.registerScreen(&menuHUDScreen, false);
    uiCtrl.show(&menuHUDScreen, false);

    // setup view phases
    prevPauseState = false;
    prevEnvironmentState = false;
    phase = ViewPhase::Environment;
}

ViewState StationView::update()
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

        if (playerCtrl->isTileAt() == TileType::SCENE_0)
        {
            musicCtrl.pause();
            return ViewState::PAULOWNIA_MALL;
        }

        gluLookAt(camPos.cameraX,
                  camPos.cameraY,
                  camPos.cameraZ,
                  camPos.targetX,
                  camPos.targetY,
                  camPos.targetZ,
                  camPos.upX,
                  camPos.upY,
                  camPos.upZ);

        glPushMatrix();
        stationEnv.draw();
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
            iprintf("\x1b[22;0Hmtranslate(x,z): %d, %d", (int)(charPos.x * 100), (int)(charPos.z * 100));
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
    characterAnimationCtrl.update();

    return ViewState::KEEP_CURRENT;
}

void StationView::cleanup()
{
    BaseView::cleanup();

    stationEnv.cleanup();
    uiCtrl.cleanup();

    delete playerCtrl;
    playerCtrl = nullptr;
}
