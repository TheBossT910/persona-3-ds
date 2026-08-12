#pragma once
#include "controllers/MusicController.hpp"
#include "controllers/TextController.hpp"
#include "core/globals.hpp"
#include "views/BaseView.hpp"

#include "components/GraphicsComponent.hpp"
#include "managers/MathManager.hpp"

class IntroView : public BaseView
{
  private:
    Sprite logoSprite[2];
    int bg[4];

    // sub screen
    int bgSubLogo;
    int bgSubSky;
    int bgSubText;
    uint16_t* textVideoBufferSub;

    // for silhouette animation
    int silhouetteX = -256;
    int silhouetteY = 192;

    // for bottom screen text animation
    bool animateText = false;
    int duration = 4;
    int durationCounter = 0;
    int textAlpha = 0;
    int textAlphaDirection = 0;

    // for logoSprite
    bool displayLogo = false;
    int logoOpacity = 0;

    // for overlayBackground
    bool displayOverlay = false;
    int overlayOpacity = 0;
    // NOTE: we use u16 to allow overflow (and naturally reset values back to 0)
    u16 waveAngle = 0;
    u16 currentRotation = 0;
    int baseSpeed = 20;
    int fluctuation = 50;

    ae::Entity* intro = nullptr;
    GraphicsComponent* graphics = nullptr;

    MathManager& math = MathManager::GetInstance();
    MusicController* musicCtrl = MusicController::getInstance();
    Font* font = textCtrl->loadFont("cosmetica", 16);
    TextController* textCtrl = TextController::getInstance();

  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
};
