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
#include "battleActions/enemies/EnemyProfileDb.h"
#include "battleActions/party/CharacterProfileDb.h"
#include "battleActions/personas/PersonaDb.h"
#include "battleActions/shoes/ShoeDb.h"
#include "battleActions/skills/SkillDb.h"
#include "battleActions/weapons/WeaponDb.h"

// aegis engine
#include "systems/BattleSystem.hpp"
#include <aegis/engine.hpp>

// variables
volatile int frame = 0;
volatile u32 systemKeysDown = 0;
volatile u32 systemKeysHeld = 0;
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
    frame = frame + 1;
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

// aegis engine
namespace GameEngineConfig
{
using LargestMessage = etl::largest_type<Event::BattleResult, Event::ExecuteBattle, Event::SetTextVideoBufferSub>;
constexpr std::size_t kLargestComponentSize = sizeof(LargestMessage);
constexpr std::size_t kLargestComponentAlign = alignof(LargestMessage);
} // namespace GameEngineConfig

using GameEngine = ae::Engine<GameEngineConfig::kLargestComponentSize, GameEngineConfig::kLargestComponentAlign>;

// TODO: add javadoc
void NDSPollInputCallback()
{
    scanKeys();
    systemKeysDown = keysDown();
    systemKeysHeld = keysHeld();
}

// TODO: add javadoc
void NDSComputeCallback()
{
    //...
}

int main(int argc, char* argv[])
{
    irqSet(IRQ_VBLANK, Vblank);
    static GameEngine engine;

    // initialize DLDI/FAT
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

    // set FEMC mode
    prevFemcMode = saveData.femcMode;

    // setup character model
    loadModels(saveData.femcMode);

    // setup db's. DO NOT CHANGE order
    WeaponDb::Initialize();
    SkillDb::Initialize();
    ArmourDb::Initialize();
    ShoeDb::Initialize();
    PersonaDb::Initialize();
    EnemyProfileDb::Initialize();
    CharacterProfileDb::Initialize();

    // setup globals
    Globals::enableDebugPrint = false;
    Globals::enableBillboards = true;
    Globals::enableCharacterAnim = true;
    Globals::isPauseMenuActive = false;

    // seed random using DS hardware timer
    TIMER0_CR = TIMER_ENABLE | TIMER_DIV_1;
    srand(TIMER0_DATA);

    // set platform hooks
    engine.SetComputeCallback(&NDSComputeCallback);
    engine.SetComputeEnabled(true);
    engine.SetPollInputCallback(&NDSPollInputCallback);
    engine.SetPollingEnabled(true);

    // register singletons
    engine.RegisterSystem(&BattleSystem::GetInstance());

    // initialize engine
    engine.InitAll();

    // set up initial game state
    // Default is DisclaimerView
    SwitchView(new DisclaimerView());

    // TODO: set to constant tied to VBlank
    const ae::fixed_t dt = ae::fixed_t(1) / 60;

    while (pmMainLoop())
    {
        swiWaitForVBlank();

        // Poll Input -> Update Systems -> Update Components -> Process Managers -> Compute
        engine.Tick(dt);

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

    engine.ShutdownAll();

    return 0;
}
