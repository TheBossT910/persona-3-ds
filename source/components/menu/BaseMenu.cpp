#include "BaseMenu.hpp"
#include "core/globals.hpp"
#include "soundbank.h"

void BaseMenu::cancelSFX()
{
    musicCtrl->stopSFX(sfxMenuHandle);
    musicCtrl->stopSFX(sfxSelectHandle);
    musicCtrl->stopSFX(sfxCancelHandle);
}

void BaseMenu::init(int iBgSlot,
                    bool* isActive,
                    uint16_t* iTextVideoBuffer,
                    uint16_t* iTextVideoBufferSub,
                    const std::string& iPauseMessage)
{
    // point to music
    musicCtrl->loadSFX(SFX_MENU);
    musicCtrl->loadSFX(SFX_SELECT);
    musicCtrl->loadSFX(SFX_CANCEL);

    // set default options
    selectedOption = 0;
    startIndex = 0;
    while (!prevOptions.empty())
        prevOptions.pop();

    pauseMessage = iPauseMessage;
    bgSlot = iBgSlot;
    isActivePtr = isActive;
    textVideoBuffer = iTextVideoBuffer;
    textVideoBufferSub = iTextVideoBufferSub;

    // initialize view state
    nextViewState = ViewState::KEEP_CURRENT;
}

void BaseMenu::reset()
{
    selectedOption = 0;
    startIndex = 0;
    while (!prevOptions.empty())
        prevOptions.pop();
}

ViewState BaseMenu::update(int keys)
{
    // navigate options
    if (keys & KEY_DOWN)
    {
        sfxMenuHandle = musicCtrl->playSFX(SFX_MENU, 255, 128);
        selectedOption = (selectedOption + 1) % optionCount;
    }
    else if (keys & KEY_UP)
    {
        sfxMenuHandle = musicCtrl->playSFX(SFX_MENU, 255, 128);
        selectedOption = (selectedOption + optionCount - 1) % optionCount;
    }

    // Adjust scroll position
    if (selectedOption < startIndex)
    {
        startIndex = selectedOption;
        textCtrl->clearScreen(textVideoBufferSub);
    }

    if (selectedOption >= startIndex + visibleOptions)
    {
        startIndex = selectedOption - visibleOptions + 1;
        textCtrl->clearScreen(textVideoBufferSub);
    }
    else if (keys & KEY_A)
    {
        cancelSFX();
        sfxSelectHandle = musicCtrl->playSFX(SFX_SELECT, 255, 128);

        MenuState currentState = {options, optionCount, selectedOption, startIndex};

        textCtrl->clearScreen(textVideoBufferSub);

        if (options[selectedOption].onSelect != nullptr)
        {
            ViewState result = (this->*(options[selectedOption].onSelect))();
            if (result != ViewState::KEEP_CURRENT)
            {
                nextViewState = result;
                *isActivePtr = false;
                if (bgSlot >= 0)
                    bgHide(bgSlot);
            }

            // if we changed options, push current state to stack
            if (options != currentState.options)
            {
                prevOptions.push(currentState);
            }
        }
    }

    if (keys & KEY_B)
    {
        cancelSFX();
        musicCtrl->playSFX(SFX_CANCEL, 255, 128);
        selectedOption = 0;
        startIndex = 0;
        prevOption();
    }

    sassert(textVideoBufferSub != nullptr, "BaseMenu::update - textVideoBufferSub is null");

    // blink the "Pause" text
    if (frame % 60 < 30)
    {
        textCtrl->drawText(pauseMessage, font, textVideoBufferSub, 0, 0, 2);
    }

    // display options
    for (int i = 0; i < visibleOptions && startIndex + i < optionCount; i++)
    {
        int option = startIndex + i;
        if (option == selectedOption)
            textCtrl->drawText(options[option].name, font, textVideoBufferSub, 10, 8 + i * 9, TextColor::Blue);
        else
            textCtrl->drawText(options[option].name, font, textVideoBufferSub, 10, 8 + i * 9, TextColor::White);
    }

    // load selectedOption's background
    if (bgSlot >= 0)
    {
        int bgIndex = options[selectedOption].bgIndex;
        if (bgIndex != -1)
        {
            menuBgScreen->showBackground(bgIndex);
            bgShow(bgSlot);
        }
        else
        {
            bgHide(bgSlot);
        }
    }

    ViewState viewState = nextViewState;
    nextViewState = ViewState::KEEP_CURRENT;
    return viewState;
}

ViewState BaseMenu::changeMenu(MenuOption* newOptions, int newOptionCount)
{
    selectedOption = 0;
    startIndex = 0;
    options = newOptions;
    optionCount = newOptionCount;
    return ViewState::KEEP_CURRENT;
}

void BaseMenu::prevOption()
{
    textCtrl->clearScreen(textVideoBufferSub);
    // if we're in a submenu, return to main menu
    if (!prevOptions.empty())
    {
        MenuState prevState = prevOptions.top();
        prevOptions.pop();

        options = prevState.options;
        optionCount = prevState.optionCount;
        selectedOption = prevState.selectedOption;
        startIndex = prevState.startIndex;
    }
    else
    {
        // otherwise, close the menu
        *isActivePtr = false;
        if (bgSlot >= 0)
            bgHide(bgSlot);
    }
}
