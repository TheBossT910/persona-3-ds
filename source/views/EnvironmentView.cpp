#include "EnvironmentView.h"
#include "core/globals.h"
#include <nds.h>
#include <string>

// model
#include "models/kotone.h"
#include "models/makoto.h"

namespace
{
/**
 * @brief Loads a single .grit asset and returns its raw tile pointer.
 *
 * Stashes the owning GraphicAsset in @p asset so the caller can unload it
 * once the texture has been uploaded to VRAM.
 *
 * @param path  Full path (base path + grit base name) of the asset to load.
 * @param asset Output parameter that receives the loaded GraphicAsset,
 *              which the caller is responsible for unloading later.
 * @return Raw pointer to the asset's tile data, reinterpreted as
 *         unsigned int, suitable for passing to the texture upload code.
 */
const unsigned int* loadEnvironmentBitmap(const std::string& path, GraphicAsset& asset)
{
    asset = GraphicsController::getInstance()->loadGrit(path);
    return reinterpret_cast<const unsigned int*>(asset.tiles);
}

/**
 * @brief Strips the compiled ".img.bin" suffix from a texture filename to
 *        recover the base name expected by loadGrit.
 *
 * environmentDb.cpp stores the *compiled* texture filename, e.g.
 * "f007_002wall01.img.bin", but loadGrit wants the .grit base name instead
 * (e.g. "f007_002wall01").
 *
 * @param compiledFileName The compiled texture filename as stored in the
 *                          environment database (e.g. "name.img.bin").
 * @return The same name with a trailing ".img.bin" suffix removed, or the
 *         name unchanged if it does not end with that suffix.
 */
std::string gritBaseName(const char* compiledFileName)
{
    std::string name(compiledFileName);
    static const std::string suffix = ".img.bin";
    if (name.size() > suffix.size() && name.compare(name.size() - suffix.size(), suffix.size(), suffix) == 0)
    {
        name.erase(name.size() - suffix.size());
    }
    return name;
}
} // namespace

void EnvironmentView::setupEnvironment()
{
    GraphicAsset envTextures[MAX_ENVIRONMENT_TEXTURES] = {};
    std::array<const unsigned int*, MAX_ENVIRONMENT_TEXTURES> bitmapsEnv = {nullptr};

    const std::string basePath = fatBasePath + "environments/" + dbEntry->name + "/";

    for (int i = 0; i < dbEntry->textureCount; ++i)
    {
        bitmapsEnv[i] = loadEnvironmentBitmap(basePath + gritBaseName(dbEntry->textures[i].name), envTextures[i]);
    }

    if (!env.load(dbEntry, bitmapsEnv))
    {
        textCtrl->drawText("EnvironmentView: failed to load environment " + std::string(dbEntry->name),
                           cosmeticaFont,
                           textVideoBufferSub,
                           0,
                           0,
                           TextColor::Red);
    }

    for (int i = 0; i < dbEntry->textureCount; ++i)
    {
        graphicsCtrl->unloadGrit(envTextures[i]);
    }
}

