#pragma once

#include <nds.h>

enum class SpriteType
{
    NONE = 0,
    MOON,
    DAY_OF_WEEK,
    DIGIT,
    TIME,
    SKILL_SPRITE,
    DIALOGUE,
    CUSTOM,
};

struct SpriteDBEntry
{
    SpriteType type;
    int id;
    const char* filename;
};

//a simple sprite structure
struct Sprite
{
    u16* gfx;
    SpriteSize size;
    SpriteColorFormat format;
    int rotationIndex;
    int paletteAlpha;
    int x;
    int y;
};

struct GraphicAsset
{
    int id;
    void* tiles = nullptr;
    u32 tilesLen;
    void* pal = nullptr;
    u32 palLen;
    void* map = nullptr;
    u32 mapLen;
};
