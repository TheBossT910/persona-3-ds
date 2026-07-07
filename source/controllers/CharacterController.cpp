#include "CharacterController.h"
#include "core/globals.h"
#include "math.h"
#include <nds.h>
#include <stdio.h>

// check collision
TileType CharacterController::isTileAt(int tileX, int tileZ)
{
    // default
    if (tileX < 0 || tileX >= mapWidth || tileZ < 0 || tileZ >= mapHeight)
        return TileType::NO_COLLISION;

    // else use collision data
    return (TileType)collisionMap[(tileZ * mapWidth) + tileX];
}

bool CharacterController::isTileWalkable(float worldX, float worldZ)
{
    float distanceToEdge = characterSize.x * 0.5f;

    int tileMinX = (int)((worldX - distanceToEdge + worldOffsetX) / tileSize);
    int tileMaxX = (int)((worldX + distanceToEdge + worldOffsetX) / tileSize);
    int tileMinZ = (int)((worldZ - distanceToEdge + worldOffsetZ) / tileSize);
    int tileMaxZ = (int)((worldZ + distanceToEdge + worldOffsetZ) / tileSize);

    for (int z = tileMinZ; z <= tileMaxZ; z++)
    {
        for (int x = tileMinX; x <= tileMaxX; x++)
        {
            if (isTileAt(x, z) == TileType::COLLISION)
            {
                return false;
            }
        }
    }

    return true;
}

// get tile at character position
TileType CharacterController::isTileAt()
{
    int tileX = (int)((characterTranslate.x + worldOffsetX) / tileSize);
    int tileZ = (int)((characterTranslate.z + worldOffsetZ) / tileSize);
    return isTileAt(tileX, tileZ);
}

// get character position
CharacterPosition CharacterController::isCharacterAt()
{
    CharacterPosition charPos;

    charPos.x = characterTranslate.x;
    charPos.z = characterTranslate.z;
    charPos.y = height;
    charPos.angle = angle;
    charPos.facingAngle = characterFacingAngle;

    return charPos;
}

CameraPosition CharacterController::update(u32 keys)
{
    float forwardX;
    float forwardZ;
    float rightX;
    float rightZ;

    float deltaX = 0.0f;
    float deltaZ = 0.0f;

    float nextX;
    float nextZ;

    float angleRad;

    CameraPosition camPos;

    forwardX = -sin(angle) * speed;
    forwardZ = cos(angle) * speed;
    rightX = cos(angle) * speed;
    rightZ = sin(angle) * speed;

    if (!fixedCamera && (keys & KEY_L))
        angle -= angleIncrement;
    if (!fixedCamera && (keys & KEY_R))
        angle += angleIncrement;

    if (keys & KEY_UP)
    {
        deltaX += forwardX;
        deltaZ += forwardZ;
    }

    if (keys & KEY_DOWN)
    {
        deltaX -= forwardX;
        deltaZ -= forwardZ;
    }

    if (keys & KEY_RIGHT)
    {
        deltaX -= rightX;
        deltaZ -= rightZ;
    }

    if (keys & KEY_LEFT)
    {
        deltaX += rightX;
        deltaZ += rightZ;
    }

    if (deltaX != 0.0f && deltaZ != 0.0f)
    {
        // set walking animation
        if (Globals::enableCharacterAnim && (characterAnimationCtrl->getCurrentAnimIndex() != characterWalkAnim))
        {
            characterAnimationCtrl->set(characterWalkAnim, true);
        }

        // normalize diagonal movement to prevent faster speed
        const float invSqrt2 = 0.707106781187; // 1/sqrt(2)
        deltaX *= invSqrt2;
        deltaZ *= invSqrt2;
    }
    else
    {
        // set idle animation
        if (Globals::enableCharacterAnim && (characterAnimationCtrl->getCurrentAnimIndex() != characterIdleAnim))
        {
            characterAnimationCtrl->set(characterIdleAnim, true);
        }
    }
    characterAnimationCtrl->play();

    nextX = characterTranslate.x + deltaX;
    nextZ = characterTranslate.z + deltaZ;

    // try full movement first
    if (isTileWalkable(nextX, nextZ))
    {
        characterTranslate.x = nextX;
        characterTranslate.z = nextZ;
    }
    // if blocked, try X only (slide along Z wall)
    else if (isTileWalkable(nextX, characterTranslate.z))
    {
        characterTranslate.x = nextX;
    }
    // if blocked, try Z only (slide along X wall)
    else if (isTileWalkable(characterTranslate.x, nextZ))
    {
        characterTranslate.z = nextZ;
    }

    // only update the angle if button is being pressed
    if (deltaX != 0.0f || deltaZ != 0.0f)
    {
        // return angle in radians and convert to degrees
        angleRad = atan2(deltaX, deltaZ);
        characterFacingAngle = angleRad * (180.0f / 3.14159265f);
    }

    // set camera positions
    camPos.cameraX = characterTranslate.x + (sin(angle) * distance);
    camPos.cameraY = 0.6f + height;
    camPos.cameraZ = characterTranslate.z - (cos(angle) * distance);

    // look further down the same path the camera is facing
    camPos.targetX = characterTranslate.x - (sin(angle) * lookAhead);
    camPos.targetY = 0.1f + height;
    camPos.targetZ = characterTranslate.z + (cos(angle) * lookAhead);

    camPos.upX = 0.0f;
    camPos.upY = 1.0f + height;
    camPos.upZ = 0.0f;

    return camPos;
}
