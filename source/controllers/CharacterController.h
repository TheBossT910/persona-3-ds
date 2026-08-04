#pragma once
#include <stdint.h>

#include "controllers/AnimationController.h"
#include "core/enums.h"
#include "core/geometry.h"
#include "core/globals.h"
#include "core/structs.h"
#include "systems/CameraSystem.hpp"

// models
#include "models/kotone.h"
#include "models/makoto.h"

class CharacterController
{
  public:
    // 3D environment
    const int mapWidth;
    const int mapHeight;
    const uint16_t* collisionMap;

    // animations
    int characterWalkAnim =
        saveData.femcMode ? (int)MODEL_KOTONE_ROOT_MODEL_MOTION_0002 : (int)MODEL_MAKOTO_PLAYER_ROOT_MODEL_MOTION_0002;
    int characterIdleAnim =
        saveData.femcMode ? (int)MODEL_KOTONE_ROOT_MODEL_MOTION : (int)MODEL_MAKOTO_PLAYER_ROOT_MODEL_MOTION;

    // world
    const float tileSize;
    const float worldOffsetX;
    const float worldOffsetZ;
    const Point2D<float> characterSize;

    CameraMode cameraMode = CameraMode::Static;
    // movement
    const float speed;

    // translation (mutable)
    float height = 0.0;
    Point2D<float> characterTranslate = Point2D<float>(0.0, 0.0);
    float characterFacingAngle = 0.0f;

    CharacterController(int iMapWidth,
                        int iMapHeight,
                        const uint16_t* iCollisionMap,
                        float iTileSize,
                        float iWorldOffsetX,
                        float iWorldOffsetZ,
                        Point2D<float> iCharacterSize,
                        float iSpeed,
                        float iHeight,
                        Point2D<float> iCharacterTranslate,
                        float iCharacterFacingAngle)
        : mapWidth(iMapWidth), mapHeight(iMapHeight), collisionMap(iCollisionMap), tileSize(iTileSize),
          worldOffsetX(iWorldOffsetX), worldOffsetZ(iWorldOffsetZ), characterSize(iCharacterSize), speed(iSpeed)
    {
        height = iHeight;
        characterTranslate = iCharacterTranslate;
        characterFacingAngle = iCharacterFacingAngle;
    };

    void update(u32 keys);
    CharacterPosition isCharacterAt();
    TileType isTileAt();

  private:
    TileType isTileAt(int tileX, int TileY);
    bool isTileWalkable(float worldX, float worldZ);

    AnimationController* characterAnimationCtrl = AnimationController::getInstance();
};
