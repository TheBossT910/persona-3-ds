#include "views/BaseView3D.h"
#include <nds/arm9/console.h>
// controllers
#include "controllers/CharacterController.h"
// environments
#include "environments/paulownia_mall.h"

class PaulowniaMallView : public BaseView3D
{
  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
    void setupEnvironment() override;
    PaulowniaMallView();

  private:
    touchPosition touch;

    // sub screen
    int bgSharedSub1;
    int bgSharedSub2;
    int bgSharedSub3;
    PrintConsole console;

    // 3D
    paulownia_mall_Environment paulowniaMallEnv;

    ViewPhase phase;
    bool prevPauseState;
    bool prevEnvironmentState;

    CharacterController* playerCtrl;

    // camera pos
    CameraPosition camPos;
    // world
    const float tileSize = 0.062500f;
    const float worldOffsetX = PAULOWNIA_MALL_WORLD_OFFSET_X;
    const float worldOffsetZ = PAULOWNIA_MALL_WORLD_OFFSET_Z;
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    // movement and viewpoint
    const float speed = 0.03f;
    const float angleIncrement = 0.05f;
    const float distance = 1.0f;
    const float lookAhead = 0.2f;
    // set character initial translation position
    const Point2D<float> characterTranslate = Point2D<float>(0.0122f, 2.3355f);
    const float height = 0.2f;
    const float angle = 1.5708f * 2; // 180 degrees (rad)
    const float characterFacingAngle = 180;

    void setMusic();
};
