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

    std::string spritePath = "graphics/Dialogue/sprites/";

    // alt palette
    void* cornerGreenPalette = nullptr;
    void* edgeGreenPalette = nullptr;

    int spriteId = 0;

    ae::Entity* dialogue = nullptr;
    GraphicsComponent* graphics = nullptr;

    // ---
    // sprite setup

    // blueBlock sprites
    Sprite blueBlockSprite = {SpriteSize_32x16, SpriteColorFormat_16Color, 0};
    GraphicAsset blueBlockGraphic = {};

    SpriteRenderState srs0 = {blueBlockSprite, 0, 133};
    SpritePayload sp0 = {srs0, spritePath, blueBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::BLUE_BLOCK};

    SpriteRenderState srs1 = {blueBlockSprite, 32, 133};
    SpritePayload sp1 = {srs1, spritePath, blueBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::BLUE_BLOCK};

    SpriteRenderState srs2 = {blueBlockSprite, 64, 133};
    SpritePayload sp2 = {srs2, spritePath, blueBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::BLUE_BLOCK};

    SpriteRenderState srs3 = {blueBlockSprite, 96, 133};
    SpritePayload sp3 = {srs3, spritePath, blueBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::BLUE_BLOCK};

    SpriteRenderState srs4 = {blueBlockSprite, 128, 133};
    SpritePayload sp4 = {srs4, spritePath, blueBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::BLUE_BLOCK};

    SpriteRenderState srs5 = {blueBlockSprite, 160, 133};
    SpritePayload sp5 = {srs5, spritePath, blueBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::BLUE_BLOCK};

    SpriteRenderState srs6 = {blueBlockSprite, 192, 133};
    SpritePayload sp6 = {srs6, spritePath, blueBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::BLUE_BLOCK};

    SpriteRenderState srs7 = {blueBlockSprite, 224, 133};
    SpritePayload sp7 = {srs7, spritePath, blueBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::BLUE_BLOCK};

    // corner sprites
    Sprite cornerSprite = {SpriteSize_32x16, SpriteColorFormat_16Color, 2};
    GraphicAsset cornerGraphic = {};

    SpriteRenderState srs8 = {cornerSprite, 224, 149};
    SpritePayload sp8 = {srs8, spritePath, cornerGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::CORNER};

    SpriteRenderState srs9 = {cornerSprite, 0, 149, 1, -1, true, false};
    SpritePayload sp9 = {srs9, spritePath, cornerGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::CORNER};

    SpriteRenderState srs10 = {cornerSprite, 0, 176, 1, -1, true, true};
    SpritePayload sp10 = {srs10, spritePath, cornerGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::CORNER};

    SpriteRenderState srs11 = {cornerSprite, 224, 176, 1, -1, false, true};
    SpritePayload sp11 = {srs11, spritePath, cornerGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::CORNER};

    // edge sprites
    Sprite edgeSprite = {SpriteSize_32x16, SpriteColorFormat_16Color, 3};
    GraphicAsset edgeGraphic = {};

    SpriteRenderState srs12 = {edgeSprite, 32, 149};
    SpritePayload sp12 = {srs12, spritePath, edgeGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::EDGE};

    SpriteRenderState srs13 = {edgeSprite, 64, 149};
    SpritePayload sp13 = {srs13, spritePath, edgeGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::EDGE};

    SpriteRenderState srs14 = {edgeSprite, 96, 149};
    SpritePayload sp14 = {srs14, spritePath, edgeGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::EDGE};

    SpriteRenderState srs15 = {edgeSprite, 128, 149};
    SpritePayload sp15 = {srs15, spritePath, edgeGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::EDGE};

    SpriteRenderState srs16 = {edgeSprite, 160, 149};
    SpritePayload sp16 = {srs16, spritePath, edgeGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::EDGE};

    SpriteRenderState srs17 = {edgeSprite, 192, 149};
    SpritePayload sp17 = {srs17, spritePath, edgeGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::EDGE};

    SpriteRenderState srs18 = {edgeSprite, 32, 176, 1, -1, false, true};
    SpritePayload sp18 = {srs18, spritePath, edgeGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::EDGE};

    SpriteRenderState srs19 = {edgeSprite, 64, 176, 1, -1, false, true};
    SpritePayload sp19 = {srs19, spritePath, edgeGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::EDGE};

    SpriteRenderState srs20 = {edgeSprite, 96, 176, 1, -1, false, true};
    SpritePayload sp20 = {srs20, spritePath, edgeGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::EDGE};

    SpriteRenderState srs21 = {edgeSprite, 128, 176, 1, -1, false, true};
    SpritePayload sp21 = {srs21, spritePath, edgeGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::EDGE};

    SpriteRenderState srs22 = {edgeSprite, 160, 176, 1, -1, false, true};
    SpritePayload sp22 = {srs22, spritePath, edgeGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::EDGE};

    SpriteRenderState srs23 = {edgeSprite, 192, 176, 1, -1, false, true};
    SpritePayload sp23 = {srs23, spritePath, edgeGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::EDGE};

    SpriteRenderState srs24 = {edgeSprite, -8, 165, 1, 1};
    SpritePayload sp24 = {srs24, spritePath, edgeGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::EDGE};

    SpriteRenderState srs25 = {edgeSprite, 231, 165, 1, 2};
    SpritePayload sp25 = {srs25, spritePath, edgeGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::EDGE};

    // whiteBlock sprites
    Sprite whiteBlockSprite = {SpriteSize_32x16, SpriteColorFormat_16Color, 1};
    GraphicAsset whiteBlockGraphic = {};

    SpriteRenderState srs26 = {whiteBlockSprite, 0, 165};
    SpritePayload sp26 = {srs26, spritePath, whiteBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::WHITE_BLOCK};

    SpriteRenderState srs27 = {whiteBlockSprite, 32, 165};
    SpritePayload sp27 = {srs27, spritePath, whiteBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::WHITE_BLOCK};

    SpriteRenderState srs28 = {whiteBlockSprite, 64, 165};
    SpritePayload sp28 = {srs28, spritePath, whiteBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::WHITE_BLOCK};

    SpriteRenderState srs29 = {whiteBlockSprite, 96, 165};
    SpritePayload sp29 = {srs29, spritePath, whiteBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::WHITE_BLOCK};

    SpriteRenderState srs30 = {whiteBlockSprite, 128, 165};
    SpritePayload sp30 = {srs30, spritePath, whiteBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::WHITE_BLOCK};

    SpriteRenderState srs31 = {whiteBlockSprite, 160, 165};
    SpritePayload sp31 = {srs31, spritePath, whiteBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::WHITE_BLOCK};

    SpriteRenderState srs32 = {whiteBlockSprite, 192, 165};
    SpritePayload sp32 = {srs32, spritePath, whiteBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::WHITE_BLOCK};

    SpriteRenderState srs33 = {whiteBlockSprite, 224, 165};
    SpritePayload sp33 = {srs33, spritePath, whiteBlockGraphic, SpriteType::DIALOGUE, (int)DialogueSprite::WHITE_BLOCK};

    // sprite transforms
    SpriteTransform st0 = {degreesToAngle(90), intToFixed(1, 8), intToFixed(1, 8)};
    SpriteTransform st1 = {degreesToAngle(270), intToFixed(1, 8), intToFixed(1, 8)};

    // data groups
    etl::array<GraphicAsset*, 4> spritePalettes = {&blueBlockGraphic, &whiteBlockGraphic, &cornerGraphic, &edgeGraphic};

    etl::array<SpritePayload, 34> spritePayloads = {
        sp0,  sp1,  sp2,  sp3,  sp4,  sp5,  sp6,  sp7,  sp8,  sp9,  sp10, sp11, sp12, sp13, sp14, sp15, sp16,
        sp17, sp18, sp19, sp20, sp21, sp22, sp23, sp24, sp25, sp26, sp27, sp28, sp29, sp30, sp31, sp32, sp33};

    etl::array<SpriteTransform, 2> spriteTransforms = {st0, st1};
};
