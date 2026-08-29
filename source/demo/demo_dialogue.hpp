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
inline std::string bustExpressionPath = "";

// base bust
// variable names based on asset names
// in graphics/busts/[character]/base
inline Sprite topLeftSprite = {};
inline GraphicAsset topLeftGraphic = {};
inline SpriteRenderState topLeftSrs = {topLeftSprite, 0, 0};
inline SpritePayload topLeftSp = {
    topLeftSrs, bustBasePath, topLeftGraphic, SpriteType::BUST, (int)BustSprite::TOP_LEFT};

inline Sprite topRightSprite = {};
inline GraphicAsset topRightGraphic = {};
inline SpriteRenderState topRightSrs = {topRightSprite, 64, 0};
inline SpritePayload topRightSp = {
    topRightSrs, bustBasePath, topRightGraphic, SpriteType::BUST, (int)BustSprite::TOP_RIGHT};

inline Sprite middleLeftSprite = {};
inline GraphicAsset middleLeftGraphic = {};
inline SpriteRenderState middleLeftSrs = {middleLeftSprite, 0, 64};
inline SpritePayload middleLeftSp = {
    middleLeftSrs, bustBasePath, middleLeftGraphic, SpriteType::BUST, (int)BustSprite::MIDDLE_LEFT};

inline Sprite middleRightSprite = {};
inline GraphicAsset middleRightGraphic = {};
inline SpriteRenderState middleRightSrs = {middleRightSprite, 64, 64};
inline SpritePayload middleRightSp = {
    middleRightSrs, bustBasePath, middleRightGraphic, SpriteType::BUST, (int)BustSprite::MIDDLE_RIGHT};

inline Sprite bottomLeftSprite = {};
inline GraphicAsset bottomLeftGraphic = {};
inline SpriteRenderState bottomLeftSrs = {bottomLeftSprite, 0, 128};
inline SpritePayload bottomLeftSp = {
    bottomLeftSrs, bustBasePath, bottomLeftGraphic, SpriteType::BUST, (int)BustSprite::BOTTOM_LEFT};

inline Sprite bottomRightSprite = {};
inline GraphicAsset bottomRightGraphic = {};
inline SpriteRenderState bottomRightSrs = {bottomRightSprite, 64, 128};
inline SpritePayload bottomRightSp = {
    bottomRightSrs, bustBasePath, bottomRightGraphic, SpriteType::BUST, (int)BustSprite::BOTTOM_RIGHT};

// expressions
// pieces of experessions
// variable names based on asset names
// in graphics/busts/[character]/expressions
inline Sprite angryEyesSprite = {};
inline GraphicAsset angryEyesGraphic = {};
inline SpriteRenderState angryEyesSrs = {angryEyesSprite, 0, 0};
inline SpritePayload angryEyesSp = {
    angryEyesSrs, bustExpressionPath, angryEyesGraphic, SpriteType::BUST, (int)BustSprite::BOTTOM_LEFT};

inline Sprite neutralMouthSprite = {};
inline GraphicAsset neutralMouthGraphic = {};
inline SpriteRenderState neutralMouthSrs = {neutralMouthSprite, 0, 0};
inline SpritePayload neutralMouthSp = {
    neutralMouthSrs, bustExpressionPath, neutralMouthGraphic, SpriteType::BUST, (int)BustSprite::BOTTOM_LEFT};

// combination of base bust + expressions to form one expression per array
// inline etl::array<SpritePayload, 3> spAngry = {topLeftSp, angryEyesSp, neutralMouthSp};
// TODO: add actual expression SpritePayloads
inline etl::array<SpritePayload, 6> spAngry = {
    topLeftSp, topRightSp, middleLeftSp, middleRightSp, bottomLeftSp, bottomRightSp};

} // namespace BustYukari

namespace BustAkihiko
{
inline etl::array<SpritePayload, 0> spNeutral = {};
}

namespace BustKenji
{
inline etl::array<SpritePayload, 0> spNeutral = {};
}
