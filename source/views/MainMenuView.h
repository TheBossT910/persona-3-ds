#pragma once
#include "components/menu/MainMenuComponent.h"
#include "views/BaseView.h"

class MainMenuView : public BaseView
{
  private:
    PrintConsole console;
    MainMenuComponent mainMenuCmpt;
    bool isMainMenuCmptActive;
    int bg[3];

    // for silhouette animation
    int silhouetteX = -256;
    int silhouetteY = 192;

    // for bottom screen text animation
    int brightness = 0;

    // for fogBackground
    bool displayFog = false;
    int fogOpacity = 0;
    // NOTE: we use u16 to allow overflow (and naturally reset values back to 0)
    u16 waveAngle = 0;
    u16 currentRotation = 0;
    int baseSpeed = 20;
    int fluctuation = 50;

    GraphicsController* graphicsCtrl = GraphicsController::getInstance();

  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
};
