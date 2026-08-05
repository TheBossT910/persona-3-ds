#include "PaulowniaMallView.h"

PaulowniaMallView::PaulowniaMallView()
{
}

void PaulowniaMallView::setMusic()
{
    musicCtrl->init(
        (fatBasePath + "music/locations/paulowniaMall/overworld/color_your_night.pcm").c_str(), 2.050f, 204.191f);
}

void PaulowniaMallView::setMovementConfig()
{
    movement->configureMovement(MovementConfig(PAULOWNIA_MALL_MAP_WIDTH,
                                               PAULOWNIA_MALL_MAP_HEIGHT,
                                               &paulownia_mall_map[0][0],
                                               tileSize,
                                               dbEntry->worldOffsetX,
                                               dbEntry->worldOffsetZ,
                                               characterSize,
                                               speed,
                                               height,
                                               characterTranslate,
                                               characterFacingAngle));
}

ViewState PaulowniaMallView::onTileCheck(TileType tile, u32 pressed)
{
    switch (tile)
    {
    // left
    case TileType::SCENE_0:
        return ViewState::IWATODAI_STREETS;
    // right
    case TileType::SCENE_1:
        return ViewState::IWATODAI_DORM;
    // middle
    case TileType::SCENE_2:
    case TileType::SCENE_3:
    case TileType::SCENE_4:
    case TileType::SCENE_5:
    case TileType::SCENE_6:
    case TileType::SCENE_7:
    case TileType::SCENE_8:
    case TileType::SCENE_9:
        return ViewState::STATION;
    default:
        break;
    }

    return ViewState::KEEP_CURRENT;
}

void PaulowniaMallView::onDialogueStart()
{
    // No dialogue currently
}