void EnvironmentView::init()
{
    // set modes
    videoSetMode(MODE_5_3D | DISPLAY_BG3_ACTIVE);
    videoSetModeSub(MODE_3_2D | DISPLAY_BG3_ACTIVE);

    // set vram
    vramSetBankA(VRAM_A_TEXTURE_SLOT0); // texture slot 0
    vramSetBankB(VRAM_B_TEXTURE_SLOT1); // texture slot 1

    vramSetBankC(VRAM_C_SUB_BG);
    vramSetBankD(VRAM_D_SUB_SPRITE);
    vramSetBankE(VRAM_E_MAIN_BG);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);
    vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);
    bgExtPaletteEnableSub();

    // 3D init
    glInit();
    glEnable(GL_ANTIALIAS);  // cleans up edges
    glEnable(GL_TEXTURE_2D); // for textures
    // glEnable(GL_BLEND);      // useful for UI
    glEnable(GL_FOG);     // fog effect
    glEnable(GL_OUTLINE); // stylistic outline

    glClearColor(0, 0, 0, 31);
    glClearPolyID(63);
    glClearDepth(0x7FFF);

    // viewport
    glViewport(0, 0, 255, 191);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // zNear is how close the camera can see, zFar is the maximum draw distance
    gluPerspective(55, 256.0 / 192.0, 0.1, 40);

    // outline
    glSetOutlineColor(0, RGB15(0, 0, 0));

    // fog
    // setup color
    glFogColor(22, 25, 28, 31); // daytime blue
    // glFogColor(30, 25, 16, 31);  // evening orange
    // glFogColor(16, 17, 19, 31);  // rainy gray

    // how much depth difference there is between table entries
    glFogShift(shift);
    // depth at which the fog starts (and the table starts applying)
    glFogOffset(depth);

    // generate a linear density table
    int density = 0;
    for (int i = 0; i < 32; i++) // it has 32 steps
    {
        glFogDensity(i, density);
        // exponentially increase mass the furthur back the fog is
        density += (mass * i) >> 2;

        // entries are 7 bit, so cap the density to 127
        if (density > 127)
            density = 127;
    }

    glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FOG);
    glColor3b(255, 255, 255);

    dbEntry = getEnvironmentDbEntry();
    if (!dbEntry)
    {
        sassert(false, "EnvironmentView::init - no EnvironmentDbEntry for this room");
        return;
    }

    // setup sub screen
    // https://mtheall.com/vram.html#SUB=1&T0=1&NT0=512&MB0=2&TB0=1&S0=0&T1=3&NT1=128&MB1=5&TB1=0&T2=1&NT2=512&MB2=3&TB2=3&S2=0&T3=1&NT3=512&MB3=4&TB3=5&S3=0
    bgSharedSub1 = bgInitSub(0, BgType_Text8bpp, BgSize_T_256x256, 0, 1);
    bgSharedSub2 = bgInitSub(2, BgType_Text8bpp, BgSize_T_256x256, 2, 2);
    bgSharedSub3 = bgInitSub(1, BgType_Text8bpp, BgSize_T_256x256, 4, 3);

    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub1), 2048);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub2), 2048);
    dmaFillHalfWords(0, bgGetMapPtr(bgSharedSub3), 2048);

    // adjust sub screen image and console to sit correctly on each other
    bgSetPriority(bgSharedSub1, 1);
    bgSetPriority(bgSharedSub2, 2);
    bgSetPriority(bgSharedSub3, 3);
    bgUpdate();

    // setup player controller (room-specific map/tuning, generic call site)
    playerCtrl = createPlayerController();

    configureCameraController();
    cameraCtrl.configure(camConfig);

    // setup character model (identical across rooms)
    std::string modelPath = fatBasePath + "models/";
    characterAnimationCtrl->loadModel(
        (modelPath + (saveData.femcMode ? "kotone/kotone.bin" : "makoto/makoto.bin")).c_str());

    if (saveData.femcMode)
    {
        kotone_loadTextures(*characterAnimationCtrl, (const unsigned int**)bitmapsCharacter);
    }
    else
    {
        makoto_loadTextures(*characterAnimationCtrl, (const unsigned int**)bitmapsCharacter);
    }

    //setup main screen text engine
    int bgText = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);
    textVideoBuffer = (uint16_t*)bgGetGfxPtr(bgText);
    bgSetPriority(bgText, 0); //set text layer on main to be on top of 3D view

    //setup sub screen text engine
    int bgTextSub = bgInitSub(3, BgType_Bmp8, BgSize_B8_256x256, 4, 0);
    textVideoBufferSub = (uint16_t*)bgGetGfxPtr(bgTextSub);
    bgSetPriority(bgTextSub, 0);

    cosmeticaFont = textCtrl->loadFont("cosmetica", 12);
    textCtrl->loadDefaultPalette();

    // setup environment geometry/textures (fully generic, data-driven)
    setupEnvironment();

    // setup dialogue rendering target (which sub-bg the dialogue box uses)
    demo_dialogue_bg_slot = bgSharedSub1;

    // setup pause menu
    pauseMenuCmpt->init(bgSharedSub1, &Globals::isPauseMenuActive, textVideoBuffer, textVideoBufferSub);
    pauseMenuCmpt->setCameraController(&cameraCtrl);

    // setup battle menu
    battleMenuCmpt->init(-1, &isBattleMenuActive, textVideoBuffer, textVideoBufferSub);

    // setup UI
    // NOTE: bg 0 is the 3D view
    int bgMain[3] = {1, 2, 3};
    // TODO: Setting the first index to anything other than bgSharedSub results in black bg (but sprites still load)
    // This might be okay/intended, as long as we create 4 seperate bg to pass in
    int bgSub[4] = {bgSharedSub2, bgSharedSub3, 2, 3};

    // initialize sub sprite engine with 1D mapping, 128 byte boundry, external palette support
    oamInit(&oamSub, SpriteMapping_1D_128, true);

    uiCtrl->setGraphics(bgSub, bgMain, &oamSub, nullptr);
    uiCtrl->registerScreen(menuHUDScreen, false);
    uiCtrl->registerScreen(dialogueScreen, false);
    uiCtrl->show(menuHUDScreen, false);

    // setup music (room-specific path/loop points)
    setMusic();

    onSetupDialogueAndUI();

    // setup view phases
    prevPauseState = false;
    prevDialogueState = false;
    prevEnvironmentState = false;
    isBattleMenuActive = false;
    prevBattleState = false;
    phase = ViewPhase::Environment;

    bgSetPriority(0, 2); //set 3D view on main to be behind text layer
}

