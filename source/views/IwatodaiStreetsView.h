#pragma once
#include "controllers/CharacterController.h"
#include "environments/iwatodai_streets.h"
#include "views/BaseView3D.h"
#include <nds/arm9/console.h>
// battle-related
#include "./battleActions/BattleParticipant.h"
#include "./battleActions/BattleStartCondition.h"
#include "./battleActions/enemies/EnemyDb.h"
#include "./battleActions/party/CharacterProfileDb.h"
#include "./controllers/BattleController.h" // TODO: move somewhere
#include <memory>

class IwatodaiStreetsView : public BaseView3D
{
  public:
    void init() override;
    ViewState update() override;
    void cleanup() override;
    void setupEnvironment() override;

  private:
    touchPosition touch;

    // sub screen
    int bgSharedSub1;
    int bgSharedSub2;
    int bgSharedSub3;
    PrintConsole console;

    ViewPhase phase;
    bool prevBattleState;
    bool prevPauseState;
    bool prevEnvironmentState;

    // Battle participants
    Enemy* mercilessMaya = new Enemy(EnemyDb::mercilessMaya);
    Enemy* cowardlyMaya = new Enemy(EnemyDb::cowardlyMaya);
    Player* player = new Player(CharacterProfileDb::player);
    PartyMember* yukari = new PartyMember(CharacterProfileDb::yukari);
    PartyMember* junpei = new PartyMember(CharacterProfileDb::junpei);

    std::vector<BattleParticipant*> battleParticipants = {mercilessMaya, cowardlyMaya, player, yukari, junpei};
    std::vector<Enemy*> enemies = {mercilessMaya, cowardlyMaya};
    std::vector<PartyMember*> partyMembers = {player, yukari, junpei};

    // hardcoded for now, we will have to build a battle creater for tartarus anyways
    BattleStartCondition battleStartCondition = BattleStartCondition::Even;

    // 3D
    iwatodai_streets_Environment iwatodaiStreetsEnv;

    // controllers
    BattleController battleController;
    CharacterController* playerCtrl;

    CameraPosition camPos;
    const float tileSize = 0.062500f;
    const float worldOffsetX = IWATODAI_STREETS_WORLD_OFFSET_X;
    const float worldOffsetZ = IWATODAI_STREETS_WORLD_OFFSET_Z;
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    const float speed = 0.03f;
    const float angleIncrement = 0.05f;
    const float distance = 1.0f;
    const float lookAhead = 0.2f;
    // spawn in the middle of the street, facing along it (X axis)
    const Point2D<float> characterTranslate = Point2D<float>(0.60f, 0.60f);
    const float height = 0.05f;
    const float angle = 1.5708f * 2; // 180 degrees (rad)
    const float characterFacingAngle = 0.0f;

    bool isBattleMenuActive = false;

    void setMusic();
};
