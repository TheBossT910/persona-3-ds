#include "views/BaseView3D.h"
#include <nds/arm9/console.h>
// controllers
#include "controllers/CharacterController.h"
#include "controllers/DialogueController.h"
// environments
#include "environments/iwatodai_dorm_floor_1.h"

class IwatodaiDormView : public BaseView3D
{
  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
    void setupEnvironment() override;
    IwatodaiDormView();

  private:
    touchPosition touch;

    // sub screen
    int bgSharedSub1;
    int bgSharedSub2;
    int bgSharedSub3;
    PrintConsole console;

    // 3D
    iwatodai_dorm_floor_1_Environment iwatodaiDormFloor1Env;

    ViewPhase phase;
    bool prevPauseState;
    bool prevDialogueState;
    bool prevEnvironmentState;

    // controllers
    CharacterController* playerCtrl;
    // camera pos
    CameraPosition camPos;
    // world
    const float tileSize = 0.062500f;
    const float worldOffsetX = IWATODAI_DORM_FLOOR_1_WORLD_OFFSET_X;
    const float worldOffsetZ = IWATODAI_DORM_FLOOR_1_WORLD_OFFSET_Z;
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    // movement and viewpoint
    const float speed = 0.03f;
    const float angleIncrement = 0.07f;
    const float distance = 0.8f;
    const float lookAhead = 0.2f;
    // set character initial translation position
    const Point2D<float> characterTranslate = Point2D<float>(0.4f, 2.8f);
    const float height = 0.0;
    const float angle = -1.6;
    const float characterFacingAngle = 180.0f;
    DialogueController dialogueCtrl;

    void setMusic();
};
