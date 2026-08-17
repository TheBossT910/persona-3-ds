#pragma once

#include <core/geometry.h>
#include <nds.h>

/**
 * @brief A struct that holds initial config values for the MovementComponent
 */
struct MovementConfig
{
    // 3D environment
    int mapWidth;
    int mapHeight;
    uint16_t* collisionMap;

    // world
    float tileSize;
    float worldOffsetX;
    float worldOffsetZ;
    Point2D<float> characterSize;

    // translation
    float speed;
    float height;
    Point2D<float> characterTranslate;
    float characterFacingAngle;

    MovementConfig() = default;

    MovementConfig(int iMapWidth,
                   int iMapHeight,
                   uint16_t* iCollisionMap,
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
};
