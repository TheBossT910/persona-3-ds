#pragma once
#include "../BattleParticipant.h"
#include "../TurnResult.h"
#include "EnemyProfile.h"
#include <nds.h>
#include <vector>

struct Enemy : BattleParticipant
{
    Skill** skill;
    u32 skillCount;
    BattleStats battleStats;

    EnemyProfile enemyProfile;

    Enemy(const EnemyProfile& iEnemyProfile);

    Skill* pickSkill();
    BattleParticipant* pickTarget(std::vector<BattleParticipant*>& partyMembers);
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
