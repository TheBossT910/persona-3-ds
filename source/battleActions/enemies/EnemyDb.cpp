#include "EnemyDb.h"
#include "../armours/ArmourDb.h"
#include "../personas/PersonaDb.h"
#include "../shoes/ShoeDb.h"
#include "../skills/SkillDb.h"

Enemy EnemyDb::mercilessMaya;
Enemy EnemyDb::cowardlyMaya;

Skill* EnemyDb::mercilessMayaSkills[1];
Skill* EnemyDb::cowardlyMayaSkills[1];

void EnemyDb::Initialize()
{
    /*--------------MERCILESS MAYA--------------*/
    mercilessMaya.name = "Merciless Maya";
    mercilessMaya.maxHp = 83;
    mercilessMaya.hp = 83;
    mercilessMaya.maxSp = 15;
    mercilessMaya.sp = 15;
    mercilessMaya.lv = 2;
    mercilessMaya.battleStats.st = 2;
    mercilessMaya.battleStats.ma = 4;
    mercilessMaya.battleStats.en = 3;
    mercilessMaya.battleStats.ag = 2;
    mercilessMaya.battleStats.lu = 2;
    mercilessMaya.baseAttackAction = &SkillDb::strikeAttack;
    mercilessMaya.armour = ArmourDb::enemyArmour;
    mercilessMaya.shoe = ShoeDb::enemyShoe;

    mercilessMayaSkills[0] = &SkillDb::agi;
    mercilessMaya.skill = mercilessMayaSkills;
    mercilessMaya.skillCount = 1;

    mercilessMaya.battleStats.affinities[(u32)Element::Slash] = BattleStats::Weak;
    mercilessMaya.battleStats.affinities[(u32)Element::Strike] = BattleStats::Neutral;
    mercilessMaya.battleStats.affinities[(u32)Element::Pierce] = BattleStats::Neutral;
    mercilessMaya.battleStats.affinities[(u32)Element::Fire] = BattleStats::Resist;
    mercilessMaya.battleStats.affinities[(u32)Element::Ice] = BattleStats::Weak;
    mercilessMaya.battleStats.affinities[(u32)Element::Electricity] = BattleStats::Null;
    mercilessMaya.battleStats.affinities[(u32)Element::Wind] = BattleStats::Neutral;
    mercilessMaya.battleStats.affinities[(u32)Element::Light] = BattleStats::Neutral;
    mercilessMaya.battleStats.affinities[(u32)Element::Dark] = BattleStats::Neutral;
    mercilessMaya.battleStats.affinities[(u32)Element::Almighty] = BattleStats::Neutral;

    /*--------------COWARDLY MAYA--------------*/
    cowardlyMaya.name = "Cowardly Maya";
    cowardlyMaya.maxHp = 83;
    cowardlyMaya.hp = 83;
    cowardlyMaya.maxSp = 20;
    cowardlyMaya.sp = 20;
    cowardlyMaya.lv = 2;
    cowardlyMaya.battleStats.st = 2;
    cowardlyMaya.battleStats.ma = 3;
    cowardlyMaya.battleStats.en = 3;
    cowardlyMaya.battleStats.ag = 2;
    cowardlyMaya.battleStats.lu = 2;
    cowardlyMaya.baseAttackAction = &SkillDb::strikeAttack;
    cowardlyMaya.armour = ArmourDb::enemyArmour;
    cowardlyMaya.shoe = ShoeDb::enemyShoe;

    cowardlyMayaSkills[0] = &SkillDb::bufu;
    cowardlyMaya.skill = cowardlyMayaSkills;
    cowardlyMaya.skillCount = 1;

    cowardlyMaya.battleStats.affinities[(u32)Element::Slash] = BattleStats::Weak;
    cowardlyMaya.battleStats.affinities[(u32)Element::Strike] = BattleStats::Neutral;
    cowardlyMaya.battleStats.affinities[(u32)Element::Pierce] = BattleStats::Neutral;
    cowardlyMaya.battleStats.affinities[(u32)Element::Fire] = BattleStats::Weak;
    cowardlyMaya.battleStats.affinities[(u32)Element::Ice] = BattleStats::Neutral;
    cowardlyMaya.battleStats.affinities[(u32)Element::Electricity] = BattleStats::Neutral;
    cowardlyMaya.battleStats.affinities[(u32)Element::Wind] = BattleStats::Neutral;
    cowardlyMaya.battleStats.affinities[(u32)Element::Light] = BattleStats::Neutral;
    cowardlyMaya.battleStats.affinities[(u32)Element::Dark] = BattleStats::Neutral;
    cowardlyMaya.battleStats.affinities[(u32)Element::Almighty] = BattleStats::Neutral;
}
