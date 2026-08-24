#pragma once
#include "Element.hpp"
#include "armours/Armour.hpp"
#include <nds.h>

/**
 * @brief Basic stats / affinities. Used by Personas or directly by Enemies.
 *
 * @author Nolan Kolb (TrueGiles / themoonwalker8692)
*/

struct BattleStats
{
    u32 st;
    u32 ma;
    u32 en;
    u32 ag;
    u32 lu;

    enum Affinity
    {
        Neutral,
        Weak,
        Resist,
        Null,
        Absorb,
        Repel
    };

    Affinity affinities[10] = {
        Neutral,
        Neutral,
        Neutral,
        Neutral,
        Neutral,
        Neutral,
        Neutral,
        Neutral,
        Neutral,
        Neutral // almighty
    };

    BattleStats* getBattleStats()
    {
        return this;
    };
};
