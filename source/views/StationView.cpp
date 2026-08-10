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

void StationView::setTextConfig()
{
    text->configureText(TextConfig(textVideoBuffer, &FONT_NAME, FONT_SIZE));
    textSub->configureText(TextConfig(textVideoBufferSub, &FONT_NAME, FONT_SIZE));
}

void StationView::setupUI()
{
    textMenu->configureText(TextConfig(textVideoBufferSub, &FONT_NAME, FONT_SIZE));

    pauseMenuCmpt = PauseMenuComponent::getInstance();

    // setup pause menu
    pauseMenuCmpt->init(bgSharedSub1, &Globals::isPauseMenuActive, textMenu);

    // TODO: replace this. We shouldn't be calling MenuBackgroundScreen here
    MenuBackgroundScreen::getInstance()->bgId = bgSharedSub1;
    MenuBackgroundScreen::getInstance()->load();

    menuHUDScreen = MenuHUDScreen::getInstance();

    uiCtrl->registerScreen(menuHUDScreen, false);
    uiCtrl->show(menuHUDScreen, false);
}

void StationView::hookCleanup()
{
    if (pauseMenuCmpt != nullptr)
    {
        pauseMenuCmpt->cancelSFX();
    }
}
