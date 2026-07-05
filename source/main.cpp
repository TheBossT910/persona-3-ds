#include <dirent.h>
#include <fat.h>
#include <filesystem.h>
#include <maxmod9.h>
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "core/enums.h"

// states
#include "views/BaseView.h"
#include "views/DisclaimerView.h"
#include "views/IntroView.h"
#include "views/IwatodaiDormView.h"
#include "views/IwatodaiStreetsView.h"
#include "views/MainMenuView.h"
#include "views/PaulowniaMallView.h"
#include "views/SignContractView.h"
#include "views/StationView.h"
#include "views/VideoView.h"

// controllers
#include "controllers/SaveController.h"

// components
#include "components/ui/MenuHUDScreen.h"

// sfx
#include "soundbank_bin.h"

// character models
#include "models/kotone.h"
#include "models/makoto.h"

// DBs
#include "battleActions/armours/ArmourDb.h"
#include "battleActions/enemies/EnemyDb.h"
#include "battleActions/party/CharacterProfileDb.h"
#include "battleActions/personas/PersonaDb.h"
#include "battleActions/shoes/ShoeDb.h"
#include "battleActions/skills/SkillDb.h"
#include "battleActions/weapons/WeaponDb.h"

// variables
volatile int frame = 0;
int fps = 0;
int fpsTimer = 0;
std::string fatBasePath = "";
Save saveData;

// models
unsigned int** bitmapsCharacter = nullptr;

static unsigned int* bitmapsKotone[MODEL_KOTONE_TEX_COUNT] = {nullptr};
static unsigned int* bitmapsMakoto[MODEL_MAKOTO_TEX_COUNT] = {nullptr};

// TODO: figure out a way to unload after being copied to ram
static unsigned int* loadCharacterTexture(const std::string& name, bool isFemc)
{
    std::string basePath = fatBasePath + "models/" + (isFemc ? "kotone/" : "makoto/");
    GraphicAsset asset = GraphicsController::getInstance()->loadGrit(basePath + name);
    unsigned int* tiles = reinterpret_cast<unsigned int*>(asset.tiles);
    // GraphicsController::getInstance()->unloadGrit(asset);
    return tiles;
}

BaseView* currentView = nullptr;
bool prevFemcMode;

void SwitchView(BaseView* newView)
{
    // cleanup any existing view
    if (currentView != nullptr)
    {
        currentView->cleanup();

        // free memory
        delete currentView;
    }

    // load new view
    currentView = newView;
    if (currentView != nullptr)
    {
        currentView->init();
    }
}

// fn for the interrupt
void Vblank()
{
    frame++;
}

void loadModels(bool isFemc)
{
    // Kotone
    if (isFemc)
    {
        bitmapsKotone[MODEL_KOTONE_TEX_KOTONE_TEXTURE_0] = loadCharacterTexture("kotone_texture_0", true);
        bitmapsKotone[MODEL_KOTONE_TEX_KOTONE_TEXTURE_1] = loadCharacterTexture("kotone_texture_1", true);
        bitmapsKotone[MODEL_KOTONE_TEX_KOTONE_TEXTURE_2] = loadCharacterTexture("kotone_texture_2", true);
        bitmapsKotone[MODEL_KOTONE_TEX_KOTONE_TEXTURE_3] = loadCharacterTexture("kotone_texture_3", true);
        bitmapsKotone[MODEL_KOTONE_TEX_KOTONE_TEXTURE_4] = loadCharacterTexture("kotone_texture_4", true);

        bitmapsCharacter = bitmapsKotone;
    }
    // Makoto
    else
    {
        bitmapsMakoto[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_0] = loadCharacterTexture("makoto_texture_0", false);
        bitmapsMakoto[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_1] = loadCharacterTexture("makoto_texture_1", false);
        bitmapsMakoto[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_2] = loadCharacterTexture("makoto_texture_2", false);
        bitmapsMakoto[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_3] = loadCharacterTexture("makoto_texture_3", false);
        bitmapsMakoto[MODEL_MAKOTO_TEX_MAKOTO_TEXTURE_4] = loadCharacterTexture("makoto_texture_4", false);

        bitmapsCharacter = bitmapsMakoto;
    }
}

