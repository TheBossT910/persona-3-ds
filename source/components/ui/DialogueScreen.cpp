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

void DialogueScreen::renderSprites()
{
    // setup palettes
    dmaCopy(blueBlockGraphic.pal, SPRITE_PALETTE_SUB, 16 * sizeof(u16));
    dmaCopy(whiteBlockGraphic.pal, SPRITE_PALETTE_SUB + (1 * 16), 16 * sizeof(u16));
    dmaCopy(cornerGraphic.pal, SPRITE_PALETTE_SUB + (2 * 16), 16 * sizeof(u16));
    dmaCopy(edgeGraphic.pal, SPRITE_PALETTE_SUB + (3 * 16), 16 * sizeof(u16));

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
    blueBlockSprite = {0, SpriteSize_32x16, SpriteColorFormat_16Color, 0, 0, 0, 133};
    whiteBlockSprite = {0, SpriteSize_32x16, SpriteColorFormat_16Color, 0, 1, 0, 165};
    cornerSprite = {0, SpriteSize_32x16, SpriteColorFormat_16Color, -1, 2, 0, 0};
    edgeSprite = {0, SpriteSize_32x16, SpriteColorFormat_16Color, -1, 3, 0, 0};

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

    // copy sprites into memory
    dmaCopy(blueBlockGraphic.tiles, blueBlockSprite.gfx, blueBlockGraphic.tilesLen);
    dmaCopy(whiteBlockGraphic.tiles, whiteBlockSprite.gfx, whiteBlockGraphic.tilesLen);
    dmaCopy(cornerGraphic.tiles, cornerSprite.gfx, cornerGraphic.tilesLen);
    dmaCopy(edgeGraphic.tiles, edgeSprite.gfx, edgeGraphic.tilesLen);
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
