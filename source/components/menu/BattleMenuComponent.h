#pragma once
#include "battleActions/BattleParticipant.h"
#include "battleActions/actions/ActionBase.h"
#include "battleActions/personas/PersonaBase.h"
#include "components/menu/BaseMenu.h"
#include "components/menu/BattleMenuComponent.h"

class BattleMenuComponent : public BaseMenu
{
  protected:
    void loadBg(int bgIndex) override;

  private:
    BattleMenuOptions loadedOption = BattleMenuOptions::NONE;

    std::vector<MenuOption> battleOptions;
    int alertStartFrame = 0;

    // option handlers
    int battleOptionSelected();

  public:
    void init(int iBgSlot, bool* isActive, const std::string& iPauseMessage = "") override;
    ViewState update(int keys) override;
    // option loaders
    void loadActionOptions(std::array<ActionBase*, 4>* actions, std::string name);
    void loadSkillOptions(PersonaBase* persona);
    void loadPersonaOptions(std::vector<PersonaBase*>* personas);
    void loadTargetOptions(std::vector<BattleParticipant*>* targets, bool healTarget);
    void loadAllOutAttackConfirmation();
    void loadAlertOptions(const std::string& text);
    bool isAlertExpired(int durationFrames) const;
    void reset();
};
