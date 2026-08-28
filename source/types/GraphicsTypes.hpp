#pragma once

#include <nds.h>

//a simple sprite structure
struct Sprite
{
    SpriteSize size;
    SpriteColorFormat format;
    int paletteAlpha;
    u16* gfx; // default = 0?
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

struct SpriteRenderState
{
    // sprite data
    Sprite& sprite;

    // render data
    int x = 0;
    int y = 0;
    int priority = 1;
    int affineIndex = 0;
    bool hflip = false;
    bool vflip = false;
    // uncommon
    bool hide = false;
    bool sizeDouble = false;
    bool mosaic = false;
};

struct SpriteTransform
{
    int angle;
    int sx;
    int sy;
};
