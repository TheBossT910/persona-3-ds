#include "DialogueScreen.hpp"
#include "core/globals.hpp"

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
    // NOTE: we are currently assuming that the sprite extended palette will be set on VRAM bank I
    vramSetBankI(VRAM_I_LCD);
    dmaCopy(calendarSprite.at(0).pal, &VRAM_I_EXT_SPR_PALETTE[0][0], calendarSprite.at(0).palLen);
    dmaCopy(textBox.at(0).pal, &VRAM_I_EXT_SPR_PALETTE[1][0], textBox.at(0).palLen);
    dmaCopy(nameTag.at(0).pal, &VRAM_I_EXT_SPR_PALETTE[2][0], nameTag.at(0).palLen);
    vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);

    int i = 0;
    oamSet(oam, // sub display (OamState)
           i,   // oam entry to set (id)
           sprites.at(i).x,
           sprites.at(i).y,            // position
           1,                          // priority
           sprites.at(i).paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
           sprites.at(i).size,
           sprites.at(i).format,
           sprites.at(i).gfx,
           sprites.at(i).rotationIndex,
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           true,
           false, // vflip, hflip
           false  // apply mosaic
    );

    i = 1;
    oamSet(oam, // sub display (OamState)
           i,   // oam entry to set (id)
           sprites.at(i).x,
           sprites.at(i).y,            // position
           1,                          // priority
           sprites.at(i).paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
           sprites.at(i).size,
           sprites.at(i).format,
           sprites.at(i).gfx,
           sprites.at(i).rotationIndex,
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           false,
           false, // vflip, hflip
           false  // apply mosaic
    );

    i = 2;
    oamSet(oam, // sub display (OamState)
           i,   // oam entry to set (id)
           sprites.at(i).x,
           sprites.at(i).y,            // position
           1,                          // priority
           sprites.at(i).paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
           sprites.at(i).size,
           sprites.at(i).format,
           sprites.at(i).gfx,
           sprites.at(i).rotationIndex,
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           false,
           false, // vflip, hflip
           false  // apply mosaic
    );

    i = 3;
    oamSet(oam, // sub display (OamState)
           i,   // oam entry to set (id)
           sprites.at(i).x,
           sprites.at(i).y,            // position
           1,                          // priority
           sprites.at(i).paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
           sprites.at(i).size,
           sprites.at(i).format,
           sprites.at(i).gfx,
           sprites.at(i).rotationIndex,
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           false,
           false, // vflip, hflip
           false  // apply mosaic
    );

    i = 4;
    oamSet(oam, // sub display (OamState)
           i,   // oam entry to set (id)
           sprites.at(i).x,
           sprites.at(i).y,            // position
           1,                          // priority
           sprites.at(i).paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
           sprites.at(i).size,
           sprites.at(i).format,
           sprites.at(i).gfx,
           sprites.at(i).rotationIndex,
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           false,
           false, // vflip, hflip
           false  // apply mosaic
    );

    i = 5;
    oamSet(oam, // sub display (OamState)
           i,   // oam entry to set (id)
           sprites.at(i).x,
           sprites.at(i).y,            // position
           1,                          // priority
           sprites.at(i).paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
           sprites.at(i).size,
           sprites.at(i).format,
           sprites.at(i).gfx,
           sprites.at(i).rotationIndex,
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           true,
           false, // vflip, hflip
           false  // apply mosaic
    );

    i = 6;
    oamSet(oam, // sub display (OamState)
           i,   // oam entry to set (id)
           sprites.at(i).x,
           sprites.at(i).y,            // position
           1,                          // priority
           sprites.at(i).paletteAlpha, // palette for 16 color sprite or alpha for bmp sprite
           sprites.at(i).size,
           sprites.at(i).format,
           sprites.at(i).gfx,
           sprites.at(i).rotationIndex,
           false, // double the size of rotated sprites
           false, // don't hide the sprite
           false,
           false, // vflip, hflip
           false  // apply mosaic
    );

    // rotate sprite
    // oamRotateScale(
    //     oam,
    //     sprites.at(i).rotationIndex,
    //     degreesToAngle(0),
    //     intToFixed(1, 8),
    //     intToFixed(1, 8)
    // );

    // render multiple of the same sprits
    // in this case, the skills level
    // for (int i = 0; i < 3; i++)
    // {
    //     oamSet(
    //         oam,                    // sub display (OamState)
    //         7 + i,                      // oam entry to set (id)
    //         sprites.at(7).x + (13 * i), sprites.at(7).y, // position
    //         1,                          // priority
    //         sprites.at(7).paletteAlpha,    // palette for 16 color sprite or alpha for bmp sprite
    //         sprites.at(7).size,
    //         sprites.at(7).format,
    //         sprites.at(7).gfx,
    //         sprites.at(7).rotationIndex,
    //         true,         // double the size of rotated sprites
    //         false,        // don't hide the sprite
    //         false, false, // vflip, hflip
    //         false         // apply mosaic
    //     );
    // }
}

