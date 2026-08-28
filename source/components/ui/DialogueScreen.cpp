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

void DialogueScreen::renderSprites()
{
    // setup palettes
    dmaCopy(blueBlockGraphic.pal, SPRITE_PALETTE_SUB, 16 * sizeof(u16));
    dmaCopy(whiteBlockGraphic.pal, SPRITE_PALETTE_SUB + (1 * 16), 16 * sizeof(u16));
    dmaCopy(cornerGraphic.pal, SPRITE_PALETTE_SUB + (2 * 16), 16 * sizeof(u16));
    dmaCopy(edgeGraphic.pal, SPRITE_PALETTE_SUB + (3 * 16), 16 * sizeof(u16));
    // busts
    dmaCopy(topLeftGraphic.pal, SPRITE_PALETTE_SUB + (4 * 16), 16 * sizeof(u16));
    dmaCopy(topRightGraphic.pal, SPRITE_PALETTE_SUB + (5 * 16), 16 * sizeof(u16));
    dmaCopy(middleLeftGraphic.pal, SPRITE_PALETTE_SUB + (6 * 16), 16 * sizeof(u16));
    dmaCopy(middleRightGraphic.pal, SPRITE_PALETTE_SUB + (7 * 16), 16 * sizeof(u16));
    dmaCopy(bottomLeftGraphic.pal, SPRITE_PALETTE_SUB + (8 * 16), 16 * sizeof(u16));
    dmaCopy(bottomRightGraphic.pal, SPRITE_PALETTE_SUB + (9 * 16), 16 * sizeof(u16));

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
    // busts
    for (SpriteRenderState& srs : srsBusts)
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

void DialogueScreen::load()
{
    // create relevant entities, components
    if (dialogue == nullptr)
    {
        dialogue = engine.CreateEntity();
        graphics = engine.CreateComponent<GraphicsComponent>();
        dialogue->AddComponent(graphics);
    }

    // setup sprites
    blueBlockSprite = {SpriteSize_32x16, SpriteColorFormat_16Color, 0};
    whiteBlockSprite = {SpriteSize_32x16, SpriteColorFormat_16Color, 1};
    cornerSprite = {SpriteSize_32x16, SpriteColorFormat_16Color, 2};
    edgeSprite = {SpriteSize_32x16, SpriteColorFormat_16Color, 3};

    // allocating space for sprites
    blueBlockSprite.gfx = oamAllocateGfx(oam, blueBlockSprite.size, blueBlockSprite.format);
    whiteBlockSprite.gfx = oamAllocateGfx(oam, whiteBlockSprite.size, whiteBlockSprite.format);
    cornerSprite.gfx = oamAllocateGfx(oam, cornerSprite.size, cornerSprite.format);
    edgeSprite.gfx = oamAllocateGfx(oam, edgeSprite.size, edgeSprite.format);

    // load sprites
    std::string spritePath = "graphics/Dialogue/sprites/";
    blueBlockGraphic = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::BLUE_BLOCK);
    whiteBlockGraphic = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::WHITE_BLOCK);
    cornerGraphic = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::CORNER);
    edgeGraphic = graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::EDGE);
    // load alt palette sprites
    cornerGreenGraphic =
        genericGraphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::CORNER_GREEN);
    edgeGreenGraphic = genericGraphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::EDGE_GREEN);

    // copy sprites into memory
    dmaCopy(blueBlockGraphic.tiles, blueBlockSprite.gfx, blueBlockGraphic.tilesLen);
    dmaCopy(whiteBlockGraphic.tiles, whiteBlockSprite.gfx, whiteBlockGraphic.tilesLen);
    dmaCopy(cornerGraphic.tiles, cornerSprite.gfx, cornerGraphic.tilesLen);
    dmaCopy(edgeGraphic.tiles, edgeSprite.gfx, edgeGraphic.tilesLen);

    // bust demo
    loadBustDemo();
};

