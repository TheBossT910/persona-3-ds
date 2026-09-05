#pragma once
#include "components/GraphicsComponent.hpp"
#include "components/TextComponent.hpp"
#include "controllers/MusicController.hpp"
#include "views/BaseView.hpp"

#include <maxmod9.h>

class SignContractView : public BaseView
{
  private:
    int bg[3];
    bool isLastName = true;
    bool isNameConfirmed = false;
    int lastNameIndex = 0;
    int firstNameIndex = 0;

    // sfx
    mm_sfxhand sfxMenuHandle = 0;
    mm_sfxhand sfxSelectHandle = 0;
    mm_sfxhand sfxCancelHandle = 0;

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