void DialogueScreen::removeSprites()
{
    oamClear(oam, 0, 6);
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
    // calendar
    sprites.at(0) = {0, SpriteSize_64x64, SpriteColorFormat_256Color, -1, 0, 192, 0};
    sprites.at(1) = {0, SpriteSize_64x64, SpriteColorFormat_256Color, 0, 0, 128, 0};
    // text box
    sprites.at(2) = {0, SpriteSize_64x64, SpriteColorFormat_256Color, 0, 1, 0, 128};
    sprites.at(3) = {0, SpriteSize_64x64, SpriteColorFormat_256Color, 0, 1, 64, 128};
    sprites.at(4) = {0, SpriteSize_64x64, SpriteColorFormat_256Color, 0, 1, 128, 128};
    sprites.at(5) = {0, SpriteSize_64x64, SpriteColorFormat_256Color, -1, 1, 192, 128};
    // name tag
    sprites.at(6) = {0, SpriteSize_64x32, SpriteColorFormat_256Color, 0, 2, 20, 112};

    // allocating space for sprite graphics
    // calendar
    sprites.at(0).gfx = oamAllocateGfx(oam, sprites.at(0).size, sprites.at(0).format);
    sprites.at(1).gfx = oamAllocateGfx(oam, sprites.at(1).size, sprites.at(1).format);
    // text box
    sprites.at(2).gfx = oamAllocateGfx(oam, sprites.at(2).size, sprites.at(2).format);
    sprites.at(3).gfx = oamAllocateGfx(oam, sprites.at(3).size, sprites.at(3).format);
    sprites.at(4).gfx = oamAllocateGfx(oam, sprites.at(4).size, sprites.at(4).format);
    sprites.at(5).gfx = oamAllocateGfx(oam, sprites.at(5).size, sprites.at(5).format);
    // name tag
    sprites.at(6).gfx = oamAllocateGfx(oam, sprites.at(6).size, sprites.at(6).format);

    // get sprites
    // calendar
    calendarSprite.at(0) = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::CALENDAR);
    calendarSprite.at(1) = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::CALENDAR);
    // text box
    textBox.at(0) = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::TEXT_CORNER);
    textBox.at(1) = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::TEXT_MIDDLE);
    textBox.at(2) = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::TEXT_MIDDLE);
    textBox.at(3) = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::TEXT_CORNER);
    // name tag
    nameTag.at(0) = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::NAME_TAG);

    // copy sprites into memory
    // calendar
    dmaCopy(calendarSprite.at(0).tiles, sprites.at(0).gfx, calendarSprite.at(0).tilesLen);
    dmaCopy(calendarSprite.at(1).tiles, sprites.at(1).gfx, calendarSprite.at(1).tilesLen);
    // text box
    dmaCopy(textBox.at(0).tiles, sprites.at(2).gfx, textBox.at(0).tilesLen);
    dmaCopy(textBox.at(1).tiles, sprites.at(3).gfx, textBox.at(1).tilesLen);
    dmaCopy(textBox.at(2).tiles, sprites.at(4).gfx, textBox.at(2).tilesLen);
    dmaCopy(textBox.at(3).tiles, sprites.at(5).gfx, textBox.at(3).tilesLen);
    // name tag
    dmaCopy(nameTag.at(0).tiles, sprites.at(6).gfx, nameTag.at(0).tilesLen);
};

void DialogueScreen::unload()
{
    removeSprites();
    for (Sprite& sprite : sprites)
    {
        if (sprite.gfx != nullptr)
        {
            oamFreeGfx(oam, sprite.gfx);
            sprite.gfx = nullptr;
        }
    }

    if (dialogue != nullptr)
    {
        engine.DestroyEntity(dialogue);

        dialogue = nullptr;
        graphics = nullptr;
    }
}
