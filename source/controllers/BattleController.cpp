#include "BattleController.h"
#include "./battleActions/skills/BattleCalcs.h"

#include "./helpers/random.h"
#include "core/globals.h"
#include <cstdlib>
#include <ctime>

BattleController::BattleController()
{
}

void BattleController::execute(Player* player,
                               std::vector<PartyMember*>* partyMembers,
                               std::vector<Enemy*>* enemies,
                               std::vector<BattleParticipant*>* battleParticipants,
                               BattleStartCondition battleStartCondition)
{
    active = true;

    std::string path =
        fatBasePath + "music/battle/" + (saveData.femcMode ? "wiping_all_out.pcm" : "mass_destruction.pcm");
    musicCtrl.init(path.c_str(), 0.0f, saveData.femcMode ? 78.315f : 84.767f);

    this->player = player;
    this->partyMembers = partyMembers;
    this->enemies = enemies;
    this->battleParticipants = battleParticipants;
    this->battleStartCondition = battleStartCondition;

    turnsTaken = 0;
    currentParticipantIndex = 0;
    selectedSkill = nullptr;
    pendingAlert.clear();
    battleResult = BattleResult();
    allOutAttackWasPossibleThisKnockDown = false;
    pendingPersonaSwitch = false;
    switchedPersonaThisTurn = false;
    personaBeforeSwitch = nullptr;

    calculateTurnOrder();

    currentParticipantTurn = battleParticipants->at(0);

    menuIndex = -1;
    phase = currentParticipantTurn->getInitalTurnPhase();
}

