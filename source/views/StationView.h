#pragma once
#include "components/ui/MenuHUDScreen.h"
#include "controllers/CharacterController.h"
#include "controllers/UIController.h"
#include "environments/station.h"
#include "views/BaseView3D.h"
#include <nds/arm9/console.h>

class StationView : public BaseView3D
{
  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
    void setupEnvironment() override;

  private:
    touchPosition touch;

    ViewPhase phase;
    bool prevPauseState;
    bool prevEnvironmentState;

    // sub screen
    int bgSharedSub1;
    int bgSharedSub2;
    int bgSharedSub3;
    PrintConsole console;

    // 3D
    station_Environment stationEnv;

    CharacterController* playerCtrl;
    CameraPosition camPos;
    const float tileSize = 0.062500f;
    const float worldOffsetX = STATION_WORLD_OFFSET_X;
    const float worldOffsetZ = STATION_WORLD_OFFSET_Z;
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    const float speed = 0.02f;
    const float angleIncrement = 0.05f;
    const float distance = 0.7f;
    const float lookAhead = 0.3f;
    // spawn in the middle of the street, facing along it (X axis)
    const Point2D<float> characterTranslate = Point2D<float>(-0.0175f, 1.3216f);
    const float height = 0.0f;
    const float angle = 1.5708f * 2; // 180 degrees (rad)
    const float characterFacingAngle = 180.0f;

    UIController* uiCtrl = UIController::getInstance();
    GraphicsController* graphicsCtrl = GraphicsController::getInstance();
    MenuHUDScreen* menuHUDScreen = MenuHUDScreen::getInstance();
};
