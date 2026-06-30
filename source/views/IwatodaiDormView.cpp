#include "IwatodaiDormView.h"
#include "core/globals.h"
#include "math.h"
#include <malloc.h>
#include <nds.h>
#include <stdio.h>
#include <string>

// model
#include "models/kotone.h"
#include "models/makoto.h"
// dialogue
#include "dialogue/demo_dialogue.h"
// maps
#include "maps/iwatodai_dorm_floor_1.h"

const unsigned int* loadEnvironmentBitmap(const std::string& path, GraphicAsset& asset)
{
    asset = graphicsCtrl.loadGrit(path);
    return reinterpret_cast<const unsigned int*>(asset.tiles);
}

void IwatodaiDormView::setMusic()
{
    musicCtrl.init((fatBasePath + "music/locations/iwatodaiDorm/iwatodai_dorm.pcm").c_str(), 1.831f, 65.907f);
}

// TODO: dont forget to clear in future
IwatodaiDormView::IwatodaiDormView()
{
}

void IwatodaiDormView::setupEnvironment()
{
    // setup environment model
    GraphicAsset envTextures[IWATODAI_DORM_FLOOR_1_TEX_COUNT] = {};
    const unsigned int* bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_COUNT] = {nullptr};

    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL02] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall02",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL02]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL03] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall03",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL03]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002DOOR02] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002door02",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002DOOR02]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002kzr01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR02] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002kzr02",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR02]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ04] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj04",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ04]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ07] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj07",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ07]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ11] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj11",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ11]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL04] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall04",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL04]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL05] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall05",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL05]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL06] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002wall06",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002WALL06]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002STEP01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002step01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002STEP01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002STEP02] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002step02",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002STEP02]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR03] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002kzr03",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR03]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR04] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002kzr04",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002KZR04]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ03] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj03",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ03]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ09] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj09",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ09]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002BOLT01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002bolt01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002BOLT01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002FLOOR01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002floor01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002FLOOR01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002FLOOR02] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002floor02",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002FLOOR02]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002FLOOR03] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002floor03",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002FLOOR03]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ02] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj02",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ02]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ05] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj05",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ05]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ15] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj15",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ15]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002BOLT03] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002bolt03",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002BOLT03]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ13] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj13",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ13]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ12] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj12",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ12]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002DOOR01] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002door01",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002DOOR01]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ10] =
        loadEnvironmentBitmap(fatBasePath + "environments/iwatodai_dorm_floor_1/f007_002obj10",
                              envTextures[IWATODAI_DORM_FLOOR_1_TEX_F007_002OBJ10]);
    bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_AKIHIKO] = loadEnvironmentBitmap(
        fatBasePath + "environments/iwatodai_dorm_floor_1/akihiko", envTextures[IWATODAI_DORM_FLOOR_1_TEX_AKIHIKO]);
    iwatodaiDormFloor1Env.load((fatBasePath + "environments/iwatodai_dorm_floor_1/iwatodai_dorm_floor_1.bin").c_str(),
                               bitmapsEnv);
    for (int i = 0; i < IWATODAI_DORM_FLOOR_1_TEX_COUNT; ++i)
    {
        graphicsCtrl.unloadGrit(envTextures[i]);
    }
}

void IwatodaiDormView::init()
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
    playerCtrl = new CharacterController(IWATODAI_DORM_FLOOR_1_MAP_WIDTH,
                                         IWATODAI_DORM_FLOOR_1_MAP_HEIGHT,
                                         &iwatodai_dorm_floor_1_map[0][0],
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
    IwatodaiDormView::setupEnvironment();

    // setup dialogue
    demo_dialogue_bg_slot = bgSharedSub1;

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
    uiCtrl.registerScreen(&dialogueScreen, false);
    uiCtrl.show(&menuHUDScreen, false);

    // setup view phases
    prevPauseState = false;
    prevDialogueState = false;
    prevEnvironmentState = false;
    phase = ViewPhase::Environment;
}

ViewState IwatodaiDormView::update()
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

    case ViewPhase::Dialogue:
    {
        bool isActive = dialogueCtrl.isActive();
        // set
        if (!isActive && !prevDialogueState)
        {
            uiCtrl.show(&dialogueScreen, false);
            demo_yukari_kenji_argument_load();
            dialogueCtrl.setLoader(demo_yukari_kenji_argument_load_bg);
            dialogueCtrl.start(demo_yukari_kenji_argument_first());
            prevDialogueState = true;
        }
        // exit
        else if (!isActive && prevDialogueState)
        {
            bgHide(bgSharedSub1);
            prevDialogueState = false;
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

        // check position
        switch (playerCtrl->isTileAt())
        {
        case TileType::SCENE_1:
            musicCtrl.pause();
            return ViewState::PAULOWNIA_MALL;
        case TileType::SCENE_0:
            musicCtrl.pause();
            return ViewState::IWATODAI_STREETS;
        case TileType::C_AK:
            // start dialogue
            iprintf("\x1b[0;0HTalk");
            if (pressed & KEY_A)
            {
                prevEnvironmentState = false;
                phase = ViewPhase::Dialogue;
            }
            break;
        default:
            consoleClear();
            break;
        }

        // update camera position
        gluLookAt(camPos.cameraX,
                  camPos.cameraY + 0.1f,
                  camPos.cameraZ,
                  camPos.targetX,
                  camPos.targetY,
                  camPos.targetZ,
                  camPos.upX,
                  camPos.upY,
                  camPos.upZ);

        // draw environment
        glPushMatrix();
        iwatodaiDormFloor1Env.draw();
        iwatodaiDormFloor1Env.drawBillboards(enableBillboards, // billboards face camera
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
            iprintf("\x1b[23;0Hangle(w,c): %d, %d \033[37;1m",
                    (int)(charPos.angle * 100),
                    (int)(charPos.facingAngle * 100));
        }
        break;
    }

    default:
    {
        phase = ViewPhase::Environment;
        break;
    }
    }

    dialogueCtrl.update(keys);
    characterAnimationCtrl.update();
    musicCtrl.update();

    return ViewState::KEEP_CURRENT;
}

void IwatodaiDormView::cleanup()
{
    BaseView::cleanup();

    // cleanup environment
    iwatodaiDormFloor1Env.cleanup();
    // reset ui
    uiCtrl.cleanup();

    // cleanup controllers
    delete playerCtrl;
    playerCtrl = NULL;
}
