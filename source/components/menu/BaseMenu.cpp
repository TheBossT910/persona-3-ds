#include "BaseMenu.h"

void BaseMenu::configureMenu(bool* isActive, const std::string& iPauseMessage)
{
    // set default options
    selectedOption = 0;
    startIndex = 0;
    while (!prevOptions.empty())
    {
        prevOptions.pop();
    }

    pauseMessage = iPauseMessage;
    isActivePtr = isActive;

    // initialize view state
    nextViewState = ViewState::KEEP_CURRENT;
}

void BaseMenu::resetMenu()
{
    selectedOption = 0;
    startIndex = 0;
    while (!prevOptions.empty())
        prevOptions.pop();
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
    }
}

ViewState BaseMenu::updateHook()
{
    return ViewState::DEFAULT;
}
