#pragma once

#include <algorithm>
#include <array>
#include <nds.h>
#include <string>
#include <vector>

#include "./battleActions/actions/AttackAction.h"
#include "./battleActions/actions/Guard.h"
#include "./battleActions/actions/PersonaAction.h"
#include "./battleActions/actions/SwitchPersona.h"

#include "./battleActions/BattleParticipant.h"
#include "./battleActions/BattlePhase.h"
#include "./battleActions/BattleResult.h"
#include "./battleActions/BattleStartCondition.h"
#include "./battleActions/TurnResult.h"
#include "./battleActions/enemies/Enemy.h"
#include "./battleActions/party/CharacterProfileDb.h"
#include "./battleActions/party/PartyMember.h"
#include "./battleActions/party/Player.h"

class BattleController
{
  private:
    static constexpr u32 ACTION_ATTACK = 0;
    static constexpr u32 ACTION_GUARD = 1;
    static constexpr u32 ACTION_PERSONA = 2;
    static constexpr u32 ACTION_SWITCH = 3;

    bool active = false;
    u32 turnsTaken = 0;

    BattlePhase phase;
    BattleResult battleResult;
    BattleParticipant* currentParticipantTurn = nullptr;
    u32 currentParticipantIndex = 0;

    int menuIndex = 0;
    Skill* selectedSkill = nullptr;

    bool pendingPersonaSwitch = false;
    bool switchedPersonaThisTurn = false;
    PersonaBase* personaBeforeSwitch = nullptr;

    std::string pendingAlert;
    BattlePhase alertReturnPhase = BattlePhase::Done;

    bool allOutAttackWasPossibleThisKnockDown = false;

    std::vector<BattleParticipant*>* battleParticipants = nullptr;
    std::vector<Enemy*>* enemies = nullptr;
    std::vector<PartyMember*>* partyMembers = nullptr;
    Player* player = nullptr;

    BattleStartCondition battleStartCondition = BattleStartCondition::Even;

    AttackAction attack;
    Guard guard;
    PersonaAction persona;
    SwitchPersona switchPersona;

    std::array<ActionBase*, 4> actions = {&attack, &guard, &persona, &switchPersona};

    void applyResult(const TurnResult& r, BattleParticipant* target = nullptr);
    void advanceTurn();
    void setNextPhase(BattlePhase nextPhase);
    void calculateTurnOrder();
    void handleDeadParticipants();
    std::vector<BattleParticipant*> getAliveEnemies();
    bool allEnemiesKnockedDown();
    bool isSingleTarget(SkillType type);

    static bool getParticipantByHigherAgility(BattleParticipant* a, BattleParticipant* b)
    {
        return a->currentTurnOrderAgility > b->currentTurnOrderAgility;
    };

  public:
    bool isActive() const
    {
        return active;
    }
    BattlePhase getPhase() const
    {
        return phase;
    }

    void execute(Player* player,
                 std::vector<PartyMember*>* partyMembers,
                 std::vector<Enemy*>* enemies,
                 std::vector<BattleParticipant*>* battleParticipants,
                 BattleStartCondition battleStartCondition);
    BattleResult update(u32 keys);
    void exit();

    BattleController();
    ~BattleController()
    {
    }
};
