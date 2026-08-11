#pragma once
#include "battleActions/BattleParticipant.h"
#include "battleActions/actions/ActionBase.h"
#include "battleActions/personas/PersonaBase.h"
#include "components/menu/BaseMenu.h"
#include "components/menu/BattleMenuComponent.h"
#include <etl/vector.h>

class BattleMenuComponent : public BaseMenu
{
  private:
    BattleMenuComponent() {};
    virtual ~BattleMenuComponent() = default;
    static BattleMenuComponent* instance;

    BattleMenuOptions loadedOption = BattleMenuOptions::NONE;

    etl::vector<MenuOption, 10> battleOptions;
    int alertStartFrame = 0;
    bool messagePrinted = false;

    // option handlers
    int battleOptionSelected();

  public:
    static void create();
    static void destroy();
    static BattleMenuComponent* getInstance();

    void init(int iBgSlot, bool* isActive, TextComponent* iText, const std::string& iPauseMessage = "") override;
    ViewState update(int keys) override;
    // option loaders
    void loadActionOptions(std::array<ActionBase*, 4>* actions, std::string name);
    void loadSkillOptions(PersonaBase* persona);
    void loadPersonaOptions(etl::vector<PersonaBase*, 13>* personas);
    void loadTargetOptions(etl::vector<BattleParticipant*, 13>* targets, bool healTarget);
    void loadAllOutAttackConfirmation();
    void loadAlertOptions(const std::string& text);
    bool isAlertExpired(int durationFrames) const;
    void reset();
};
