#pragma once

#define MENU_BIND(ClassName, Method) reinterpret_cast<ViewState (BaseMenu::*)()>(&ClassName::Method)

#include "components/ui/MenuBackgroundScreen.h"
#include "controllers/MusicController.h"
#include "core/structs.h"
#include <maxmod9.h>
#include <nds.h>
#include <stack>
#include <string>

#include "components/TextComponent.hpp"
#include "managers/RenderManager.hpp"

class BaseMenu
{
  protected:
    bool* isActivePtr;
    int bgSlot = 0;
    std::string pauseMessage = "Pause";

    // options
    MenuOption* options;
    int optionCount = 0;
    int selectedOption = 0;
    int startIndex = 0;

  private:
    RenderManager& render = RenderManager::GetInstance();

    // sfx
    mm_sfxhand sfxMenuHandle;
    mm_sfxhand sfxSelectHandle;
    mm_sfxhand sfxCancelHandle;

    // options
    int visibleOptions = 23;
    std::stack<MenuState> prevOptions;
    ViewState nextViewState = ViewState::KEEP_CURRENT;

  public:
    virtual void init(int iBgSlot, bool* isActive, TextComponent* iText, const std::string& iPauseMessage = "Pause");
    /**
     * @brief Resets the menu to its initial state.
     */
    virtual void reset();
    virtual ViewState update(int keys);
    void cancelSFX();
    ViewState changeMenu(MenuOption* newOptions, int newOptionCount);
    void prevOption();

  protected:
    MusicController* musicCtrl = MusicController::getInstance();
    MenuBackgroundScreen* menuBgScreen = MenuBackgroundScreen::getInstance();

    TextComponent* text = nullptr;
};