int main(int argc, char* argv[])
{
    irqSet(IRQ_VBLANK, Vblank);

    // Initialize DLDI/FAT instead
    if (!fatInitDefault())
    {
        consoleDemoInit();
        iprintf("FAT initialization failed!\nPlease ensure the SD card is inserted.\n");
        while (1)
            swiWaitForVBlank();
    }

    // dynamically resolve runtime path using argv[0]
    if (argc > 0 && argv[0] != nullptr)
    {
        std::string execPath(argv[0]);
        size_t lastSlash = execPath.find_last_of('/');

        if (lastSlash != std::string::npos)
        {
            fatBasePath = execPath.substr(0, lastSlash + 1) + "data/";
        }
    }

    // initialize maxmod (for audio)
    mm_ds_system sys;
    sys.mod_count = 0;
    sys.samp_count = 0;
    sys.mem_bank = 0;
    mmInit(&sys);

    // initialize maxmod (for sfx)
    mmInitDefaultMem((mm_addr)soundbank_bin);

    // load save data
    if (!SaveController::getInstance()->read())
    {
        consoleDemoInit();
        iprintf("Failed to read save data!\n");
        while (1)
        {
            swiWaitForVBlank();
        }
    }

    prevFemcMode = saveData.femcMode;

    // setup character model
    loadModels(saveData.femcMode);

    // setup db's. DO NOT CHANGE order
    WeaponDb::Initialize();
    SkillDb::Initialize();
    ArmourDb::Initialize();
    ShoeDb::Initialize();
    PersonaDb::Initialize();
    EnemyDb::Initialize();
    CharacterProfileDb::Initialize();
    //Setup globals
    Globals::enableDebugPrint = false;
    Globals::enableBillboards = true;
    Globals::enableCharacterAnim = true;
    Globals::isPauseMenuActive = false;

    // seed random using DS hardware timer
    TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1;
    srand(TIMER0_DATA);

    // start with DisclaimerView
    SwitchView(new DisclaimerView());

    while (pmMainLoop())
    {
        swiWaitForVBlank();

        if (saveData.femcMode != prevFemcMode)
        {
            loadModels(saveData.femcMode);
            prevFemcMode = saveData.femcMode;
        }

        // check state of currentView
        if (currentView != nullptr)
        {
            ViewState nextState = currentView->update();
            switch (nextState)
            {
            case ViewState::INTRO:
                SwitchView(new IntroView());
                break;

            case ViewState::MAIN_MENU:
                SwitchView(new MainMenuView());
                break;

            case ViewState::IWATODAI_DORM:
                SwitchView(new IwatodaiDormView());
                break;

            case ViewState::IWATODAI_STREETS:
                SwitchView(new IwatodaiStreetsView());
                break;

            case ViewState::DISCLAIMER:
                SwitchView(new DisclaimerView());
                break;

            case ViewState::INTRO_VIDEO:
                SwitchView(new VideoView(saveData.introVideoPath, ViewState::INTRO));
                break;

            case ViewState::CUTSCENE_1:
                SwitchView(new VideoView("cutscene-1.vid", ViewState::SIGN_CONTRACT));
                break;

            case ViewState::SIGN_CONTRACT:
                SwitchView(new SignContractView());
                break;

            case ViewState::CUTSCENE_2:
                SwitchView(new VideoView("cutscene-2.vid", ViewState::IWATODAI_DORM));
                break;

            case ViewState::STATION:
                SwitchView(new StationView());
                break;

            case ViewState::PAULOWNIA_MALL:
                SwitchView(new PaulowniaMallView());
                break;

            default:
                break;
            }
        }

        bgUpdate();
        oamUpdate(&oamMain);
    }

    return 0;
}