BattleResult BattleController::update(u32 keys)
{
    if (!active)
        return battleResult;

    switch (phase)
    {
    case BattlePhase::ChooseAction:
    {
        PartyMember* actor = static_cast<PartyMember*>(currentParticipantTurn);

        // render battleMenu
        battleMenuCmpt.loadActionOptions(&actions, actor->name);
        menuIndex = -1;
        menuIndex = (int)battleMenuCmpt.update(keys);

        if ((menuIndex != -1) && (keys & KEY_A) && actor->actorCanUse(actions[menuIndex]))
        {
            consoleClear();
            if (menuIndex == ACTION_ATTACK)
            {
                selectedSkill = actor->baseAttackAction;
                phase = BattlePhase::ChooseTarget;
            }
            else if (menuIndex == ACTION_GUARD)
            {
                TurnResult turnResult = guard.resolve(actor, nullptr);
                applyResult(turnResult);
                advanceTurn();
            }
            else if (menuIndex == ACTION_PERSONA)
            {
                phase = BattlePhase::ChooseSkill;
            }
            else if (menuIndex == ACTION_SWITCH)
            {
                if (switchedPersonaThisTurn)
                {
                    return battleResult;
                }
                phase = BattlePhase::ChoosePersona;
            }
        }
        break;
    }

    case BattlePhase::ChooseSkill:
    {
        PartyMember* actor = static_cast<PartyMember*>(currentParticipantTurn);

        // render battleMenu
        battleMenuCmpt.loadSkillOptions(actor->curPersona);
        //TODO: why not just return nullptr if nothing happens instead of setting -1 manually everywhere?

        menuIndex = -1;
        menuIndex = (int)battleMenuCmpt.update(keys);

        if ((menuIndex != -1) && (keys & KEY_A))
        {
            Skill* s = actor->curPersona->skills[menuIndex];

            s32* resource;
            if (s->skillRace == SkillRace::mag)
                resource = &actor->sp;
            else
                resource = &actor->hp;

            bool canAfford = *resource >= s->cost;
            if (canAfford)
            {
                *resource -= s->cost;
                selectedSkill = s;
                menuIndex = 0;
                phase = BattlePhase::ChooseTarget;
            }
            else
            {
                pendingAlert = (s->skillRace == SkillRace::mag) ? "Not enough SP\n" : "Not enough HP\n";
                alertReturnPhase = BattlePhase::ChooseSkill;
                battleMenuCmpt.reset();
                phase = BattlePhase::ShowAlert;
            }
        }

        if (keys & KEY_B)
            phase = BattlePhase::ChooseAction;
        break;
    }

    case BattlePhase::ChoosePersona:
    {
        Player* actor = static_cast<Player*>(currentParticipantTurn);

        //inital capture of start persona of this turn, so that if we switch personas and
        //then switch back to the same old starting persona it wont count it as a switch
        if (personaBeforeSwitch == nullptr)
        {
            personaBeforeSwitch = actor->curPersona;
        }

        battleMenuCmpt.loadPersonaOptions(&actor->personas);
        menuIndex = -1;
        menuIndex = (int)battleMenuCmpt.update(keys);

        if ((menuIndex != -1) && (keys & KEY_A))
        {
            if (actor->curPersona == actor->personas[menuIndex])
            {
                pendingAlert = "Already using this Persona\n";
                alertReturnPhase = BattlePhase::ChoosePersona;
            }
            else
            {
                actor->curPersona = actor->personas[menuIndex];
                pendingPersonaSwitch = (actor->curPersona != personaBeforeSwitch);
                pendingAlert = "Switched to: " + actor->curPersona->name + "\n";
                alertReturnPhase = BattlePhase::ChooseAction;
            }

            battleMenuCmpt.reset();
            phase = BattlePhase::ShowAlert;
        }

        if (keys & KEY_B)
            phase = BattlePhase::ChooseAction;
        break;
    }

    case BattlePhase::ChooseTarget:
    {
        PartyMember* actor = static_cast<PartyMember*>(currentParticipantTurn);

        bool healTarget = selectedSkill && (selectedSkill->skillType == SkillType::Heal ||
                                            selectedSkill->skillType == SkillType::MultiHeal);

        std::vector<BattleParticipant*> targets;
        if (healTarget)
        {
            for (PartyMember* partyMember : *partyMembers)
            {
                targets.push_back(partyMember);
            }
        }
        else
        {
            for (Enemy* enemy : *enemies)
            {
                targets.push_back(enemy);
            }
        }

        targets.erase(std::remove_if(targets.begin(), targets.end(), [](BattleParticipant* t) { return t->hp <= 0; }),
                      targets.end());

        battleMenuCmpt.loadTargetOptions(&targets, healTarget);
        menuIndex = -1;
        menuIndex = (int)battleMenuCmpt.update(keys);

        if (menuIndex != -1 && (keys & KEY_A))
        {
            if (isSingleTarget(selectedSkill->skillType))
            {
                BattleParticipant* selectedTarget = targets[menuIndex];

                targets.clear();
                targets.push_back(selectedTarget);
            }

            // Check so you cant heal target that has max hp
            if (selectedSkill &&
                (selectedSkill->skillType == SkillType::Heal || selectedSkill->skillType == SkillType::MultiHeal))
            {
                bool canHealAnyTarget = false;
                for (BattleParticipant* target : targets)
                {
                    if (target->hp < target->maxHp)
                    {
                        canHealAnyTarget = true;
                        break;
                    }
                }
                if (!canHealAnyTarget)
                    return battleResult;
            }

            bool usingBaseAttack = selectedSkill == actor->baseAttackAction;

            for (BattleParticipant* target : targets)
            {
                TurnResult turnResult =
                    (usingBaseAttack) ? attack.resolve(actor, target) : persona.resolve(actor, target, selectedSkill);
                applyResult(turnResult, target);
            }

            advanceTurn();
        }

        if (keys & KEY_B)
        {
            phase = (selectedSkill == actor->baseAttackAction) ? BattlePhase::ChooseAction : BattlePhase::ChooseSkill;
            menuIndex = -1;
        }
        break;
    }

    case BattlePhase::ConfirmAllOutAttack:
    {
        battleMenuCmpt.loadAllOutAttackConfirmation();
        menuIndex = -1;
        menuIndex = (int)battleMenuCmpt.update(keys);

        if (menuIndex != -1 && (keys & KEY_A))
        {
            allOutAttackWasPossibleThisKnockDown = true;

            //if yes
            if (menuIndex == 0)
            {
                std::vector<BattleParticipant*> aliveEnemies = getAliveEnemies();

                uint8_t participantCount = 0;
                for (PartyMember* partyMember : *partyMembers)
                {
                    if (partyMember->canParticipateInAllOutAttack())
                    {
                        participantCount++;
                    }
                }

                for (BattleParticipant* enemy : aliveEnemies)
                {
                    //TODO: get participant count over virtual canParticipate method
                    u32 damage = BattleCalcs::allOutAttack(*player, *enemy, participantCount);
                    enemy->knockedDown = false;
                    TurnResult turnResult = {true, -(s32)damage, false, enemy->name + ": "};
                    applyResult(turnResult, enemy);
                }

                advanceTurn();
            } //no
            else
            {
                currentParticipantTurn->oneMore = false;
                battleMenuCmpt.reset();
                phase = BattlePhase::ChooseAction;
            }
        }
        break;
    }

    case BattlePhase::ShowAlert:
    {
        battleMenuCmpt.loadAlertOptions(pendingAlert);
        battleMenuCmpt.update(keys);
        if (battleMenuCmpt.isAlertExpired(120))
        {
            pendingAlert.clear();
            battleMenuCmpt.reset();
            phase = alertReturnPhase;
        }
        break;
    }

    case BattlePhase::EnemyTurn:
    {
        Enemy* enemy = static_cast<Enemy*>(currentParticipantTurn);
        Skill* skill = enemy->pickSkill();
        std::vector<BattleParticipant*> targets;
        //TODO: branch in future if using healing / buff
        for (PartyMember* partyMember : *partyMembers)
        {
            targets.push_back(partyMember);
        }

        BattleParticipant* target = enemy->pickTarget(targets);
        TurnResult turnResult = enemy->resolve(target, skill);
        applyResult(turnResult, target);
        advanceTurn();
        //cant be set in advanceTurn, needs to be specifically cleared when an enemy has recoverd
        allOutAttackWasPossibleThisKnockDown = false;
        break;
    }

    case BattlePhase::Done:
        break;
    }

    return battleResult;
}

