/**
 * @file structs.h
 * @brief Holds structs used game-wide
 */

#pragma once
#include "core/enums.h"
#include "core/geometry.h"
#include <etl/vector.h>
#include <nds.h>
#include <string>
#include <vector>

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

struct CharacterPosition
{
    float x;
    float z;
    float y; // height
    float facingAngle;
};

struct Save
{
    char introVideoPath[128];
    char lastName[32];
    char firstName[32];
    bool femcMode;
} __attribute__((packed));

struct GraphicAsset
{
    int id;
    void* tiles = nullptr;
    u32 tilesLen;
    void* pal = nullptr;
    u32 palLen;
    void* map = nullptr;
    u32 mapLen;
};