ViewState EnvironmentView::update()
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
        if (!prevBattleState)
        {
            uiCtrl->hideAll();

            startBattle();

            prevBattleState = true;
        }

        battleController->update(pressed);

        if (!battleController->isActive() && prevBattleState)
        {
            prevBattleState = false;

            uiCtrl->show(menuHUDScreen, false);

            prevEnvironmentState = true;
            phase = ViewPhase::Environment;

            setMusic();
        }

        break;
    }

    case ViewPhase::Pause:
    {
        if (!prevPauseState)
        {
            uiCtrl->hideAll();
            pauseMenuCmpt->reset();
            prevPauseState = true;
        }

        ViewState menuResult = pauseMenuCmpt->update(pressed);

        if (menuResult != ViewState::KEEP_CURRENT)
        {
            musicCtrl->pause();
            return menuResult;
        }

        if (pressed & KEY_START)
        {
            textCtrl->clearScreen(textVideoBufferSub);
            prevPauseState = false;
            phase = ViewPhase::Environment;
            prevEnvironmentState = false;
        }

        break;
    }

    case ViewPhase::Dialogue:
    {
        bool isActive = dialogueCtrl.isActive();

        if (!isActive && !prevDialogueState)
        {
            uiCtrl->show(dialogueScreen, false);

            onDialogueStart();

            prevDialogueState = true;
        }
        else if (!isActive && prevDialogueState)
        {
            bgHide(bgSharedSub1);

            prevDialogueState = false;
            prevEnvironmentState = false;

            phase = ViewPhase::Environment;
        }

        dialogueCtrl.update(keys);

        break;
    }

    case ViewPhase::Environment:
    {
        if (!prevEnvironmentState)
        {
            uiCtrl->show(menuHUDScreen, false);
            prevEnvironmentState = true;
        }

        playerCtrl->update(keys, &cameraCtrl);
        CharacterPosition charPos = playerCtrl->isCharacterAt();
        camPos = cameraCtrl.update(keys, charPos);

        if (pressed & KEY_START)
        {
            textCtrl->clearScreen(textVideoBufferSub);
            prevEnvironmentState = false;
            phase = ViewPhase::Pause;
            break;
        }

        if (pressed & KEY_TOUCH)
        {
            touchRead(&touch);

            if (menuHUDScreen->onTouch(&touch) == 1)
            {
                prevEnvironmentState = false;
                phase = ViewPhase::Pause;
                break;
            }
        }

        ViewState tileResult = onTileCheck(playerCtrl->isTileAt(), pressed);

        if (tileResult != ViewState::KEEP_CURRENT)
        {
            musicCtrl->pause();
            return tileResult;
        }

        gluLookAt(camPos.eye.x,
                  camPos.eye.y + getCameraYOffset(),
                  camPos.eye.z,
                  camPos.target.x,
                  camPos.target.y,
                  camPos.target.z,
                  camPos.up.x,
                  camPos.up.y,
                  camPos.up.z);

        // environment
        glPushMatrix();
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FOG | POLY_ID(0));
        env.draw();
        env.drawBillboards(Globals::enableBillboards, camPos.eye.x, camPos.eye.y, camPos.eye.z);
        glPopMatrix(1);

        // model
        glPushMatrix();

        glTranslatef(charPos.x, charPos.y, charPos.z);
        glRotatef(charPos.facingAngle, 0.0f, 1.0f, 0.0f);
        glPolyFmt(POLY_ALPHA(31) | POLY_CULL_BACK | POLY_FOG | POLY_ID(1));
        characterAnimationCtrl->render();
        glPopMatrix(1);

        glFlush(0);

        if (Globals::enableDebugPrint)
        {
            if (frame % 60 == 30) //restricting this 2Hz otherwise it tanks performance
            {
                textCtrl->clearArea(textVideoBufferSub, 1, 120, 128, 72);
                char buf[128];
                std::string debugText = "";
                std::sprintf(buf, "Touch x = %04X, %04X\n", touch.rawx, touch.px);
                debugText += buf;
                std::sprintf(buf, "Touch y = %04X, %04X\n", touch.rawy, touch.py);
                debugText += buf;
                std::sprintf(buf,
                             "tile(x,z): %d, %d\n",
                             (int)((charPos.x + dbEntry->worldOffsetX) / tileSize),
                             (int)((charPos.z + dbEntry->worldOffsetZ) / tileSize));
                debugText += buf;
                std::sprintf(buf, "translate(x,z): %d, %d\n", (int)(charPos.x * 100), (int)(charPos.z * 100));
                debugText += buf;
                std::sprintf(
                    buf, "angle(w,c): %d, %d\n", (int)(cameraCtrl.getAngle() * 100), (int)(charPos.facingAngle * 100));
                debugText += buf;
                textCtrl->drawText(debugText, cosmeticaFont, textVideoBufferSub, 1, 120, TextColor::Red);
            }
        }

        break;
    }

    default:
    {
        phase = ViewPhase::Environment;
        break;
    }
    }

    characterAnimationCtrl->update();
    musicCtrl->update();

    return ViewState::KEEP_CURRENT;
}

void EnvironmentView::cleanup()
{
    textCtrl->clearScreen(textVideoBuffer);
    textCtrl->clearScreen(textVideoBufferSub);
    textCtrl->unloadPalette();
    // the console was setup in init(), so we can safely clear it here
    //consoleClear();
    BaseView::cleanup();

    env.cleanup();
    uiCtrl->cleanup();

    delete playerCtrl;
    playerCtrl = nullptr;
}
