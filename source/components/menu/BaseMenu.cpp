#include "BaseMenu.h"

void BaseMenu::resetMenu()
{
    selectedOption = 0;
    startIndex = 0;
    isActive = false;
    nextViewState = ViewState::KEEP_CURRENT;

    while (!prevOptions.empty())
    {
        prevOptions.pop();
    }
}

ViewState BaseMenu::changeMenu(MenuOption* newOptions, int newOptionCount)
{
    // add to prevOptions
    MenuState currentState = {options, optionCount, selectedOption, startIndex};
    prevOptions.push(currentState);

    // set new options
    options = newOptions;
    optionCount = newOptionCount;
    selectedOption = 0;
    startIndex = 0;

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
        isActive = false;
    }
}

ViewState BaseMenu::updateHook()
{
    return ViewState::DEFAULT;
}
