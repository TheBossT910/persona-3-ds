#include "BattleCalcs.h"

u32 BattleCalcs::attack(BattleParticipant& attacker, BattleParticipant& defender, Skill& skill)
{
    float base = attacker.calculateBaseDamage(defender, skill);
    float range = 95 + (u32)(rand() % 11);
    return std::clamp((u32)trunc(base * range / 100.0f), (u32)1, (u32)99999);
}

u32 BattleCalcs::hitrate(BattleParticipant& attacker, BattleParticipant& defender, Skill& skill)
{
    BattleStats& attackerStats = *attacker.getBattleStats();
    BattleStats& defenderStats = *defender.getBattleStats();

    if (skill.hitRate == 100)
        return skill.hitRate;

    float baseAccuracy = (float)(attackerStats.ag + 200) / (defenderStats.ag + 200);
    u32 multipliedAccuracy;
    if (attacker.participantType == ParticipantType::Enemy)
    {
        float shoeMultiplier = (float)(attackerStats.ag + 200) / (defender.shoe.evasion / 2.0f + 200);
        multipliedAccuracy = baseAccuracy * skill.hitRate * shoeMultiplier;
    }
    else
    {
        multipliedAccuracy = (u32)(baseAccuracy * skill.hitRate);
    }

    return std::clamp(multipliedAccuracy, (u32)50, (u32)99);
}

u32 BattleCalcs::healing(BattleParticipant& user, Skill& skill)
{
    BattleStats* battleStats = user.getBattleStats();
    float teamMultiplier = user.getTeamMultiplier();

    u32 magicBoost = BattleCalcs::getMagicBoostHeal(battleStats->ma);
    u32 base = (u32)floor((skill.movePower + magicBoost) * teamMultiplier);
    u32 range = 95 + (u32)(rand() % 11);
    return (u32)floor(base * range / 100.0f);
}

u32 BattleCalcs::allOutAttack(Player& attacker, BattleParticipant& defender, u32 participantCount)
{
    float levelDifference = BattleCalcs::getLevelDifference(attacker.lv, defender.lv);
    BattleStats& attackerStats = *attacker.getBattleStats();
    BattleStats& defenderStats = *defender.getBattleStats();

    float affinityMtp = BattleCalcs::getAffinityMtp(*defender.getBattleStats(), SkillDb::allOutAttack);
    float base = trunc(sqrt((float)((attacker.weapon.weaponPower / 2.0f) * 15 * attackerStats.st) / defenderStats.en) *
                       1.6f * (levelDifference * levelDifference) * affinityMtp * participantCount);
    float range = 95 + (u32)(rand() % 11);
    return (u32)trunc(base * range / 100.0f);
}

const float BattleCalcs::levelMultipliers[24] = {0.5f,  0.51f, 0.53f, 0.59f, 0.66f, 0.75f, 0.84f, 0.91f,
                                                 0.97f, 0.99f, 1.0f,  1.0f,  1.0f,  1.0f,  1.01f, 1.03f,
                                                 1.09f, 1.16f, 1.25f, 1.34f, 1.41f, 1.47f, 1.49f, 1.5f};

const float BattleCalcs::magicBoostTableHeal[20] = {
    0,   // 1-5
    6,   // 6-10
    12,  // 11-15
    17,  // 16-20
    22,  // 21-25
    27,  // 26-30
    34,  // 31-35
    44,  // 36-40
    54,  // 41-45
    65,  // 46-50
    75,  // 51-55
    85,  // 56-60
    93,  // 61-65
    100, // 66-70
    105, // 71-75
    110, // 76-80
    115, // 81-85
    120, // 86-90
    125, // 91-95
    130  // 96-99
};

u32 BattleCalcs::getAtk(BattleStats& attackerStats, Skill& skill)
{
    if (skill.skillRace == SkillRace::phys)
    {
        return attackerStats.st;
    }
    else
    {
        return attackerStats.ma;
    }
}

float BattleCalcs::getLevelDifference(u32 attackerLevel, u32 defenderLevel)
{
    s32 diff = attackerLevel - defenderLevel;
    diff = std::clamp(diff, (s32)-13, (s32)10);

    return levelMultipliers[diff + 13]; // offset so -13 is 0
}

float BattleCalcs::getAffinityMtp(BattleStats& defenderStats, Skill& skill)
{
    u32 affinity = defenderStats.affinities[(u32)skill.element];

    switch (affinity)
    {
    case BattleStats::Affinity::Weak:
        return 1.25f;
    case BattleStats::Affinity::Resist:
        return 0.5f;
    case BattleStats::Affinity::Null:
        return 0.0f;
    case BattleStats::Affinity::Absorb:
        return -1.0f;
    case BattleStats::Affinity::Repel:
        return -2.0f; // TODO: add repel logic
    case BattleStats::Affinity::Neutral:
    default:
        return 1.0f;
    }
}

u32 BattleCalcs::getMagicBoostHeal(u32& magic)
{
    u32 index = (magic - 1) / 5;
    index = std::clamp(index, (u32)0, (u32)19);
    return magicBoostTableHeal[index];
}
