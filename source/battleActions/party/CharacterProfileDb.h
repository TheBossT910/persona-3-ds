#pragma once
#include "CharacterProfile.h"

struct CharacterProfileDb
{
    static CharacterProfile player;
    static CharacterProfile yukari;
    static CharacterProfile junpei;

    static void Initialize();
};
