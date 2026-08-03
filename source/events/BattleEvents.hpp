#pragma once
#include "core/enums.h"
#include <aegis/system.hpp>

#include "./battleActions/BattleStartCondition.h"
#include "./battleActions/enemies/EnemyProfileDb.h"
#include "./battleActions/party/CharacterProfileDb.h"

namespace Event
{
struct ExecuteBattle : public etl::message<EventID::ExecuteBattle>
{
    CharacterProfile& player;
    std::vector<CharacterProfile>& characterProfiles;
    std::vector<EnemyProfile>& enemyProfiles;
    BattleStartCondition battleStartCondition;

    ExecuteBattle(CharacterProfile& iPlayer,
                  std::vector<CharacterProfile>& iCharacterProfiles,
                  std::vector<EnemyProfile>& iEnemyProfiles,
                  BattleStartCondition iBattleStartCondition)
        : player(iPlayer), characterProfiles(iCharacterProfiles), enemyProfiles(iEnemyProfiles),
          battleStartCondition(iBattleStartCondition)
    {
    }
};

// TODO: deprecate /project/source/battleActions/BattleResult.h in favour of this Event version
struct BattleResult : public etl::message<EventID::BattleResult>
{
    bool playerDied = false;
};
} // namespace Event
