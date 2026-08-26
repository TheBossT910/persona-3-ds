#pragma once

#include <nds.h>

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
