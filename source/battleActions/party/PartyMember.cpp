#include "PartyMember.h"
#include "../skills/BattleCalcs.h"

PartyMember::PartyMember(const CharacterProfile& iCharacterProfile) : characterProfile(iCharacterProfile)

{
    name = characterProfile.name;
    maxHp = characterProfile.maxHp;
    hp = characterProfile.hp;
    maxSp = characterProfile.maxSp;
    sp = characterProfile.sp;
    lv = characterProfile.lv;

    baseAttackAction = characterProfile.baseAttackAction;
    participantType = characterProfile.participantType;

    armourType = characterProfile.armourType;
    armour = characterProfile.armour;
    shoe = characterProfile.shoe;
    weaponType = characterProfile.weaponType;
    weapon = characterProfile.weapon;
    personas = characterProfile.personas;
    curPersona = characterProfile.curPersona;
}

float PartyMember::calculateBaseDamage(BattleParticipant& defender, Skill& skill)
{
    u32 atk = BattleCalcs::getAtk(curPersona->battleStats, skill);
    float levelDifference = BattleCalcs::getLevelDifference(lv, defender.lv);
    float affinityMtp = BattleCalcs::getAffinityMtp(*defender.getBattleStats(), skill);
    u32 movePower = (skill.skillType == SkillType::RegularAttack) ? (weapon.weaponPower / 2.0f) : skill.movePower;
    return floor(sqrt((float)(movePower * 15 * atk) / defender.getBattleStats()->en) * 2 * levelDifference *
                 affinityMtp);
}

float PartyMember::getTeamMultiplier()
{
    return 1.0f;
}

void PartyMember::setCurrentTurnOrderAgility(float boost)
{
    currentTurnOrderAgility = curPersona->battleStats.ag * boost;
}

BattlePhase PartyMember::getInitalTurnPhase()
{
    return BattlePhase::ChooseAction;
}

void PartyMember::onDead(BattleResult& battleResult)
{
    //TODO: clear ailments, buffs etc in the future as they get wiped when revived
}

bool PartyMember::canParticipateInAllOutAttack()
{
    if (hp <= 0 || knockedDown)
    {
        return false;
    }

    return true;
}

bool PartyMember::actorCanUse(ActionBase* action)
{
    return action->possibleUsers == ParticipantType::Party;
}
