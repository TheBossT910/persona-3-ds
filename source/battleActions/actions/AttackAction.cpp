#include "AttackAction.hpp"
#include "./battleActions/skills/BattleCalcs.hpp"
#include <stdlib.h>

//ignore skill here
TurnResult AttackAction::resolve(PartyMember* user, BattleParticipant* target, Skill* skill)
{
    Skill* atk = user->baseAttackAction;
    Enemy* enemy = static_cast<Enemy*>(target);

    u32 accuracy = BattleCalcs::hitrate(*user, *target, *atk);
    bool hit = accuracy > u32(rand() % 100);

    if (!hit)
        return {false, 0, false, "Miss"};

    u32 damage = BattleCalcs::attack(*user, *target, *atk);
    bool oneMore = false;

    u32 affinity = enemy->battleStats.affinities[(u32)atk->element];
    if (affinity == BattleStats::Affinity::Weak && !enemy->knockedDown)
    {
        oneMore = true;
        enemy->knockedDown = true;
    }

    return {true, -(s32)damage, oneMore, atk->name};
}
