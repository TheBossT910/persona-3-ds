#pragma once
#include "../BattleParticipant.hpp"
#include "../TurnResult.hpp"
#include "EnemyProfile.hpp"
#include <etl/vector.h>
#include <nds.h>

struct Enemy : BattleParticipant
{
    Skill** skill;
    u32 skillCount;
    BattleStats battleStats;

    EnemyProfile enemyProfile;

    Enemy(const EnemyProfile& iEnemyProfile);

    Skill* pickSkill();
    BattleParticipant* pickTarget(etl::vector<BattleParticipant*, 13>& partyMembers);
    TurnResult resolve(BattleParticipant* target, Skill* skill);

    BattleStats* getBattleStats() override
    {
        return &battleStats;
    }

    float calculateBaseDamage(BattleParticipant& defender, Skill& skill) override;
    float getTeamMultiplier() override;
    BattlePhase getInitalTurnPhase() override;
    void onDead(Event::BattleResult& battleResult) override;
    void setCurrentTurnOrderAgility(float boost);

    virtual ~Enemy() = default;
};
