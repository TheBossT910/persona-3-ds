#pragma once

#include "components/GraphicsComponent.hpp"
#include "components/ui/UIScreen.hpp"

#include <etl/array.h>
#include <etl/span.h>
#include <nds.h>

class DialogueScreen : public UIScreen
{
  public:
    static void create();
    static void destroy();
    static DialogueScreen* getInstance();

    void load();
    void unload();
    void triggerAction(UIAction action) override;
    void renderSprites() override;

    // debug
    void loadBust(etl::span<SpritePayload>& bust);
    void renderBust();

  private:
    DialogueScreen() : UIScreen(false) {};
    ~DialogueScreen() {};
    static DialogueScreen* instance;

    // busts
    etl::span<SpritePayload> bust;
    void* bustPalette = nullptr;
    int bustPaletteId = 4;

    // sprites
    Sprite blueBlockSprite = {};
    Sprite whiteBlockSprite = {};
    Sprite cornerSprite = {};
    Sprite edgeSprite = {};

    // graphics
    GraphicAsset blueBlockGraphic = {};
    GraphicAsset whiteBlockGraphic = {};
    GraphicAsset cornerGraphic = {};
    GraphicAsset edgeGraphic = {};

    // alt palette
    void* cornerGreenPalette = nullptr;
    void* edgeGreenPalette = nullptr;

    int srsId = 0;

    ae::Entity* dialogue = nullptr;
    GraphicsComponent* graphics = nullptr;

    // ---
    // sprite setup
    // blueBlock sprites
    SpriteRenderState srs0 = {blueBlockSprite, 0, 133};
    SpriteRenderState srs1 = {blueBlockSprite, 32, 133};
    SpriteRenderState srs2 = {blueBlockSprite, 64, 133};
    SpriteRenderState srs3 = {blueBlockSprite, 96, 133};
    SpriteRenderState srs4 = {blueBlockSprite, 128, 133};
    SpriteRenderState srs5 = {blueBlockSprite, 160, 133};
    SpriteRenderState srs6 = {blueBlockSprite, 192, 133};
    SpriteRenderState srs7 = {blueBlockSprite, 224, 133};

    // corner sprites
    SpriteRenderState srs8 = {cornerSprite, 224, 149};
    SpriteRenderState srs9 = {cornerSprite, 0, 149, 1, -1, true, false};
    SpriteRenderState srs10 = {cornerSprite, 0, 176, 1, -1, true, true};
    SpriteRenderState srs11 = {cornerSprite, 224, 176, 1, -1, false, true};

    // edge sprites
    SpriteRenderState srs12 = {edgeSprite, 32, 149};
    SpriteRenderState srs13 = {edgeSprite, 64, 149};
    SpriteRenderState srs14 = {edgeSprite, 96, 149};
    SpriteRenderState srs15 = {edgeSprite, 128, 149};
    SpriteRenderState srs16 = {edgeSprite, 160, 149};
    SpriteRenderState srs17 = {edgeSprite, 192, 149};
    SpriteRenderState srs18 = {edgeSprite, 32, 176, 1, -1, false, true};
    SpriteRenderState srs19 = {edgeSprite, 64, 176, 1, -1, false, true};
    SpriteRenderState srs20 = {edgeSprite, 96, 176, 1, -1, false, true};
    SpriteRenderState srs21 = {edgeSprite, 128, 176, 1, -1, false, true};
    SpriteRenderState srs22 = {edgeSprite, 160, 176, 1, -1, false, true};
    SpriteRenderState srs23 = {edgeSprite, 192, 176, 1, -1, false, true};
    SpriteRenderState srs24 = {edgeSprite, -8, 165, 1, 1};
    SpriteRenderState srs25 = {edgeSprite, 231, 165, 1, 2};

    // whiteBlock sprites
    SpriteRenderState srs26 = {whiteBlockSprite, 0, 165};
    SpriteRenderState srs27 = {whiteBlockSprite, 32, 165};
    SpriteRenderState srs28 = {whiteBlockSprite, 64, 165};
    SpriteRenderState srs29 = {whiteBlockSprite, 96, 165};
    SpriteRenderState srs30 = {whiteBlockSprite, 128, 165};
    SpriteRenderState srs31 = {whiteBlockSprite, 160, 165};
    SpriteRenderState srs32 = {whiteBlockSprite, 192, 165};
    SpriteRenderState srs33 = {whiteBlockSprite, 224, 165};

    etl::array<SpriteRenderState, 34> spriteRenderStates = {
        srs0,  srs1,  srs2,  srs3,  srs4,  srs5,  srs6,  srs7,  srs8,  srs9,  srs10, srs11,
        srs12, srs13, srs14, srs15, srs16, srs17, srs18, srs19, srs20, srs21, srs22, srs23,
        srs24, srs25, srs26, srs27, srs28, srs29, srs30, srs31, srs32, srs33};

    SpriteTransform st0 = {degreesToAngle(90), intToFixed(1, 8), intToFixed(1, 8)};
    SpriteTransform st1 = {degreesToAngle(270), intToFixed(1, 8), intToFixed(1, 8)};

    etl::array<SpriteTransform, 2> spriteTransforms = {st0, st1};
    // ---
};
