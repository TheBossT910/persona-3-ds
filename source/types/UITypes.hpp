#pragma once

#include "types/StateTypes.hpp"

class UIMenu;

struct MenuOption
{
    const char* name;
    int bgIndex;
    ViewState (UIMenu::*onSelect)();
};

struct MenuState
{
    MenuOption* options;
    int optionCount;
    int selectedOption;
    int startIndex;
};
