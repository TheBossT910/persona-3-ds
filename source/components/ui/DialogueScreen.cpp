#include "DialogueScreen.hpp"

DialogueScreen* DialogueScreen::instance = nullptr;

void DialogueScreen::create()
{
    if (instance == nullptr)
    {
        instance = new DialogueScreen();
    }
}

void DialogueScreen::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }
    instance = nullptr;
}

DialogueScreen* DialogueScreen::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }
    return instance;
}

// TODO: clean up and properly implement class

// helper
void DialogueScreen::renderSprites()
{
    // we have 16 colours x 16 palettes
    dmaCopy(blueBlock[0].pal, SPRITE_PALETTE_SUB, 16 * sizeof(u16));
    dmaCopy(whiteBlock[0].pal, SPRITE_PALETTE_SUB + (1 * 16), 16 * sizeof(u16));

    dmaCopy(corner[0].pal, SPRITE_PALETTE_SUB + (2 * 16), 16 * sizeof(u16));
    dmaCopy(edge[0].pal, SPRITE_PALETTE_SUB + (3 * 16), 16 * sizeof(u16));
    int i = 0;

    // render multiple blue block sprites
    for (int count = 0; count < 8; count++)
    {
        oamSet(oam, // sub display (OamState)
               i++, // oam entry to set (id)
               sprites[0].x + (32 * count),
               sprites[0].y,            // position
               1,                       // priority
               sprites[0].paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
               sprites[0].size,
               sprites[0].format,
               sprites[0].gfx,
               sprites[0].rotationIndex,
               false, // double the size of rotated sprites
               false, // don't hide the sprite
               false,
               false, // vflip, hflip
               false  // apply mosaic
        );
    }

    // corner
    // top right
    oamSet(oam, // sub display (OamState)
           i++, // oam entry to set (id)
           224,
           149,                     // position
           1,                       // priority
           sprites[2].paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
           sprites[2].size,
           sprites[2].format,
           sprites[2].gfx,
           sprites[2].rotationIndex,
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           false,
           false,
           false // apply mosaic
    );

    // top left
    oamSet(oam, // sub display (OamState)
           i++, // oam entry to set (id)
           0,
           149,                     // position
           1,                       // priority
           sprites[2].paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
           sprites[2].size,
           sprites[2].format,
           sprites[2].gfx,
           sprites[2].rotationIndex,
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           true,
           false,
           false // apply mosaic
    );

    // bottom left
    oamSet(oam, // sub display (OamState)
           i++, // oam entry to set (id)
           0,
           176,                     // position
           1,                       // priority
           sprites[2].paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
           sprites[2].size,
           sprites[2].format,
           sprites[2].gfx,
           sprites[2].rotationIndex,
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           true,
           true,
           false // apply mosaic
    );

    // bottom right
    oamSet(oam, // sub display (OamState)
           i++, // oam entry to set (id)
           224,
           176,                     // position
           1,                       // priority
           sprites[2].paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
           sprites[2].size,
           sprites[2].format,
           sprites[2].gfx,
           sprites[2].rotationIndex,
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           false,
           true,
           false // apply mosaic
    );

    // render multiple edge sprites on the top
    for (int count = 0; count < 6; count++)
    {
        oamSet(oam, // sub display (OamState)
               i++, // oam entry to set (id)
               32 + (32 * count),
               149,                     // position
               1,                       // priority
               sprites[3].paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
               sprites[3].size,
               sprites[3].format,
               sprites[3].gfx,
               sprites[3].rotationIndex,
               false, // double the size of rotated sprites
               false, // don't hide the sprite
               false,
               false, // vflip, hflip
               false  // apply mosaic
        );
    }

    // render multiple edge sprites on the bottom
    for (int count = 0; count < 6; count++)
    {
        oamSet(oam, // sub display (OamState)
               i++, // oam entry to set (id)
               32 + (32 * count),
               176,                     // position
               1,                       // priority
               sprites[3].paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
               sprites[3].size,
               sprites[3].format,
               sprites[3].gfx,
               sprites[3].rotationIndex,
               false, // double the size of rotated sprites
               false, // don't hide the sprite
               false,
               true, // vflip, hflip
               false // apply mosaic
        );
    }

    // left edge
    oamSet(oam, // sub display (OamState)
           i++, // oam entry to set (id)
           -8,
           165,                     // position
           1,                       // priority
           sprites[3].paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
           sprites[3].size,
           sprites[3].format,
           sprites[3].gfx,
           1,
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           false,
           false,
           false // apply mosaic
    );
    oamRotateScale(oam, 1, degreesToAngle(90), intToFixed(1, 8), intToFixed(1, 8));

    // right edge
    oamSet(oam, // sub display (OamState)
           i++, // oam entry to set (id)
           231,
           165,                     // position
           1,                       // priority
           sprites[3].paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
           sprites[3].size,
           sprites[3].format,
           sprites[3].gfx,
           2,
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           false,
           false,
           false // apply mosaic
    );
    oamRotateScale(oam, 2, degreesToAngle(270), intToFixed(1, 8), intToFixed(1, 8));

    // render multiple white block sprites
    for (int count = 0; count < 8; count++)
    {
        oamSet(oam, // sub display (OamState)
               i++, // oam entry to set (id)
               sprites[1].x + (32 * count),
               sprites[1].y,            // position
               1,                       // priority
               sprites[1].paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
               sprites[1].size,
               sprites[1].format,
               sprites[1].gfx,
               sprites[1].rotationIndex,
               false, // double the size of rotated sprites
               false, // don't hide the sprite
               false,
               false, // vflip, hflip
               false  // apply mosaic
        );
    }
}

