#pragma once
#include "../BattleParticipant.h"
#include "../BattleStats.h"
#include "../enemies/Enemy.h"
#include "../party/Player.h"
#include "../shoes/Shoe.h"
#include "../skills/SkillDb.h"
#include "Skill.h"
#include <algorithm>
#include <cmath>
#include <nds.h>
#include <string>

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
