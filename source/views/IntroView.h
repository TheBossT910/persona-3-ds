#pragma once
#include "core/globals.h"
#include "views/BaseView.h"

class IntroView : public BaseView
{
  private:
    Sprite logoSprite[2];
    int bg[4];

    // sub screen
    int bgSubLogo;
    int bgSubSky;
    PrintConsole console;

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

  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
};
