#pragma once
#include "../BattleParticipant.h"
#include "../ParticipantType.h"
#include "../actions/ActionBase.h"
#include "../armours/Armour.h"
#include "../personas/PersonaBase.h"
#include "../shoes/Shoe.h"
#include "../weapons/Weapon.h"
#include "CharacterProfile.h"
#include <nds.h>

struct PartyMember : BattleParticipant
{
    ArmourType armourType;
    std::vector<PersonaBase*> personas;
    PersonaBase* curPersona;
    WeaponType weaponType;
    Weapon weapon;

    bool guarding = false;

    CharacterProfile characterProfile;

    PartyMember(const CharacterProfile& iCharacterProfile);

    BattleStats* getBattleStats() override
    {
        return &curPersona->battleStats;
    }

    float calculateBaseDamage(BattleParticipant& defender, Skill& skill) override;
    float getTeamMultiplier() override;
    void setCurrentTurnOrderAgility(float boost) override;
    BattlePhase getInitalTurnPhase() override;
    void onDead(BattleResult& battleResult) override;
    bool canParticipateInAllOutAttack();
    virtual bool actorCanUse(ActionBase* action);

    ~PartyMember()
    {
    }
};
