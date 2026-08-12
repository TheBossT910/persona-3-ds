#include "MenuBackgroundScreen.hpp"
#include "core/globals.hpp"

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
    if (menuBackground == nullptr)
    {
        menuBackground = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        menuBackground->AddComponent(graphics);
    }

    loadedBgIndex = MENU_BACKGROUND_SCREEN_INVALID_BG_INDEX;
}

void MenuBackgroundScreen::unload()
{
    loadedBgIndex = MENU_BACKGROUND_SCREEN_INVALID_BG_INDEX;

    if (graphics != nullptr)
    {
        graphics->unloadAll();
    }

    if (menuBackground != nullptr)
    {
        menuBackground->RemoveComponent<GraphicsComponent>();
        engine.DestroyEntity(menuBackground);

        menuBackground = nullptr;
        graphics = nullptr;
    }
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

    std::string bgPath = "graphics/Dialogue/backgrounds/" + bgName + "/" + bgName;

    GraphicAsset bg = this->graphics->loadGraphic(bgPath);

    dmaCopy(bg.tiles, bgGetGfxPtr(bgId), bg.tilesLen);
    dmaCopy(bg.map, bgGetMapPtr(bgId), bg.mapLen);

    vramSetBankH(VRAM_H_LCD);

    dmaCopy(bg.pal, &VRAM_H_EXT_PALETTE[0][0], bg.palLen);

    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

    this->graphics->unloadGraphic(bg);

    this->loadedBgIndex = bgIndex;
}
