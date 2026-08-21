#pragma once
#include "../BattleParticipant.hpp"
#include "../BattleStats.hpp"
#include "../enemies/Enemy.hpp"
#include "../party/Player.hpp"
#include "../shoes/Shoe.hpp"
#include "../skills/SkillDb.hpp"
#include "Skill.hpp"
#include <algorithm>
#include <cmath>
#include <nds.h>
#include <string>

/**
 * @brief various calculations used in Batlle
 *
 * @details
 * These are battle calculations to decide things like damage, healing, hitrate etc.
 * These forumlas are directly from Reload, since those are the only one well documented.
 *
 * Private functions are helpers.
 *
 * For a complete breakdown, visit https://steamcommunity.com/sharedfiles/filedetails/?id=3230774091
 * Credits for this document goes to CTOBN on Steam.
 *
 * @author Nolan Kolb (TrueGiles / themoonwalker8692)
 */

struct BattleCalcs
{
    static u32 attack(BattleParticipant& attacker, BattleParticipant& defender, Skill& skill);
    static u32 hitrate(BattleParticipant& attacker, BattleParticipant& defender, Skill& skill);
    static u32 healing(BattleParticipant& user, Skill& skill);

    static u32 getAtk(BattleStats& attackerStats, Skill& skill);
    static float getLevelDifference(u32 attackerLevel, u32 defenderLevel);
    static float getAffinityMtp(BattleStats& battleStats, Skill& skill);

    static u32 allOutAttack(Player& attacker, BattleParticipant& defender, u32 participantCount);

  private:
    //Attack
    static const float levelMultipliers[24];

    //Heal
    static const float magicBoostTableHeal[20];
    static u32 getMagicBoostHeal(u32& magic);
};