void BattleController::exit()
{
    consoleClear();
    musicCtrl.pause();
    active = false;
    phase = BattlePhase::Done;

    currentParticipantTurn = nullptr;
    player = nullptr;
    battleParticipants = nullptr;
    enemies = nullptr;
    partyMembers = nullptr;
}

void BattleController::applyResult(const TurnResult& turnResult, BattleParticipant* target)
{
    if (!turnResult.log.empty())
        pendingAlert += turnResult.log + "\n";

    if (turnResult.hit && target && turnResult.hpDelta != 0)
    {
        s32 hpBefore = target->hp;
        s32 actuallyHealedHp = turnResult.hpDelta;
        target->hp += turnResult.hpDelta;

        if (target->hp > target->maxHp)
        {
            target->hp = target->maxHp;
            actuallyHealedHp = target->maxHp - hpBefore;
        }

        char buf[48];
        if (turnResult.hpDelta < 0)
            std::sprintf(buf, "Damage: %ld\n", (long)-turnResult.hpDelta);
        else
            std::sprintf(buf, "HP healed: %ld\n", (long)actuallyHealedHp);
        pendingAlert += buf;

        std::sprintf(buf, "%s HP: %ld\n", target->name.c_str(), (long)target->hp);
        pendingAlert += buf;
    }

    if (turnResult.oneMore)
    {
        pendingAlert += "One More!\n";
        currentParticipantTurn->oneMore = true;
    }
}

