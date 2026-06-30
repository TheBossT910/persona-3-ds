#include "PaulowniaMallView.h"
#include "core/globals.h"
#include "math.h"
#include <malloc.h>
#include <nds.h>
#include <stdio.h>
#include <string>

// model
#include "models/kotone.h"
#include "models/makoto.h"
// map
#include "maps/paulownia_mall.h"

static const unsigned int* loadEnvironmentBitmap(const std::string& path, GraphicAsset& asset)
{
    asset = graphicsCtrl.loadGrit(path);
    return reinterpret_cast<const unsigned int*>(asset.tiles);
}

void PaulowniaMallView::setMusic()
{
    musicCtrl.init(
        (fatBasePath + "music/locations/paulowniaMall/overworld/color_your_night.pcm").c_str(), 2.050f, 204.191f);
}

PaulowniaMallView::PaulowniaMallView()
{
}

void PaulowniaMallView::setupEnvironment()
{
    GraphicAsset envTextures[PAULOWNIA_MALL_TEX_COUNT] = {};
    const unsigned int* bitmapsEnv[PAULOWNIA_MALL_TEX_COUNT] = {nullptr};

    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_FL02] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_fl02", envTextures[PAULOWNIA_MALL_TEX_F008_001_FL02]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_FL03] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_fl03", envTextures[PAULOWNIA_MALL_TEX_F008_001_FL03]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_GSFL] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_gsfl", envTextures[PAULOWNIA_MALL_TEX_F008_001_GSFL]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_HASIRA] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_hasira", envTextures[PAULOWNIA_MALL_TEX_F008_001_HASIRA]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KAIDAN] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_kaidan", envTextures[PAULOWNIA_MALL_TEX_F008_001_KAIDAN]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_SKY] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_sky", envTextures[PAULOWNIA_MALL_TEX_F008_001_SKY]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_WL01] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_wl01", envTextures[PAULOWNIA_MALL_TEX_F008_001_WL01]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_WL02] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_wl02", envTextures[PAULOWNIA_MALL_TEX_F008_001_WL02]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KOMONO01] =
        loadEnvironmentBitmap(fatBasePath + "environments/paulownia_mall/f008_001_komono01",
                              envTextures[PAULOWNIA_MALL_TEX_F008_001_KOMONO01]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_FUN03] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_fun03", envTextures[PAULOWNIA_MALL_TEX_F008_001_FUN03]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KOMONO02] =
        loadEnvironmentBitmap(fatBasePath + "environments/paulownia_mall/f008_001_komono02",
                              envTextures[PAULOWNIA_MALL_TEX_F008_001_KOMONO02]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_LIGFLA] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_ligfla", envTextures[PAULOWNIA_MALL_TEX_F008_001_LIGFLA]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_ENTER02] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_enter02", envTextures[PAULOWNIA_MALL_TEX_F008_001_ENTER02]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_PLANTA] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_plantA", envTextures[PAULOWNIA_MALL_TEX_F008_001_PLANTA]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KEIMARK] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_keimark", envTextures[PAULOWNIA_MALL_TEX_F008_001_KEIMARK]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_JYUTAN] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_jyutan", envTextures[PAULOWNIA_MALL_TEX_F008_001_JYUTAN]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_FL01] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_fl01", envTextures[PAULOWNIA_MALL_TEX_F008_001_FL01]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_GRA02] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_gra02", envTextures[PAULOWNIA_MALL_TEX_F008_001_GRA02]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_HIHO] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_hiho", envTextures[PAULOWNIA_MALL_TEX_F008_001_HIHO]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KANBAN03] =
        loadEnvironmentBitmap(fatBasePath + "environments/paulownia_mall/f008_001_kanban03",
                              envTextures[PAULOWNIA_MALL_TEX_F008_001_KANBAN03]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KANBAN] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_kanban", envTextures[PAULOWNIA_MALL_TEX_F008_001_KANBAN]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_FUN01] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_fun01", envTextures[PAULOWNIA_MALL_TEX_F008_001_FUN01]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_FUN02] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_fun02", envTextures[PAULOWNIA_MALL_TEX_F008_001_FUN02]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_ACYANE] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_acyane", envTextures[PAULOWNIA_MALL_TEX_F008_001_ACYANE]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_ENTER01] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_enter01", envTextures[PAULOWNIA_MALL_TEX_F008_001_ENTER01]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KAR] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_kar", envTextures[PAULOWNIA_MALL_TEX_F008_001_KAR]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_GLASS] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_glass", envTextures[PAULOWNIA_MALL_TEX_F008_001_GLASS]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_CHAIR] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_chair", envTextures[PAULOWNIA_MALL_TEX_F008_001_CHAIR]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_GAFL] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_gafl", envTextures[PAULOWNIA_MALL_TEX_F008_001_GAFL]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_CD] = loadEnvironmentBitmap(
        fatBasePath + "environments/paulownia_mall/f008_001_cd", envTextures[PAULOWNIA_MALL_TEX_F008_001_CD]);
    bitmapsEnv[PAULOWNIA_MALL_TEX_F008_001_KANBAN02] =
        loadEnvironmentBitmap(fatBasePath + "environments/paulownia_mall/f008_001_kanban02",
                              envTextures[PAULOWNIA_MALL_TEX_F008_001_KANBAN02]);
    paulowniaMallEnv.load((fatBasePath + "environments/paulownia_mall/paulownia_mall.bin").c_str(), bitmapsEnv);
    for (int i = 0; i < PAULOWNIA_MALL_TEX_COUNT; ++i)
    {
        graphicsCtrl.unloadGrit(envTextures[i]);
    }
}

