#pragma once
#include "data/spriteDb.hpp"
#include "types/DialogueTypes.hpp"
#include "types/GraphicsTypes.hpp"

/**
 * KNOWN LIMITATIONS
 * If a sprite needs to be moved to a different x/y pos, a completely new sprite needs to be created.
 *
 * Some boilerplate can be easily reduced. For example, we may not need to explicitly declare either a
 * Sprite or GraphicAsset. Right now, a Sprite needs to be explicitly declared since a SpriteRenderState
 * requires a Sprite& reference.
 */

// dialogue content
extern Dialogue demo_yukari_kenji_argument_lines[18];

// init() needs to be called to properly set the dialogue content
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
// namespace name based on asset folder name in graphics/busts/[name]
namespace BustYukari
{

// asset paths
// we can have as many unique paths as we want here, there isn't any restriction
// the possible asset paths need to be explicitly set
inline std::string assetPath = "graphics/Busts/yukari/sprites/";

// base bust
// the variable names should be based on the asset's names
// each asset will need 4 structs: Sprite, GraphicAsset, SpriteRenderState, SpritePayload

// set the sprite size and sprite colour format. We CANNOT assume these! They need to be explicitly set
inline Sprite topLeftSprite = {SpriteSize_64x64, SpriteColorFormat_16Color};
// empty
inline GraphicAsset topLeftGraphic = {};
// set the sprite, the x position, and the y position to draw the sprite to
inline SpriteRenderState topLeftSrs = {topLeftSprite, 0, 0};
// set the SpriteRenderState, asset path, GraphicAsset & critically the SpriteType, and the sprite variant
// the asset path, SpriteType and sprite variant need to be explicitly set
inline SpritePayload topLeftSp = {topLeftSrs, assetPath, topLeftGraphic, SpriteType::BUST, (int)BustSprite::TOP_LEFT};

inline Sprite topRightSprite = {SpriteSize_64x64, SpriteColorFormat_16Color};
inline GraphicAsset topRightGraphic = {};
inline SpriteRenderState topRightSrs = {topRightSprite, 64, 0};
inline SpritePayload topRightSp = {
    topRightSrs, assetPath, topRightGraphic, SpriteType::BUST, (int)BustSprite::TOP_RIGHT};

inline Sprite middleLeftSprite = {SpriteSize_64x64, SpriteColorFormat_16Color};
inline GraphicAsset middleLeftGraphic = {};
inline SpriteRenderState middleLeftSrs = {middleLeftSprite, 0, 64};
inline SpritePayload middleLeftSp = {
    middleLeftSrs, assetPath, middleLeftGraphic, SpriteType::BUST, (int)BustSprite::MIDDLE_LEFT};

inline Sprite middleRightSprite = {SpriteSize_64x64, SpriteColorFormat_16Color};
inline GraphicAsset middleRightGraphic = {};
inline SpriteRenderState middleRightSrs = {middleRightSprite, 64, 64};
inline SpritePayload middleRightSp = {
    middleRightSrs, assetPath, middleRightGraphic, SpriteType::BUST, (int)BustSprite::MIDDLE_RIGHT};

inline Sprite bottomLeftSprite = {SpriteSize_64x64, SpriteColorFormat_16Color};
inline GraphicAsset bottomLeftGraphic = {};
inline SpriteRenderState bottomLeftSrs = {bottomLeftSprite, 0, 128};
inline SpritePayload bottomLeftSp = {
    bottomLeftSrs, assetPath, bottomLeftGraphic, SpriteType::BUST, (int)BustSprite::BOTTOM_LEFT};

inline Sprite bottomRightSprite = {SpriteSize_64x64, SpriteColorFormat_16Color};
inline GraphicAsset bottomRightGraphic = {};
inline SpriteRenderState bottomRightSrs = {bottomRightSprite, 64, 128};
inline SpritePayload bottomRightSp = {
    bottomRightSrs, assetPath, bottomRightGraphic, SpriteType::BUST, (int)BustSprite::BOTTOM_RIGHT};

// expressions
inline Sprite eyesNeutralSprite = {SpriteSize_32x32, SpriteColorFormat_16Color};
inline GraphicAsset eyesNeutralGraphic = {};
inline SpriteRenderState eyesNeutralSrs = {eyesNeutralSprite, 27, 46, 0};
inline SpritePayload eyesNeutralSp = {
    eyesNeutralSrs, assetPath, eyesNeutralGraphic, SpriteType::BUST, (int)BustSprite::EYES_NEUTRAL};

inline Sprite mouthNeutralSprite = {SpriteSize_32x32, SpriteColorFormat_16Color};
inline GraphicAsset mouthNeutralGraphic = {};
inline SpriteRenderState mouthNeutralSrs = {mouthNeutralSprite, 25, 59, 0};
inline SpritePayload mouthNeutralSp = {
    mouthNeutralSrs, assetPath, mouthNeutralGraphic, SpriteType::BUST, (int)BustSprite::MOUTH_NEUTRAL};

// after all assets have been setup, they all get grouped into SpritePayload arrays
// there can be any amount of SpritePayload arrays. Each array builds a unique image (ex. AngryYukari, HappyYukari, NeutralYukari, etc.)
// each SpritePayload array needs to be explicitly set
// you can think that this is *grouping* the above "building blocks" together
inline etl::array<SpritePayload, 8> spNeutral = {
    topLeftSp, topRightSp, middleRightSp, bottomLeftSp, bottomRightSp, eyesNeutralSp, mouthNeutralSp, middleLeftSp};

} // namespace BustYukari

namespace BustAkihiko
{
inline etl::array<SpritePayload, 0> spNeutral = {};
} // namespace BustAkihiko

namespace BustKenji
{
inline etl::array<SpritePayload, 0> spNeutral = {};
} // namespace BustKenji
