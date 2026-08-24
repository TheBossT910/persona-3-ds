#pragma once
#include "../BattleParticipant.hpp"
#include "../ParticipantType.hpp"
#include "../actions/ActionBase.hpp"
#include "../armours/Armour.hpp"
#include "../personas/PersonaBase.hpp"
#include "../shoes/Shoe.hpp"
#include "../weapons/Weapon.hpp"
#include "CharacterProfile.hpp"
#include <etl/vector.h>
#include <nds.h>

struct PartyMember : BattleParticipant
{
    ArmourType armourType;
    etl::vector<PersonaBase*, 13> personas;
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
    void onDead(Event::BattleResult& battleResult) override;
    bool canParticipateInAllOutAttack();
    virtual bool actorCanUse(ActionBase* action);

    ~PartyMember()
    {
    }
};
