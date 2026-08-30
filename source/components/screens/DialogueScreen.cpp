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
    // load palettes
    dmaCopy(palette0, SPRITE_PALETTE_SUB, 16 * sizeof(u16));

    // perform transformations
    /// index -1 is reserved for vflip/hflip, 0 is reserved for no transform
    int i = 1;
    for (SpriteTransform& st : spriteTransforms)
    {
        oamRotateScale(oam, i++, st.angle, st.sx, st.sy);
    }

    // draw sprites
    spriteId = 0;
    for (SpritePayload& sp : spritePayloads)
    {
        // use alias for easy referencing
        SpriteRenderState& srs = sp.srs;

        oamSet(oam,
               spriteId++,
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

    // load sprites
    for (SpritePayload& sp : spritePayloads)
    {
        // use alias for easy referencing
        Sprite& sprite = sp.srs.sprite;
        GraphicAsset& graphic = sp.ga;

        // load graphic if not already loaded
        if (graphic.id <= -1)
        {
            // allocating space for sprite
            sprite.gfx = oamAllocateGfx(oam, sprite.size, sprite.format);

            // load sprite
            graphic = graphics->loadSpriteGraphic(sp.spritePath, sp.spriteType, sp.spriteVariant);

            // copy sprite into memory
            dmaCopy(graphic.tiles, sprite.gfx, graphic.tilesLen);
        }
    }

    // set palettes
    palette0 = blueBlockGraphic.pal;
    palette1 = (graphics->loadSpriteGraphic(spritePath, SpriteType::DIALOGUE, DialogueSprite::EDGE_GREEN)).pal;
};

void DialogueScreen::loadBust(etl::span<SpritePayload>& bust)
{
    // don't re-render the bust
    if (this->bust.data() == bust.data())
    {
        return;
    }

    // free previous bust sprites
    unloadBust();

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

        // load graphic if not already loaded
        if (graphic.id <= -1)
        {
            // allocating space for sprite
            sprite.gfx = oamAllocateGfx(oam, sprite.size, sprite.format);

            // load sprite
            graphic = graphics->loadSpriteGraphic(sp.spritePath, sp.spriteType, sp.spriteVariant);

            // copy sprite into memory
            dmaCopy(graphic.tiles, sprite.gfx, graphic.tilesLen);

            // save the latest palette
            bustPalette = graphic.pal;
        }
    }

    this->bust = bust;
}

void DialogueScreen::renderBust()
{
    // exit if no bust or if bustPalette is undefined
    if (bust.empty() || bustPalette == nullptr)
    {
        return;
    }

    // setup bust palette
    dmaCopy(bustPalette, SPRITE_PALETTE_SUB + (bustPaletteId * 16), 16 * sizeof(u16));

    // draw bust
    int bustId = spriteId;
    for (SpritePayload& sp : bust)
    {
        // use alias for easy referencing
        SpriteRenderState& srs = sp.srs;

        oamSet(oam,
               bustId++,
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

void DialogueScreen::unloadBust()
{
    int bustId = spriteId;
    for (SpritePayload& sp : bust)
    {
        // use alias for easy referencing
        Sprite& sprite = sp.srs.sprite;
        GraphicAsset& graphic = sp.ga;

        // hide sprite
        oamClearSprite(oam, bustId);

        // free sprite vram
        if (sprite.gfx != nullptr)
        {
            oamFreeGfx(oam, sprite.gfx);
        }

        // unload graphic if not already unloaded
        if (graphic.id > -1)
        {
            // unload sprite
            graphics->unloadGraphic(graphic);

            // reset data
            sprite.paletteAlpha = -1;
            graphic = {};
        }

        bustId++;
    }
}

void DialogueScreen::unload()
{
    // hide sprites
    removeSprites();

    // free sprites
    for (SpritePayload& sp : spritePayloads)
    {
        // use alias for easy referencing
        Sprite& sprite = sp.srs.sprite;
        GraphicAsset& graphic = sp.ga;

        // free sprite vram
        if (sprite.gfx != nullptr)
        {
            oamFreeGfx(oam, sprite.gfx);
        }

        // unload graphic if not already unloaded
        if (graphic.id > -1)
        {
            // unload sprite
            graphics->unloadGraphic(graphic);

            // reset data
            graphic = {};
        }
    }

    // free bust sprites
    unloadBust();

    if (dialogue != nullptr)
    {
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
        dmaCopy(palette0, SPRITE_PALETTE_SUB, 16 * sizeof(u16));
        break;
    }

    // option selection palette
    case UIAction::SwitchToPalette1:
    {
        dmaCopy(palette1, SPRITE_PALETTE_SUB, 16 * sizeof(u16));
        break;
    }

    default:
    {
        break;
    }
    }
}
