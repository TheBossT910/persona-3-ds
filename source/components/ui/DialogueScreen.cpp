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

    // perform transformations
    /// index -1 is reserved for vflip/hflip, 0 is reserved for no transform
    int stId = 1;
    for (SpriteTransform& st : spriteTransforms)
    {
        oamRotateScale(oam, stId++, st.angle, st.sx, st.sy);
    }

    // draw sprites
    srsId = 0;
    for (SpriteRenderState& srs : spriteRenderStates)
    {
        oamSet(oam,
               srsId++,
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

    // load alt palettes
    cornerGreenPalette =
        (graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::CORNER_GREEN)).pal;
    edgeGreenPalette = (graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::EDGE_GREEN)).pal;

    // copy sprites into memory
    dmaCopy(blueBlockGraphic.tiles, blueBlockSprite.gfx, blueBlockGraphic.tilesLen);
    dmaCopy(whiteBlockGraphic.tiles, whiteBlockSprite.gfx, whiteBlockGraphic.tilesLen);
    dmaCopy(cornerGraphic.tiles, cornerSprite.gfx, cornerGraphic.tilesLen);
    dmaCopy(edgeGraphic.tiles, edgeSprite.gfx, edgeGraphic.tilesLen);
};

void DialogueScreen::loadBust(etl::span<SpritePayload>& bust)
{
    // don't re-render the bust
    if (this->bust.data() == bust.data())
    {
        return;
    }

    // free previous bust
    for (SpritePayload& sp : this->bust)
    {
        // use alias for easy referencing
        Sprite& sprite = sp.srs.sprite;
        GraphicAsset& graphic = sp.ga;

        // free vram
        if (sprite.gfx != nullptr)
        {
            oamFreeGfx(oam, sprite.gfx);
        }

        // unload from memory
        graphics->unloadGraphic(graphic);

        // reset data
        sprite.paletteAlpha = -1;
        graphic = {};
    }

    // clear old bust palette
    dmaFillHalfWords(0, SPRITE_PALETTE_SUB + (bustPaletteId * 16), 16 * sizeof(u16));
    bustPalette = nullptr;

    // load new bust
    for (SpritePayload& sp : bust)
    {
        // use alias for easy referencing
        Sprite& sprite = sp.srs.sprite;
        GraphicAsset& graphic = sp.ga;

        // setup sprite
        sprite.paletteAlpha = bustPaletteId;

        // allocating space for sprite
        sprite.gfx = oamAllocateGfx(oam, sprite.size, sprite.format);

        // load sprite
        graphic = graphics->loadSpriteGraphic(sp.spritePath, sp.spriteType, sp.spriteVariant);

        // copy sprite into memory
        dmaCopy(graphic.tiles, sprite.gfx, graphic.tilesLen);

        // save the latest palette
        bustPalette = graphic.pal;
    }

    this->bust = bust;
}

void DialogueScreen::renderBust()
{
    // exit if no bust
    if (bust.empty())
    {
        return;
    }

    // setup bust palette
    dmaCopy(bustPalette, SPRITE_PALETTE_SUB + (bustPaletteId * 16), 16 * sizeof(u16));

    // draw bust
    for (SpritePayload& sp : bust)
    {
        // use alias for easy referencing
        SpriteRenderState& srs = sp.srs;

        oamSet(oam,
               srsId++,
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

    // clear palettes
    for (int i = 0; i < 5; i++)
    {
        dmaFillHalfWords(0, SPRITE_PALETTE_SUB + (i * 16), 16 * sizeof(u16));
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
        dmaCopy(cornerGreenPalette, SPRITE_PALETTE_SUB + (2 * 16), 16 * sizeof(u16));
        dmaCopy(edgeGreenPalette, SPRITE_PALETTE_SUB + (3 * 16), 16 * sizeof(u16));
        break;
    }

    default:
    {
        break;
    }
    }
}
