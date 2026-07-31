#pragma once

#define MENU_BIND(ClassName, Method) reinterpret_cast<ViewState (BaseMenu::*)()>(&ClassName::Method)

#include "controllers/MusicController.h"
#include "controllers/TextController.h"
#include "core/structs.h"
#include <maxmod9.h>
#include <nds.h>
#include <stack>
#include <string>

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

    virtual void loadBg(int bgIndex) = 0;

  private:
    // sfx
    mm_sfxhand sfxMenuHandle;
    mm_sfxhand sfxSelectHandle;
    mm_sfxhand sfxCancelHandle;

    // options
    int visibleOptions = 23;
    std::stack<MenuState> prevOptions;
    ViewState nextViewState = ViewState::KEEP_CURRENT;

  public:
    virtual void init(int iBgSlot,
                      bool* isActive,
                      uint16_t* iTextVideoBuffer,
                      uint16_t* iTextVideoBufferSub,
                      const std::string& iPauseMessage = "Pause");
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
    TextController* textCtrl = TextController::getInstance();
    Font* font = textCtrl->loadFont("cosmetica", 12);
    uint16_t* textVideoBuffer = nullptr;
    uint16_t* textVideoBufferSub = nullptr;
};