void PaulowniaMallView::init()
{
    BaseView3D::init();

    // setup sub screen
    // https://mtheall.com/vram.html#SUB=1&T0=1&NT0=512&MB0=2&TB0=1&S0=0&T1=3&NT1=128&MB1=5&TB1=0&T2=1&NT2=512&MB2=3&TB2=3&S2=0&T3=1&NT3=512&MB3=4&TB3=5&S3=0
    bgSharedSub1 = bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 2, 1);
    bgSharedSub2 = bgInitSub(2, BgType_Text8bpp, BgSize_T_256x256, 3, 3);
    bgSharedSub3 = bgInitSub(3, BgType_Text8bpp, BgSize_T_256x256, 4, 5);

    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub1), 2048);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub2), 2048);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub3), 2048);

    // setup console
    consoleInit(&console, 1, BgType_Text4bpp, BgSize_T_256x256, 5, 0, false, true);
    consoleSelect(&console);

    // adjust sub screen image and console to sit correctly on each other
    bgSetPriority(console.bgId, 0);
    bgSetPriority(bgSharedSub1, 1);
    bgSetPriority(bgSharedSub2, 2);
    bgSetPriority(bgSharedSub3, 3);
    bgUpdate();

    // setup player controller
    playerCtrl = new CharacterController(PAULOWNIA_MALL_MAP_WIDTH,
                                         PAULOWNIA_MALL_MAP_HEIGHT,
                                         &paulownia_mall_map[0][0],
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
    PaulowniaMallView::setupEnvironment();

    // setup pause menu
    pauseMenuCmpt.init(bgSharedSub1, &isPauseMenuActive);

    // setup UI
    // NOTE: bg 0 is the 3D view
    int bgMain[3] = {1, 2, 3};
    // TODO: Setting the first index to anything other than bgSharedSub results in black bg (but sprites still load)
    // This might be okay/intended, as long as we create 4 seperate bg to pass in
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

ViewState PaulowniaMallView::update()
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

        // move character
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
        // left
        case TileType::SCENE_0:
            musicCtrl.pause();
            return ViewState::IWATODAI_STREETS;
        // right
        case TileType::SCENE_1:
            musicCtrl.pause();
            return ViewState::IWATODAI_DORM;
        // middle
        case TileType::SCENE_2:
        case TileType::SCENE_3:
        case TileType::SCENE_4:
        case TileType::SCENE_5:
        case TileType::SCENE_6:
        case TileType::SCENE_7:
        case TileType::SCENE_8:
        case TileType::SCENE_9:
            musicCtrl.pause();
            return ViewState::STATION;
        default:
            break;
        }

        // update camera position
        gluLookAt(camPos.cameraX,
                  camPos.cameraY + 0.3f,
                  camPos.cameraZ,
                  camPos.targetX,
                  camPos.targetY,
                  camPos.targetZ,
                  camPos.upX,
                  camPos.upY,
                  camPos.upZ);

        // draw environment
        glPushMatrix();
        paulowniaMallEnv.draw();
        paulowniaMallEnv.drawBillboards(enableBillboards, // billboards face camera
                                        camPos.cameraX,
                                        camPos.cameraY,
                                        camPos.cameraZ);
        glPopMatrix(1);

        // draw character
        glPushMatrix();
        // move character
        CharacterPosition charPos = playerCtrl->isCharacterAt();
        glTranslatef(charPos.x, charPos.y, charPos.z);
        glRotatef(charPos.facingAngle, 0.0f, 1.0f, 0.0f);

        // draw character
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

    // update controllers
    characterAnimationCtrl.update();
    musicCtrl.update();

    return ViewState::KEEP_CURRENT;
}

void PaulowniaMallView::cleanup()
{
    BaseView::cleanup();

    paulowniaMallEnv.cleanup();
    uiCtrl.cleanup();

    delete playerCtrl;
    playerCtrl = NULL;
}
