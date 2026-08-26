#include "MenuHUDScreen.hpp"
#include "core/globals.hpp"

MenuHUDScreen* MenuHUDScreen::instance = nullptr;

void MenuHUDScreen::create()
{
    if (instance == nullptr)
    {
        instance = new MenuHUDScreen();
    }
}

void MenuHUDScreen::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

MenuHUDScreen* MenuHUDScreen::getInstance()
{
    if (instance == nullptr)
    {
        instance = new MenuHUDScreen();
    }
    return instance;
}

// helper
void MenuHUDScreen::loadBackground()
{
    if (bgLoaded)
        return;

    std::string bgPath = "graphics/MenuHUD/backgrounds/";
    GraphicAsset bgHUD = graphics->loadGraphic(bgPath + "menuHUD/menuHUD");

    dmaCopy(bgHUD.tiles, bgGetGfxPtr(bgId), bgHUD.tilesLen);
    dmaCopy(bgHUD.map, bgGetMapPtr(bgId), bgHUD.mapLen);

    vramSetBankH(VRAM_H_LCD);
    dmaCopy(bgHUD.pal, &VRAM_H_EXT_PALETTE[bgId % 4][0], bgHUD.palLen);
    vramSetBankH(VRAM_H_SUB_BG_EXT_PALETTE);

    graphics->unloadGraphic(bgHUD);
    bgLoaded = true;
}

void MenuHUDScreen::renderSprites()
{
    // NOTE: we are currently assuming that the sprite extended palette will be set on VRAM bank I
    vramSetBankI(VRAM_I_LCD);
    dmaCopy(moonSprite.pal, &VRAM_I_EXT_SPR_PALETTE[0][0], moonSprite.palLen);                   // moon
    dmaCopy(dayOfWeekSprite.pal, &VRAM_I_EXT_SPR_PALETTE[1][0], dayOfWeekSprite.palLen);         // day of the week
    dmaCopy(numberSprites.at(0).pal, &VRAM_I_EXT_SPR_PALETTE[2][0], numberSprites.at(0).palLen); // numbers & slash
    dmaCopy(timeSprites.at(0).pal, &VRAM_I_EXT_SPR_PALETTE[3][0], timeSprites.at(0).palLen);     // time (0)
    dmaCopy(timeSprites.at(1).pal, &VRAM_I_EXT_SPR_PALETTE[4][0], timeSprites.at(1).palLen);     // time (1)
    dmaCopy(timeSprites.at(2).pal, &VRAM_I_EXT_SPR_PALETTE[5][0], timeSprites.at(2).palLen);     // time (2)
    dmaCopy(timeSprites.at(3).pal, &VRAM_I_EXT_SPR_PALETTE[6][0], timeSprites.at(3).palLen);     // time (3)
    dmaCopy(skillSprites.at(0).pal, &VRAM_I_EXT_SPR_PALETTE[7][0], skillSprites.at(0).palLen);   // skill level
    vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);

    // draw sprites
    for (int i = 0; i < 12; i++)
    {
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
               true,  // double the size of rotated sprites
               false, // don't hide the sprite
               false,
               false, // vflip, hflip
               false  // apply mosaic
        );
    }
}

void MenuHUDScreen::removeSprites()
{
    oamClear(oam, 0, 12);
}

int MenuHUDScreen::onTouch(touchPosition* touch)
{
    if (touch->px >= 193 && touch->px <= 250 && touch->py >= 166 && touch->py <= 184)
    {
        return 1;
    }

    return -1;
}