void DialogueScreen::removeSprites()
{
    oamClear(oam, 0, 34);
}

void DialogueScreen::load()
{
    if (dialogue == nullptr)
    {
        dialogue = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        dialogue->AddComponent(graphics);
    }

    // load graphics
    std::string spritePath = "graphics/Dialogue/sprites/";

    // setup sprites
    // blue block
    sprites[0] = {0, SpriteSize_32x16, SpriteColorFormat_16Color, 0, 0, 0, 133};
    // white block
    sprites[1] = {0, SpriteSize_32x16, SpriteColorFormat_16Color, 0, 1, 0, 165};
    // corner
    sprites[2] = {0, SpriteSize_32x16, SpriteColorFormat_16Color, -1, 2, 0, 0};
    // edge
    sprites[3] = {0, SpriteSize_32x16, SpriteColorFormat_16Color, -1, 3, 0, 0};
    // corner green
    sprites[4] = {0, SpriteSize_32x16, SpriteColorFormat_16Color, -1, 4, 0, 0};
    // edge green
    sprites[5] = {0, SpriteSize_32x16, SpriteColorFormat_16Color, -1, 5, 0, 0};

    // allocating space for sprite graphics
    // blue block
    sprites[0].gfx = oamAllocateGfx(oam, sprites[0].size, sprites[0].format);
    // white block
    sprites[1].gfx = oamAllocateGfx(oam, sprites[1].size, sprites[1].format);
    // corner
    sprites[2].gfx = oamAllocateGfx(oam, sprites[2].size, sprites[2].format);
    // edge
    sprites[3].gfx = oamAllocateGfx(oam, sprites[3].size, sprites[3].format);
    // corner green
    sprites[4].gfx = oamAllocateGfx(oam, sprites[4].size, sprites[4].format);
    // edge green
    sprites[5].gfx = oamAllocateGfx(oam, sprites[5].size, sprites[5].format);

    // get sprites
    // blue block
    blueBlock[0] = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::BLUE_BLOCK);
    // white block
    whiteBlock[0] = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::WHITE_BLOCK);
    // corner
    corner[0] = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::CORNER);
    // edge
    edge[0] = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::EDGE);
    // corner green
    corner[1] = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::CORNER_GREEN);
    // edge green
    edge[1] = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::EDGE_GREEN);

    // copy sprites into memory
    // blue block
    dmaCopy(blueBlock[0].tiles, sprites[0].gfx, blueBlock[0].tilesLen);
    // white block
    dmaCopy(whiteBlock[0].tiles, sprites[1].gfx, whiteBlock[0].tilesLen);
    // corner
    dmaCopy(corner[0].tiles, sprites[2].gfx, corner[0].tilesLen);
    // edge
    dmaCopy(edge[0].tiles, sprites[3].gfx, edge[0].tilesLen);
    // corner green
    dmaCopy(corner[1].tiles, sprites[4].gfx, corner[1].tilesLen);
    // edge green
    dmaCopy(edge[1].tiles, sprites[5].gfx, edge[1].tilesLen);
};

void DialogueScreen::unload()
{
    if (graphics != nullptr)
    {
        graphics->unloadAll();
    }

    if (dialogue != nullptr)
    {
        dialogue->RemoveComponent<GraphicsComponent>();
        engine.DestroyEntity(dialogue);

        dialogue = nullptr;
        graphics = nullptr;
    }
}