void DialogueScreen::loadBustDemo()
{
    // setup sprites
    // TODO: export as 1 sprite with the same palette, then splice
    topLeftSprite = {SpriteSize_64x64, SpriteColorFormat_16Color, 4};
    topRightSprite = {SpriteSize_64x64, SpriteColorFormat_16Color, 5};
    middleLeftSprite = {SpriteSize_64x64, SpriteColorFormat_16Color, 6};
    middleRightSprite = {SpriteSize_64x64, SpriteColorFormat_16Color, 7};
    bottomLeftSprite = {SpriteSize_64x64, SpriteColorFormat_16Color, 8};
    bottomRightSprite = {SpriteSize_64x64, SpriteColorFormat_16Color, 9};

    // allocating space for sprites
    topLeftSprite.gfx = oamAllocateGfx(oam, topLeftSprite.size, topLeftSprite.format);
    topRightSprite.gfx = oamAllocateGfx(oam, topRightSprite.size, topRightSprite.format);
    middleLeftSprite.gfx = oamAllocateGfx(oam, middleLeftSprite.size, middleLeftSprite.format);
    middleRightSprite.gfx = oamAllocateGfx(oam, middleRightSprite.size, middleRightSprite.format);
    bottomLeftSprite.gfx = oamAllocateGfx(oam, bottomLeftSprite.size, bottomLeftSprite.format);
    bottomRightSprite.gfx = oamAllocateGfx(oam, bottomRightSprite.size, bottomRightSprite.format);

    // load sprites
    std::string spritePath = "graphics/Busts/yukari/sprites/";
    topLeftGraphic = graphics->loadSpriteGraphic(spritePath, SpriteType::BUST, BustSprite::TOP_LEFT);
    topRightGraphic = graphics->loadSpriteGraphic(spritePath, SpriteType::BUST, BustSprite::TOP_RIGHT);
    middleLeftGraphic = graphics->loadSpriteGraphic(spritePath, SpriteType::BUST, BustSprite::MIDDLE_LEFT);
    middleRightGraphic = graphics->loadSpriteGraphic(spritePath, SpriteType::BUST, BustSprite::MIDDLE_RIGHT);
    bottomLeftGraphic = graphics->loadSpriteGraphic(spritePath, SpriteType::BUST, BustSprite::BOTTOM_LEFT);
    bottomRightGraphic = graphics->loadSpriteGraphic(spritePath, SpriteType::BUST, BustSprite::BOTTOM_RIGHT);

    // copy sprites into memory
    dmaCopy(topLeftGraphic.tiles, topLeftSprite.gfx, topLeftGraphic.tilesLen);
    dmaCopy(topRightGraphic.tiles, topRightSprite.gfx, topRightGraphic.tilesLen);
    dmaCopy(middleLeftGraphic.tiles, middleLeftSprite.gfx, middleLeftGraphic.tilesLen);
    dmaCopy(middleRightGraphic.tiles, middleRightSprite.gfx, middleRightGraphic.tilesLen);
    dmaCopy(bottomLeftGraphic.tiles, bottomLeftSprite.gfx, bottomLeftGraphic.tilesLen);
    dmaCopy(bottomRightGraphic.tiles, bottomRightSprite.gfx, bottomRightGraphic.tilesLen);
}

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

void DialogueScreen::triggerAction(UIAction action)
{
    switch (action)
    {
    // default palette
    case UIAction::SwitchToPalette0:
    {
        dmaCopy(cornerGraphic.pal, SPRITE_PALETTE_SUB + (2 * 16), 16 * sizeof(u16));
        dmaCopy(edgeGraphic.pal, SPRITE_PALETTE_SUB + (3 * 16), 16 * sizeof(u16));
        break;
    }

    // option selection palette
    case UIAction::SwitchToPalette1:
    {
        dmaCopy(cornerGreenGraphic.pal, SPRITE_PALETTE_SUB + (2 * 16), 16 * sizeof(u16));
        dmaCopy(edgeGreenGraphic.pal, SPRITE_PALETTE_SUB + (3 * 16), 16 * sizeof(u16));
        break;
    }

    default:
    {
        break;
    }
    }
}
