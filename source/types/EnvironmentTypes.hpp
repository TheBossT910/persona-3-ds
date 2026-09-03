#pragma once

#include <nds.h>

struct EnvironmentTexture
{
    const char* name;
    int width;
    int height;

    const unsigned int* bitmap;
};

struct BillboardData
{
    const char* name;
    v16 x, y, z;
    v16 halfWidth;
    v16 halfHeight;

    int texSlot;

    short u0, v0;
    short u1, v1;
};

struct EnvironmentDbEntry
{
    // Name/debugging
    const char* name;

    // Binary display list file
    const char* binaryFile;

    // World bounds
    float worldOffsetX;
    float worldOffsetZ;
    float worldWidth;
    float worldDepth;

    // Texture information
    int textureCount;
    const EnvironmentTexture* textures;

    // Billboards
    int billboardCount;
    const BillboardData* billboards;
};
