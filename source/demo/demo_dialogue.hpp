#pragma once
#include "data/spriteDb.hpp"
#include "types/DialogueTypes.hpp"
#include "types/GraphicsTypes.hpp"

// dialogue content
extern Dialogue demo_yukari_kenji_argument_lines[18];
void demo_yukari_kenji_argument_init();

// DialogueSelection actions
inline Dialogue* demo_yukari_kenji_argument_first()
{
    return &demo_yukari_kenji_argument_lines[0];
}
inline Dialogue* demo_yukari_kenji_argument_vouch()
{
    return &demo_yukari_kenji_argument_lines[7];
}
inline Dialogue* demo_yukari_kenji_argument_side_kenji()
{
    return &demo_yukari_kenji_argument_lines[12];
}
inline Dialogue* demo_yukari_kenji_argument_stay_out()
{
    return &demo_yukari_kenji_argument_lines[15];
}

// busts
// namespace name based on asset folder (character) name
// in graphics/busts/[character]
namespace BustYukari
{

// asset paths
inline std::string bustBasePath = "graphics/Busts/yukari/sprites/";
inline std::string bustExpressionPath = "graphics/Busts/yukari/sprites/";

// base bust
// variable names based on asset names
// in graphics/busts/[character]/base
inline Sprite topLeftSprite = {SpriteSize_64x64, SpriteColorFormat_16Color};
inline GraphicAsset topLeftGraphic = {};
inline SpriteRenderState topLeftSrs = {topLeftSprite, 0, 0};
inline SpritePayload topLeftSp = {
    topLeftSrs, bustBasePath, topLeftGraphic, SpriteType::BUST, (int)BustSprite::TOP_LEFT};

inline Sprite topRightSprite = {SpriteSize_64x64, SpriteColorFormat_16Color};
inline GraphicAsset topRightGraphic = {};
inline SpriteRenderState topRightSrs = {topRightSprite, 64, 0};
inline SpritePayload topRightSp = {
    topRightSrs, bustBasePath, topRightGraphic, SpriteType::BUST, (int)BustSprite::TOP_RIGHT};

inline Sprite middleLeftSprite = {SpriteSize_64x64, SpriteColorFormat_16Color};
inline GraphicAsset middleLeftGraphic = {};
inline SpriteRenderState middleLeftSrs = {middleLeftSprite, 0, 64};
inline SpritePayload middleLeftSp = {
    middleLeftSrs, bustBasePath, middleLeftGraphic, SpriteType::BUST, (int)BustSprite::MIDDLE_LEFT};

inline Sprite middleRightSprite = {SpriteSize_64x64, SpriteColorFormat_16Color};
inline GraphicAsset middleRightGraphic = {};
inline SpriteRenderState middleRightSrs = {middleRightSprite, 64, 64};
inline SpritePayload middleRightSp = {
    middleRightSrs, bustBasePath, middleRightGraphic, SpriteType::BUST, (int)BustSprite::MIDDLE_RIGHT};

inline Sprite bottomLeftSprite = {SpriteSize_64x64, SpriteColorFormat_16Color};
inline GraphicAsset bottomLeftGraphic = {};
inline SpriteRenderState bottomLeftSrs = {bottomLeftSprite, 0, 128};
inline SpritePayload bottomLeftSp = {
    bottomLeftSrs, bustBasePath, bottomLeftGraphic, SpriteType::BUST, (int)BustSprite::BOTTOM_LEFT};

inline Sprite bottomRightSprite = {SpriteSize_64x64, SpriteColorFormat_16Color};
inline GraphicAsset bottomRightGraphic = {};
inline SpriteRenderState bottomRightSrs = {bottomRightSprite, 64, 128};
inline SpritePayload bottomRightSp = {
    bottomRightSrs, bustBasePath, bottomRightGraphic, SpriteType::BUST, (int)BustSprite::BOTTOM_RIGHT};

// expressions
// pieces of experessions
// variable names based on asset names
// in graphics/busts/[character]/expressions
inline Sprite eyesNeutralSprite = {SpriteSize_32x32, SpriteColorFormat_16Color};
inline GraphicAsset eyesNeutralGraphic = {};
inline SpriteRenderState eyesNeutralSrs = {eyesNeutralSprite, 27, 46, 0};
inline SpritePayload eyesNeutralSp = {
    eyesNeutralSrs, bustExpressionPath, eyesNeutralGraphic, SpriteType::BUST, (int)BustSprite::EYES_NEUTRAL};

inline Sprite mouthNeutralSprite = {SpriteSize_32x32, SpriteColorFormat_16Color};
inline GraphicAsset mouthNeutralGraphic = {};
inline SpriteRenderState mouthNeutralSrs = {mouthNeutralSprite, 25, 59, 0};
inline SpritePayload mouthNeutralSp = {
    mouthNeutralSrs, bustExpressionPath, mouthNeutralGraphic, SpriteType::BUST, (int)BustSprite::MOUTH_NEUTRAL};

// combination of base bust + expressions to form one expression per array
inline etl::array<SpritePayload, 8> spNeutral = {
    topLeftSp, topRightSp, middleRightSp, bottomLeftSp, bottomRightSp, eyesNeutralSp, mouthNeutralSp, middleLeftSp};

} // namespace BustYukari

namespace BustAkihiko
{
inline etl::array<SpritePayload, 0> spNeutral = {};
}

namespace BustKenji
{
inline etl::array<SpritePayload, 0> spNeutral = {};
}
