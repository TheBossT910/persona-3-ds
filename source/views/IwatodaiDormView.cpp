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
    musicCtrl.init((fatBasePath + "music/locations/iwatodaiDorm/iwatodai_dorm.pcm").c_str(), 0.0f, -1.0f);
}

// TODO: dont forget to clear in future
IwatodaiDormView::IwatodaiDormView()
    : battleParticipants(new std::vector<BattleParticipant*>({&mercilessMaya, &cowardlyMaya})),
      battleController(battleParticipants, &characterProfiles, battleStartCondition)
{
}

void IwatodaiDormView::setupEnvironment()
{
    GraphicAsset envTextures[IWATODAI_DORM_FLOOR_1_TEX_COUNT] = {};
    const unsigned int* bitmapsEnv[IWATODAI_DORM_FLOOR_1_TEX_COUNT] = {nullptr};

    std::string folder = fatBasePath + "environments/iwatodai_dorm_floor_1/";

    // 1. Loop through the auto-generated filename array to load GRIT assets
    for (int i = 0; i < IWATODAI_DORM_FLOOR_1_TEX_COUNT; ++i)
    {
        bitmapsEnv[i] = loadEnvironmentBitmap(folder + iwatodai_dorm_floor_1_TextureFilenames[i], envTextures[i]);
    }

    // 2. Load the binary display list into the Environment
    iwatodaiDormFloor1Env.load((folder + "iwatodai_dorm_floor_1.bin").c_str(), bitmapsEnv);

    // 3. Unload the GRIT assets from main memory (now in VRAM)
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
    playerCtrl = new CharacterController(0,
                                         0,
                                         nullptr,
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
                                         false);

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

    // setup battle menu
    // TODO: check if isBattleMenuActive is just a dummy value
    battleMenuCmpt.init(-1, &isBattleMenuActive);

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
    prevBattleState = false;
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
    case ViewPhase::Battle:
    {
        bool isActive = battleController.isActive();
        // set
        if (!isActive && !prevBattleState)
        {
            // TODO: display battle menu UI
            uiCtrl.hideAll();
            battleController.execute();
            prevBattleState = true;
        }
        //exit
        else if (!isActive && prevBattleState)
        {
            IwatodaiDormView::setMusic();
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

        // start dialogue
        if (pressed & KEY_A)
        {
            prevEnvironmentState = false;
            phase = ViewPhase::Dialogue;
        }

        // start battle
        if (keys & KEY_Y)
        {
            prevEnvironmentState = false;
            phase = ViewPhase::Battle;
        }

        // check position
        if (playerCtrl->isTileAt() == TileType::SCENE_1)
        {
            musicCtrl.pause();
            return ViewState::PAULOWNIA_MALL;
        }
        else if (playerCtrl->isTileAt() == TileType::SCENE_0)
        {
            musicCtrl.pause();
            return ViewState::IWATODAI_STREETS;
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
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FOG | POLY_ID(0));
        iwatodaiDormFloor1Env.draw();
        // TODO: fix billboard support
        // iwatodaiDormFloor1Env.drawBillboards(enableBillboards, // billboards face camera
        //                                      camPos.cameraX,
        //                                      camPos.cameraY,
        //                                      camPos.cameraZ);
        glPopMatrix(1);

        // draw character
        glPushMatrix();
        // move character
        CharacterPosition charPos = playerCtrl->isCharacterAt();
        glTranslatef(charPos.x, charPos.y, charPos.z);
        glRotatef(charPos.facingAngle, 0.0f, 1.0f, 0.0f);

        // draw character
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FOG | POLY_ID(1));
        characterAnimationCtrl.render();
        glPopMatrix(1);

        glFlush(0);

        // print coordinates (64x64 area from 0,0 to 64,64)
        if (enableDebugPrint)
        {
            iprintf("\x1b[19;0H\033[31mTouch x = %04X, %04X\n", touch.rawx, touch.px);
            iprintf("\x1b[20;0H\033[31mTouch y = %04X, %04X\n", touch.rawy, touch.py);
            iprintf("\x1b[21;0H\033[31mtile(x,z): %d, %d",
                    (int)((charPos.x + worldOffsetX) / tileSize),
                    (int)((charPos.z + worldOffsetZ) / tileSize));
            iprintf("\x1b[22;0H\033[31mtranslate(x,z): %d, %d", (int)(charPos.x * 100), (int)(charPos.z * 100));
            iprintf(
                "\x1b[23;0H\033[31mangle(w,c): %d, %d", (int)(charPos.angle * 100), (int)(charPos.facingAngle * 100));
        }
        break;
    }

    default:
    {
        phase = ViewPhase::Environment;
        break;
    }
    }

    battleController.update(pressed);
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