void MenuHUDScreen::load()
{
    if (menuHUD == nullptr)
    {
        menuHUD = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        menuHUD->AddComponent(graphics);
    }

    // load graphics
    bgLoaded = false;
    std::string spritePath = "graphics/MenuHUD/sprites/";

    // setup sprites
    // moon
    sprites.at(0) = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 0, 202, -15};
    // day of the week
    sprites.at(1) = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 1, 134, 143};
    // numbers
    sprites.at(2) = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 2, -11, 141}; // month (10s)
    sprites.at(3) = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 2, 15, 141};  // month (1s)
    sprites.at(4) = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 2, 54, 141};  // day (10s)
    sprites.at(5) = {0, SpriteSize_32x32, SpriteColorFormat_256Color, 0, 2, 80, 141};  // day (1s)
    // time
    sprites.at(6) = {0, SpriteSize_64x32, SpriteColorFormat_256Color, 0, 3, -27, -5}; // piece 0
    sprites.at(7) = {0, SpriteSize_64x32, SpriteColorFormat_256Color, 0, 4, 37, -5};  // piece 1
    sprites.at(8) = {0, SpriteSize_64x32, SpriteColorFormat_256Color, 0, 5, 101, -5}; // piece 2
    sprites.at(9) = {0, SpriteSize_64x32, SpriteColorFormat_256Color, 0, 6, 165, -5}; // piece 3
    // skill level
    sprites.at(10) = {0, SpriteSize_16x16, SpriteColorFormat_256Color, 0, 7, 90, 77};
    // slash
    sprites.at(11) = {0, SpriteSize_16x16, SpriteColorFormat_256Color, 0, 2, 52, 157};

    // allocating space for sprite graphics
    //moon
    sprites.at(0).gfx = oamAllocateGfx(oam, SpriteSize_32x32, SpriteColorFormat_256Color);
    // day of the week
    sprites.at(1).gfx = oamAllocateGfx(oam, SpriteSize_32x32, SpriteColorFormat_256Color);
    // numbers
    sprites.at(2).gfx = oamAllocateGfx(oam, SpriteSize_32x32, SpriteColorFormat_256Color);
    sprites.at(3).gfx = oamAllocateGfx(oam, SpriteSize_32x32, SpriteColorFormat_256Color);
    sprites.at(4).gfx = oamAllocateGfx(oam, SpriteSize_32x32, SpriteColorFormat_256Color);
    sprites.at(5).gfx = oamAllocateGfx(oam, SpriteSize_32x32, SpriteColorFormat_256Color);
    // time
    sprites.at(6).gfx = oamAllocateGfx(oam, SpriteSize_64x32, SpriteColorFormat_256Color);
    sprites.at(7).gfx = oamAllocateGfx(oam, SpriteSize_64x32, SpriteColorFormat_256Color);
    sprites.at(8).gfx = oamAllocateGfx(oam, SpriteSize_64x32, SpriteColorFormat_256Color);
    sprites.at(9).gfx = oamAllocateGfx(oam, SpriteSize_64x32, SpriteColorFormat_256Color);
    // skill level
    sprites.at(10).gfx = oamAllocateGfx(oam, SpriteSize_16x16, SpriteColorFormat_256Color);
    // slash
    sprites.at(11).gfx = oamAllocateGfx(oam, SpriteSize_16x16, SpriteColorFormat_256Color);

    // get sprites
    // moon
    moonSprite = graphics->loadSpriteGraphic(spritePath, SpriteType::MOON, MoonSprite::MOON_22);
    // day of the week
    dayOfWeekSprite = graphics->loadSpriteGraphic(spritePath, SpriteType::DAY_OF_WEEK, DayOfWeekSprite::TUESDAY);
    // numbers
    numberSprites.at(0) = graphics->loadSpriteGraphic(spritePath, SpriteType::DIGIT, DigitSprite::DIGIT_0);
    numberSprites.at(1) = graphics->loadSpriteGraphic(spritePath, SpriteType::DIGIT, DigitSprite::DIGIT_4);
    numberSprites.at(2) = graphics->loadSpriteGraphic(spritePath, SpriteType::DIGIT, DigitSprite::DIGIT_0);
    numberSprites.at(3) = graphics->loadSpriteGraphic(spritePath, SpriteType::DIGIT, DigitSprite::DIGIT_7);
    // time
    timeSprites.at(0) = graphics->loadSpriteGraphic(spritePath, SpriteType::TIME, TimeSprite::EARLY_MORNING_0_0);
    timeSprites.at(1) = graphics->loadSpriteGraphic(spritePath, SpriteType::TIME, TimeSprite::EARLY_MORNING_1_0);
    timeSprites.at(2) = graphics->loadSpriteGraphic(spritePath, SpriteType::TIME, TimeSprite::EARLY_MORNING_2_0);
    timeSprites.at(3) = graphics->loadSpriteGraphic(spritePath, SpriteType::TIME, TimeSprite::EARLY_MORNING_3_0);
    // skill level
    skillSprites.at(0) = graphics->loadSpriteGraphic(spritePath, SpriteType::SKILL_SPRITE, SkillSprite::SKILLS_LEVEL);
    // slash
    slashSprite = graphics->loadSpriteGraphic(spritePath, SpriteType::DIGIT, DigitSprite::SLASH);

    // TODO: initialize any extra sprite registers for max-case arrays?
    // ...

    // copy sprites into memory
    // moon
    dmaCopy(moonSprite.tiles, sprites.at(0).gfx, moonSprite.tilesLen);
    // day of the week
    dmaCopy(dayOfWeekSprite.tiles, sprites.at(1).gfx, dayOfWeekSprite.tilesLen);
    // numbers
    dmaCopy(numberSprites.at(0).tiles, sprites.at(2).gfx, numberSprites.at(0).tilesLen);
    dmaCopy(numberSprites.at(1).tiles, sprites.at(3).gfx, numberSprites.at(1).tilesLen);
    dmaCopy(numberSprites.at(2).tiles, sprites.at(4).gfx, numberSprites.at(2).tilesLen);
    dmaCopy(numberSprites.at(3).tiles, sprites.at(5).gfx, numberSprites.at(3).tilesLen);
    // time
    dmaCopy(timeSprites.at(0).tiles, sprites.at(6).gfx, timeSprites.at(0).tilesLen);
    dmaCopy(timeSprites.at(1).tiles, sprites.at(7).gfx, timeSprites.at(1).tilesLen);
    dmaCopy(timeSprites.at(2).tiles, sprites.at(8).gfx, timeSprites.at(2).tilesLen);
    dmaCopy(timeSprites.at(3).tiles, sprites.at(9).gfx, timeSprites.at(3).tilesLen);
    // skill level
    dmaCopy(skillSprites.at(0).tiles, sprites.at(10).gfx, skillSprites.at(0).tilesLen);
    // slash
    dmaCopy(slashSprite.tiles, sprites.at(11).gfx, slashSprite.tilesLen);

    loadBackground();
};

void MenuHUDScreen::unload()
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

    if (menuHUD != nullptr)
    {
        engine.DestroyEntity(menuHUD);

        menuHUD = nullptr;
        graphics = nullptr;
    }
}
