#pragma once
#include "views/EnvironmentView.hpp"

#include "data/environmentDb.hpp"
#include "maps/iwatodai_streets.hpp"

#include "battleActions/BattleParticipant.hpp"
#include "battleActions/BattleStartCondition.hpp"
#include "battleActions/enemies/Enemy.hpp"
#include "battleActions/enemies/EnemyProfileDb.hpp"
#include "battleActions/party/CharacterProfileDb.hpp"
#include "battleActions/party/PartyMember.hpp"
#include "battleActions/party/Player.hpp"

#include <etl/vector.h>

class IwatodaiStreetsView : public EnvironmentView
{
  public:
    IwatodaiStreetsView();

    ~IwatodaiStreetsView();

  protected:
    const EnvironmentDbEntry* getEnvironmentDbEntry() override
    {
        return g_environmentDb[1];
    }

    void setupMovement() override;

    void setupMusic() override;

    ViewState onTileCheck(TileType tile, u32 pressed) override;

    void setupCamera() override;

    void setupText() override;

    void setupUI() override;

    // battle hook
    void startBattle() override;

  private:
    // movement and camera
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    const float speed = 0.03f;

    // character position
    const Point2D<float> characterTranslate = Point2D<float>(0.60f, 0.60f);
    const float height = 0.05f;
    const float characterFacingAngle = 0.0f;

    // battle
    etl::vector<CharacterProfile, 4> characterProfiles;
    etl::vector<EnemyProfile, 8> enemyProfiles;

    BattleStartCondition battleStartCondition = BattleStartCondition::Even;
};
