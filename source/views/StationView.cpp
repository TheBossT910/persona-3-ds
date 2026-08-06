#include "StationView.h"

StationView::StationView()
{
}

void StationView::setMusic()
{
    musicCtrl->init(
        (fatBasePath + "music/locations/paulowniaMall/station/paulownia_mall.pcm").c_str(), 2.002f, 73.939f);
}

void StationView::setMovementConfig()
{
    movement->configureMovement(MovementConfig(STATION_MAP_WIDTH,
                                               STATION_MAP_HEIGHT,
                                               &station_map[0][0],
                                               tileSize,
                                               dbEntry->worldOffsetX,
                                               dbEntry->worldOffsetZ,
                                               characterSize,
                                               speed,
                                               height,
                                               characterTranslate,
                                               characterFacingAngle));
}

ViewState StationView::onTileCheck(TileType tile, u32 pressed)
{
    switch (tile)
    {
    case TileType::SCENE_0:
        return ViewState::PAULOWNIA_MALL;
    default:
        break;
    }

    return ViewState::KEEP_CURRENT;
}

void StationView::setDialogueConfig()
{
    // No dialogue currently
}
