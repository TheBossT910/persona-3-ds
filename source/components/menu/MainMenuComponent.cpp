#include "MainMenuComponent.h"
#include "controllers/SaveController.h"
#include "core/globals.h"
#include <string>

void MainMenuComponent::loadBg(int bgIndex)
{
    if (bgIndex < 0)
        return;

    std::string bgName;
    bool showBg = false;
    switch (bgIndex)
    {
    case 0: // Akihiko
        bgName = "bgAkihiko";
        break;

    case 1: // Kenji
        bgName = "bgKenji";
        break;

    case 2: // Yukari
        bgName = "bgYukari";
        showBg = true;
        break;

    case 3: // YukariClose
        bgName = "bgYukariClose";
        showBg = true;
        break;

    default:
        return;
    }

    GraphicAsset bg = graphicsCtrl->loadGrit(fatBasePath + "graphics/Dialogue/backgrounds/" + bgName + "/" + bgName);
    dmaCopy(bg.tiles, bgGetGfxPtr(bgSlot), bg.tilesLen);
    dmaCopy(bg.map, bgGetMapPtr(bgSlot), bg.mapLen);

    vramSetBankH(VRAM_H_LCD);
    dmaCopy(bg.pal, &VRAM_H_EXT_PALETTE[0][0], bg.palLen);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

    graphicsCtrl->unloadGrit(bg);

    if (showBg)
    {
        bgShow(bgSlot);
    }
}

void MainMenuComponent::init(int iBgSlot, bool* isActive, const std::string& iPauseMessage)
{
    BaseMenu::init(iBgSlot, isActive, iPauseMessage);
    options = mainMenuOptions;
    optionCount = MAIN_MENU_OPTIONS;
}

// option handlers
ViewState MainMenuComponent::mainMenuOptionSelected()
{
    ViewState selectedView;
    switch (static_cast<MainMenuOptions>(selectedOption))
    {
    case MainMenuOptions::LOAD_GAME:
        changeMenu(levelOptions, LEVEL_OPTIONS);
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case MainMenuOptions::SETTINGS:
        changeMenu(settingOptions, SETTING_OPTIONS);
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case MainMenuOptions::RETURN_TO_TITLE:
        selectedView = ViewState::INTRO;
        break;
    default:
        selectedView = ViewState::KEEP_CURRENT;
    }

    return selectedView;
}

ViewState MainMenuComponent::levelOptionSelected()
{
    ViewState selectedView;
    switch (static_cast<LevelOptions>(selectedOption))
    {
    case LevelOptions::START_GAME:
        selectedView = ViewState::CUTSCENE_1;
        break;
    case LevelOptions::IWATODAI_DORM:
        selectedView = ViewState::IWATODAI_DORM;
        break;
    case LevelOptions::IWATODAI_STREETS:
        selectedView = ViewState::IWATODAI_STREETS;
        break;
    case LevelOptions::STATION:
        selectedView = ViewState::STATION;
        break;
    case LevelOptions::PAULOWNIA_MALL:
        selectedView = ViewState::PAULOWNIA_MALL;
        break;
    case LevelOptions::SIGN_CONTRACT:
        selectedView = ViewState::SIGN_CONTRACT;
        break;
    default:
        selectedView = ViewState::KEEP_CURRENT;
    }

    return selectedView;
}

ViewState MainMenuComponent::settingOptionSelected()
{
    ViewState selectedView;
    switch (static_cast<SettingOptions>(selectedOption))
    {
    case SettingOptions::CHANGE_INTRO_VIDEO:
        changeMenu(settingIntroOptions, SETTING_INTRO_OPTIONS);
        selectedView = ViewState::KEEP_CURRENT;
        break;
    case SettingOptions::FEMC_MODE:
        saveData.femcMode = !saveData.femcMode;
        updateSave();
        selectedView = ViewState::KEEP_CURRENT;
        break;
    default:
        selectedView = ViewState::KEEP_CURRENT;
    }

    return selectedView;
}

ViewState MainMenuComponent::settingIntroOptionSelected()
{
    switch (static_cast<SettingIntroOptions>(selectedOption))
    {
    case SettingIntroOptions::ORIGINAL:
        strncpy(saveData.introVideoPath, "base.vid", sizeof(saveData.introVideoPath));
        break;
    case SettingIntroOptions::FES:
        strncpy(saveData.introVideoPath, "fes.vid", sizeof(saveData.introVideoPath));
        break;
    case SettingIntroOptions::PORTABLE:
        strncpy(saveData.introVideoPath, "portable.vid", sizeof(saveData.introVideoPath));
        break;
    case SettingIntroOptions::RELOAD:
        strncpy(saveData.introVideoPath, "reload.vid", sizeof(saveData.introVideoPath));
        break;
    default:
        strncpy(saveData.introVideoPath, "reload.vid", sizeof(saveData.introVideoPath));
    }

    updateSave();
    return ViewState::KEEP_CURRENT;
}

void MainMenuComponent::updateSave()
{
    if (!SaveController::getInstance()->write())
    {
        consoleDemoInit();
        iprintf("Failed to write save data!\n");
        while (1)
        {
            swiWaitForVBlank();
        }
    }
}
