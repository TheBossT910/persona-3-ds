/**
 * @file globals.h
 * @brief Holds global variables used game-wide
 */

#pragma once
#include "core/structs.h"
#include <string>

// variables
extern volatile int frame;
extern volatile u32 systemKeysDown;
extern volatile u32 systemKeysHeld;
extern int fps;
extern int fpsTimer;
extern std::string fatBasePath;
extern Save saveData;
extern unsigned int** bitmapsCharacter;

class Globals
{
  public:
    static bool enableBillboards;
    static bool enableDebugPrint;
    static bool enableCharacterAnim;
    static bool isPauseMenuActive;
};