void BattleController::advanceTurn()
{
    handleDeadParticipants();
    if (!active)
        return;

    //all out attack availability check
    if (allEnemiesKnockedDown() && !allOutAttackWasPossibleThisKnockDown)
    {
        alertReturnPhase = BattlePhase::ChooseAction;
        battleMenuCmpt.reset();
        setNextPhase(BattlePhase::ConfirmAllOutAttack);
        return;
    }

    pendingAlert += "Previous attacker: " + currentParticipantTurn->name + "\n";

    selectedSkill = nullptr;

    switchedPersonaThisTurn = pendingPersonaSwitch;
    pendingPersonaSwitch = false;

    if (currentParticipantTurn->oneMore)
    {
        currentParticipantTurn->oneMore = false;
        BattlePhase nextPhase = currentParticipantTurn->getInitalTurnPhase();

        setNextPhase(nextPhase);
        return;
    }

    switchedPersonaThisTurn = false;
    personaBeforeSwitch = nullptr;

    currentParticipantTurn->knockedDown = false;

    u32 next = (currentParticipantIndex + 1) % battleParticipants->size();
    while (battleParticipants->at(next)->hp <= 0)
        next = (next + 1) % battleParticipants->size();

    currentParticipantIndex = next;
    turnsTaken++;
    currentParticipantTurn = battleParticipants->at(next);

    menuIndex = -1;
    BattlePhase nextPhase = currentParticipantTurn->getInitalTurnPhase();

    setNextPhase(nextPhase);
}

void BattleController::setNextPhase(BattlePhase nextPhase)
{
    if (!pendingAlert.empty())
    {
        alertReturnPhase = nextPhase;
        battleMenuCmpt.reset();
        phase = BattlePhase::ShowAlert;
    }
    else
    {
        phase = nextPhase;
    }
}

// TODO:: potentially rework since this would do a double turn if called again after 1st turn
void BattleController::calculateTurnOrder()
{
    // random boost from 1.2 to 1.4 that priorizes party
    float boost = 1.2f + (randf() * 0.2f);

    for (BattleParticipant* battleParticipant : *battleParticipants)
    {
        battleParticipant->setCurrentTurnOrderAgility(boost);
    }

    std::sort(partyMembers->begin(), partyMembers->end(), getParticipantByHigherAgility);
    std::sort(enemies->begin(), enemies->end(), getParticipantByHigherAgility);

    battleParticipants->clear();

    if (battleStartCondition == BattleStartCondition::PartyAdvantage)
    {
        for (auto p : *partyMembers)
        {
            battleParticipants->push_back(p);
        }
        for (auto e : *enemies)
        {
            battleParticipants->push_back(e);
        }
    }
    else if (battleStartCondition == BattleStartCondition::EnemyAdvantage)
    {
        for (auto e : *enemies)
        {
            battleParticipants->push_back(e);
        }
        for (auto p : *partyMembers)
        {
            battleParticipants->push_back(p);
        }
    }
    else
    {
        std::merge(partyMembers->begin(),
                   partyMembers->end(),
                   enemies->begin(),
                   enemies->end(),
                   std::back_inserter(*battleParticipants),
                   getParticipantByHigherAgility);
    }
}

void BattleController::handleDeadParticipants()
{
    for (u32 i = 0; i < battleParticipants->size(); i++)
    {
        if (battleParticipants->at(i)->hp > 0)
        {
            continue;
        }

        BattleParticipant* dead = battleParticipants->at(i);

        dead->onDead(battleResult);

        if (battleResult.playerDied)
        {
            exit();
            return;
        }
    }

    bool enemiesAlive = false;
    for (BattleParticipant* enemy : *enemies)
    {
        if (enemy->hp > 0)
        {
            enemiesAlive = true;
            break;
        }
    }

    if (!enemiesAlive)
    {
        exit();
        return;
    }
}

std::vector<BattleParticipant*> BattleController::getAliveEnemies()
{
    std::vector<BattleParticipant*> alive;
    for (BattleParticipant* enemy : *enemies)
    {
        if (enemy->hp > 0)
            alive.push_back(enemy);
    }
    return alive;
}

bool BattleController::allEnemiesKnockedDown()
{
    uint8_t aliveCount = 0;
    uint8_t knockedDownCount = 0;
    for (BattleParticipant* enemy : *enemies)
    {
        if (enemy->hp > 0)
        {
            aliveCount++;
            if (enemy->knockedDown)
                knockedDownCount++;
        }
    }

    if (knockedDownCount >= aliveCount)
        return true;

    return false;
}

bool BattleController::isSingleTarget(SkillType type)
{
    switch (type)
    {
    case SkillType::RegularAttack:
    case SkillType::Attack:
    case SkillType::Heal:
    case SkillType::Buff:
    case SkillType::Debuff:
        return true;
    default:
        return false;
    }
}
