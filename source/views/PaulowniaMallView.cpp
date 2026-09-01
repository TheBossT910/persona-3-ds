#include "PaulowniaMallView.hpp"
#include "core/globals.hpp"
#include "events/UIEvents.hpp"

PaulowniaMallView::PaulowniaMallView()
{
}

void PaulowniaMallView::setupMusic()
{
    musicCtrl->init(
        (fatBasePath + "music/locations/paulowniaMall/overworld/color_your_night.pcm").c_str(), 2.050f, 204.191f);
}

void PaulowniaMallView::setupCamera()
{
    camConfig.mode = CameraMode::Follow;
    camConfig.initialAngle = 1.5708f * 2;
    camConfig.distance = 1.0f;
    camConfig.height = height + 0.4f;
    camConfig.lookAhead = 0.2f;
    camConfig.angleIncrement = 0.05f;
    camConfig.isRotationLocked = true;
}

void PaulowniaMallView::setupMovement()
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
    {
        return ViewState::IWATODAI_STREETS;
    }

    // right
    case TileType::SCENE_1:
    {
        return ViewState::IWATODAI_DORM;
    }

    // middle
    case TileType::SCENE_2:
    case TileType::SCENE_3:
    case TileType::SCENE_4:
    case TileType::SCENE_5:
    case TileType::SCENE_6:
    case TileType::SCENE_7:
    case TileType::SCENE_8:
    case TileType::SCENE_9:
    {
        return ViewState::STATION;
    }
    default:
    {
        break;
    }
    }

    return ViewState::KEEP_CURRENT;
}

void PaulowniaMallView::setupText()
{
    text->configureText(TextConfig(textVideoBuffer, &fontName, fontSize));
    textSub->configureText(TextConfig(textVideoBufferSub, &fontName, fontSize));
}

void PaulowniaMallView::setupUI()
{
    textSub->configureText(TextConfig(textVideoBufferSub, &fontName, fontSize));

    pauseMenuCmpt = PauseMenuComponent::getInstance();

    menuHUDScreen = MenuHUDScreen::getInstance();

    std::array<UIScreen*, 5> screens = {menuHUDScreen};
    std::array<UIMenu*, 10> menus = {pauseMenuCmpt};

    ae::BroadcastEvent(Event::ConfigureUIScreen{bgSub, bgMain, &oamSub, &oamMain, screens});
    ae::BroadcastEvent(Event::ConfigureUIMenu{textSub, menus});
}
