#include "MenuHUDScreen.hpp"

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
    dmaCopy(moonGraphic.pal, &VRAM_I_EXT_SPR_PALETTE[0][0], moonGraphic.palLen);
    dmaCopy(dayOfWeekGraphic.pal, &VRAM_I_EXT_SPR_PALETTE[1][0], dayOfWeekGraphic.palLen);
    dmaCopy(digitGraphics[0].pal, &VRAM_I_EXT_SPR_PALETTE[2][0], digitGraphics[0].palLen);
    dmaCopy(timeGraphics[0].pal, &VRAM_I_EXT_SPR_PALETTE[3][0], timeGraphics[0].palLen);
    dmaCopy(timeGraphics[1].pal, &VRAM_I_EXT_SPR_PALETTE[4][0], timeGraphics[1].palLen);
    dmaCopy(timeGraphics[2].pal, &VRAM_I_EXT_SPR_PALETTE[5][0], timeGraphics[2].palLen);
    dmaCopy(timeGraphics[3].pal, &VRAM_I_EXT_SPR_PALETTE[6][0], timeGraphics[3].palLen);
    dmaCopy(skillGraphic.pal, &VRAM_I_EXT_SPR_PALETTE[7][0], skillGraphic.palLen);
    vramSetBankI(VRAM_I_SUB_SPRITE_EXT_PALETTE);

    // perform transformations
    /// index -1 is reserved for vflip/hflip, 0 is reserved for no transform
    int i = 1;
    for (SpriteTransform& st : spriteTransforms)
    {
        oamRotateScale(oam, i++, st.angle, st.sx, st.sy);
    }

    // draw sprites
    int j = 0;
    for (SpriteRenderState& srs : spriteRenderStates)
    {
        oamSet(oam,
               j++,
               srs.x,
               srs.y,
               srs.priority,
               srs.sprite.paletteAlpha,
               srs.sprite.size,
               srs.sprite.format,
               srs.sprite.gfx,
               srs.affineIndex,
               srs.sizeDouble,
               srs.hide,
               srs.hflip,
               srs.vflip,
               srs.mosaic);
    }
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
    // create relevant entities, components
    if (menuHUD == nullptr)
    {
        menuHUD = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        menuHUD->AddComponent(graphics);
    }

    // setup sprites
    moonSprite = {SpriteSize_32x32, SpriteColorFormat_256Color, 0};
    dayOfWeekSprite = {SpriteSize_32x32, SpriteColorFormat_256Color, 1};
    digitSprites[0] = {SpriteSize_32x32, SpriteColorFormat_256Color, 2};
    digitSprites[1] = {SpriteSize_32x32, SpriteColorFormat_256Color, 2};
    digitSprites[2] = {SpriteSize_32x32, SpriteColorFormat_256Color, 2};
    timeSprites[0] = {SpriteSize_64x32, SpriteColorFormat_256Color, 3};
    timeSprites[1] = {SpriteSize_64x32, SpriteColorFormat_256Color, 4};
    timeSprites[2] = {SpriteSize_64x32, SpriteColorFormat_256Color, 5};
    timeSprites[3] = {SpriteSize_64x32, SpriteColorFormat_256Color, 6};
    skillSprite = {SpriteSize_16x16, SpriteColorFormat_256Color, 7};
    slashSprite = {SpriteSize_16x16, SpriteColorFormat_256Color, 2};

    // allocating space for sprite
    moonSprite.gfx = oamAllocateGfx(oam, moonSprite.size, moonSprite.format);
    dayOfWeekSprite.gfx = oamAllocateGfx(oam, dayOfWeekSprite.size, dayOfWeekSprite.format);
    digitSprites[0].gfx = oamAllocateGfx(oam, digitSprites[0].size, digitSprites[0].format);
    digitSprites[1].gfx = oamAllocateGfx(oam, digitSprites[1].size, digitSprites[1].format);
    digitSprites[2].gfx = oamAllocateGfx(oam, digitSprites[2].size, digitSprites[2].format);
    timeSprites[0].gfx = oamAllocateGfx(oam, timeSprites[0].size, timeSprites[0].format);
    timeSprites[1].gfx = oamAllocateGfx(oam, timeSprites[1].size, timeSprites[1].format);
    timeSprites[2].gfx = oamAllocateGfx(oam, timeSprites[2].size, timeSprites[2].format);
    timeSprites[3].gfx = oamAllocateGfx(oam, timeSprites[3].size, timeSprites[3].format);
    skillSprite.gfx = oamAllocateGfx(oam, skillSprite.size, skillSprite.format);
    slashSprite.gfx = oamAllocateGfx(oam, slashSprite.size, slashSprite.format);

    // load sprites
    std::string spritePath = "graphics/MenuHUD/sprites/";
    moonGraphic = graphics->loadSpriteGraphic(spritePath, SpriteType::MOON, MoonSprite::MOON_22);
    dayOfWeekGraphic = graphics->loadSpriteGraphic(spritePath, SpriteType::DAY_OF_WEEK, DayOfWeekSprite::TUESDAY);
    digitGraphics[0] = graphics->loadSpriteGraphic(spritePath, SpriteType::DIGIT, DigitSprite::DIGIT_0);
    digitGraphics[1] = graphics->loadSpriteGraphic(spritePath, SpriteType::DIGIT, DigitSprite::DIGIT_4);
    digitGraphics[2] = graphics->loadSpriteGraphic(spritePath, SpriteType::DIGIT, DigitSprite::DIGIT_7);
    timeGraphics[0] = graphics->loadSpriteGraphic(spritePath, SpriteType::TIME, TimeSprite::EARLY_MORNING_0_0);
    timeGraphics[1] = graphics->loadSpriteGraphic(spritePath, SpriteType::TIME, TimeSprite::EARLY_MORNING_1_0);
    timeGraphics[2] = graphics->loadSpriteGraphic(spritePath, SpriteType::TIME, TimeSprite::EARLY_MORNING_2_0);
    timeGraphics[3] = graphics->loadSpriteGraphic(spritePath, SpriteType::TIME, TimeSprite::EARLY_MORNING_3_0);
    skillGraphic = graphics->loadSpriteGraphic(spritePath, SpriteType::SKILL_SPRITE, SkillSprite::SKILLS_LEVEL);
    slashGraphic = graphics->loadSpriteGraphic(spritePath, SpriteType::DIGIT, DigitSprite::SLASH);

    // copy sprites into memory
    dmaCopy(moonGraphic.tiles, moonSprite.gfx, moonGraphic.tilesLen);
    dmaCopy(dayOfWeekGraphic.tiles, dayOfWeekSprite.gfx, dayOfWeekGraphic.tilesLen);
    dmaCopy(digitGraphics[0].tiles, digitSprites[0].gfx, digitGraphics[0].tilesLen);
    dmaCopy(digitGraphics[1].tiles, digitSprites[1].gfx, digitGraphics[1].tilesLen);
    dmaCopy(digitGraphics[2].tiles, digitSprites[2].gfx, digitGraphics[2].tilesLen);
    dmaCopy(timeGraphics[0].tiles, timeSprites[0].gfx, timeGraphics[0].tilesLen);
    dmaCopy(timeGraphics[1].tiles, timeSprites[1].gfx, timeGraphics[1].tilesLen);
    dmaCopy(timeGraphics[2].tiles, timeSprites[2].gfx, timeGraphics[2].tilesLen);
    dmaCopy(timeGraphics[3].tiles, timeSprites[3].gfx, timeGraphics[3].tilesLen);
    dmaCopy(skillGraphic.tiles, skillSprite.gfx, skillGraphic.tilesLen);
    dmaCopy(slashGraphic.tiles, slashSprite.gfx, slashGraphic.tilesLen);

    // load background
    bgLoaded = false;
    loadBackground();
};

void MenuHUDScreen::unload()
{
    if (graphics != nullptr)
    {
        graphics->unloadAll();
    }

    if (menuHUD != nullptr)
    {
        menuHUD->RemoveComponent<GraphicsComponent>();
        engine.DestroyEntity(menuHUD);

        menuHUD = nullptr;
        graphics = nullptr;
    }
}
