#pragma once

#include "components/menu/BattleMenuComponent.h"
#include "controllers/MusicController.h"

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
#include "./battleActions/BattleStartCondition.h"
#include "./battleActions/TurnResult.h"
#include "./battleActions/enemies/Enemy.h"
#include "./battleActions/enemies/EnemyProfileDb.h"
#include "./battleActions/party/CharacterProfileDb.h"
#include "./battleActions/party/PartyMember.h"
#include "./battleActions/party/Player.h"

#include "core/enums.h"            // EventIDs
#include "core/routerIDs.hpp"      // RouterIDs
#include "events/BattleEvents.hpp" // Events
#include "events/GenericEvents.hpp"
#include <aegis/system.hpp>

// TODO: check for dead code/unfeasible paths
// TODO: update Javadoc

class BattleSystem : public ae::SystemRouter<BattleSystem, Event::ExecuteBattle>, public ae::Singleton<BattleSystem>
{
  public:
    void Init() override;

    void Shutdown() override;

    // TODO: update javadoc
    /**
     * @brief Actual Battle
     *
     * This is where the battle is controlled from.
     *
     * @details
     * Controlls battle. Theres a phase switch which decides which menu point you are currently on
     * or stuff like enemy turn.
     * We have a system to build alerts (with a pendingAlert string) to then display these
     * in the ShowAlert phase after each action.
     * battleMenuCmpt is used to show the diffrent menu option in console.
     *
     * @author Nolan Kolb (TrueGiles / themoonwalker8692)
     */
    void Update(ae::fixed_t /*dt*/) override;

    // TODO: update javadoc
    /**
     * @brief init function of battlesystem
     *
     * This gets called each time a new battle is made and resets
     * certain varaibles.
     *
     * @param player set player, we need to specifically know him for some things all the time
     * @param partyMembers all players currently on the field, handling
     * @param enemies all enemies
     * @param battleParticipants all enemies and partyMembers
     * @param battleStartCondition conditon like player advantage, enemy advante or even. used to decide turn order
     *
     * @details
     * Called before every new battle. At the moment we give in actuall battle participants which sucks
     * massivley. In the future i just want to pass participant profiles so the BattleSystem
     * actually just manages everything itself.
     * Then proceeds to set music, setting variables and doing various cleanup.
     * Finally turn order is calculated with the battleStartConditon and battle gets started.
     *
     * @author Nolan Kolb (themoonwalker8692 / TrueGiles)
    */
    void on_receive(const Event::ExecuteBattle& msg);

    // TODO: add javadoc
    void on_receive(const Event::SetTextVideoBufferSub& msg);

    void on_receive_unknown(const etl::imessage&);

    bool IsActive();

  private:
    friend class Singleton<BattleSystem>;
    BattleSystem() : SystemRouter(kBattleSystemRouterID)
    {
    }

    static constexpr u32 ACTION_ATTACK = 0;
    static constexpr u32 ACTION_GUARD = 1;
    static constexpr u32 ACTION_PERSONA = 2;
    static constexpr u32 ACTION_SWITCH = 3;

    uint16_t* textVideoBufferSub;

    u32 turnsTaken = 0;

    BattlePhase phase;
    Event::BattleResult battleResult;

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

    // Current battle data
    std::vector<BattleParticipant*> battleParticipants;
    std::vector<Enemy*> enemies;
    std::vector<PartyMember*> partyMembers;
    Player* player = nullptr;

    BattleStartCondition battleStartCondition = BattleStartCondition::Even;

    // Actions
    AttackAction attack;
    Guard guard;
    PersonaAction persona;
    SwitchPersona switchPersona;

    std::array<ActionBase*, 4> actions = {&attack, &guard, &persona, &switchPersona};

    // Internal helpers
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
    }

    MusicController* musicCtrl = MusicController::getInstance();
    BattleMenuComponent* battleMenuCmpt = BattleMenuComponent::getInstance();
};
