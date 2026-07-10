#include "MenuBackgroundScreen.h"
#include "core/globals.h"

#include <string>

MenuBackgroundScreen* MenuBackgroundScreen::instance = nullptr;

void MenuBackgroundScreen::create()
{
    if (instance == nullptr)
    {
        instance = new MenuBackgroundScreen();
    }
}

void MenuBackgroundScreen::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }

    instance = nullptr;
}

MenuBackgroundScreen* MenuBackgroundScreen::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }

    return instance;
}

void MenuBackgroundScreen::load()
{
    loadedBgIndex = MENU_BACKGROUND_SCREEN_INVALID_BG_INDEX;
}

void MenuBackgroundScreen::unload()
{
    loadedBgIndex = MENU_BACKGROUND_SCREEN_INVALID_BG_INDEX;
}

std::string MenuBackgroundScreen::resolveBgName(int bgIndex) const
{
    switch (bgIndex)
    {
    case 0:
        return "bgAkihiko";
    case 1:
        return "bgKenji";
    case 2:
        return "bgYukari";
    case 3:
        return "bgYukariClose";
    default:
        return "";
    }
}

void MenuBackgroundScreen::showBackground(int bgIndex)
{
    if (bgIndex == this->loadedBgIndex)
    {
        return;
    }

    std::string bgName = this->resolveBgName(bgIndex);
    if (bgName.empty())
    {
        // Path was not resolved, time to early return and skip loading
        return;
    }

    std::string bgPath = fatBasePath + "graphics/Dialogue/backgrounds/" + bgName + "/" + bgName;

    GraphicAsset bg = this->graphicsController->loadGrit(bgPath);

    dmaCopy(bg.tiles, bgGetGfxPtr(bgId), bg.tilesLen);
    dmaCopy(bg.map, bgGetMapPtr(bgId), bg.mapLen);

    vramSetBankH(VRAM_H_LCD);

    dmaCopy(bg.pal, &VRAM_H_EXT_PALETTE[0][0], bg.palLen);

    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

    this->graphicsController->unloadGrit(bg);

    this->loadedBgIndex = bgIndex;
}
