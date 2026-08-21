#pragma once
#include "controllers/MusicController.hpp"
#include "views/BaseView.hpp"
#include <maxmod9.h>
#include <nds/arm9/console.h>
#include <nds/arm9/keyboard.h>

#include "components/GraphicsComponent.hpp"
#include "components/TextComponent.hpp"

class SignContractView : public BaseView
{
  private:
    int bg[3];
    bool isLastName = true;
    bool isNameConfirmed = false;
    int lastNameIndex = 0;
    int firstNameIndex = 0;

    // sfx
    mm_sfxhand sfxMenuHandle;
    mm_sfxhand sfxSelectHandle;
    mm_sfxhand sfxCancelHandle;

    // text
    std::string FONT_NAME = "cosmetica";
    int FONT_SIZE = 12;
    std::string animText;
    std::string displayText;

    ae::Entity* signContract = nullptr;
    GraphicsComponent* graphics = nullptr;
    TextComponent* text = nullptr;

    MusicController* musicCtrl = MusicController::getInstance();

    void cancelSFX();

  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
};
