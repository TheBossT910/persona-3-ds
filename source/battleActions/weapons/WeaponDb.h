#pragma once
#include "Weapon.h"

struct WeaponDb
{
    static Weapon shortSword;
    static Weapon imitationKatana;
    static Weapon practiceBow;

    static void initialize();
};
