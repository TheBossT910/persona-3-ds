#include "IwatodaiDormView.h"
#include "core/structs.h"

// TODO: dont forget to clear in future
IwatodaiDormView::IwatodaiDormView()
{
}

// Test path
static CameraPath dormTestPath = {{
    {120, {-0.40f, 0.60f, 2.82f}, {0.4f, 0.1f, 2.80f}},
    {0, {-0.40f, 0.60f, 2.82f}, {0.4f, 0.1f, 2.80f}},
    {60, {0.40f, 0.80f, 1.80f}, {0.4f, 0.1f, 2.80f}},
    {120, {1.20f, 0.60f, 2.82f}, {0.4f, 0.1f, 2.80f}},
    {180, {0.40f, 0.40f, 3.80f}, {0.4f, 0.1f, 2.80f}},
    {240, {-0.40f, 0.60f, 2.82f}, {0.4f, 0.1f, 2.80f}},
}};

void IwatodaiDormView::setCameraConfig()
{
    camConfig.mode = CameraMode::Path;
    camConfig.initialAngle = -1.6f;
    camConfig.distance = 0.8f;
    camConfig.height = height + 0.6f;
    camConfig.lookAhead = 0.2f;
    camConfig.angleIncrement = 0.07f;
    ae::BroadcastEvent(Event::SetCameraPath{&dormTestPath});
}

void IwatodaiDormView::setMusic()
{
    musicCtrl->init((fatBasePath + "music/locations/iwatodaiDorm/iwatodai_dorm.pcm").c_str(), 1.300f, -1.000f);
}

void IwatodaiDormView::setMovementConfig()
{
    movement->configureMovement(MovementConfig(IWATODAI_DORM_FLOOR_1_MAP_WIDTH,
                                               IWATODAI_DORM_FLOOR_1_MAP_HEIGHT,
                                               &iwatodai_dorm_floor_1_map[0][0],
                                               tileSize,
                                               dbEntry->worldOffsetX,
                                               dbEntry->worldOffsetZ,
                                               characterSize,
                                               speed,
                                               height,
                                               characterTranslate,
                                               characterFacingAngle));
}

ViewState IwatodaiDormView::onTileCheck(TileType tile, u32 pressed)
{
    switch (tile)
    {
    case TileType::SCENE_1:
        return ViewState::PAULOWNIA_MALL;
    case TileType::SCENE_0:
        return ViewState::IWATODAI_STREETS;
    case TileType::C_AK:
        // start dialogue
        if (!promptDrawn)
        {
            textSub->drawText("Talk", 0, 0, TextColor::White);
            promptDrawn = true;
        }
        if (pressed & KEY_A)
        {
            prevEnvironmentState = false;
            phase = ViewPhase::Dialogue;
        }
        break;
    default:
        if (promptDrawn)
        {
            textSub->clearScreen();
            promptDrawn = false;
        }
        break;
    }
    return ViewState::KEEP_CURRENT;
}

void IwatodaiDormView::setDialogueConfig()
{
    demo_yukari_kenji_argument_load();
    dialogue->configureDialogue(
        DialogueConfig(demo_yukari_kenji_argument_first(), demo_yukari_kenji_argument_load_bg, textMenu));
}

void IwatodaiDormView::setTextConfig()
{
    text->configureText(TextConfig(textVideoBuffer, &FONT_NAME, FONT_SIZE));
    textSub->configureText(TextConfig(textVideoBufferSub, &FONT_NAME, FONT_SIZE));
}

void IwatodaiDormView::setupUI()
{
    textMenu->configureText(TextConfig(textVideoBufferSub, &FONT_NAME, FONT_SIZE));

    // setup pause menu
    pauseMenuCmpt = PauseMenuComponent::getInstance();

    menuHUDScreen = MenuHUDScreen::getInstance();
    dialogueScreen = DialogueScreen::getInstance();

    std::array<UIScreen*, 5> screens = {menuHUDScreen, dialogueScreen};
    std::array<UIMenu*, 10> menus = {pauseMenuCmpt};

    ae::BroadcastEvent(Event::ConfigureUIScreen{bgSub, bgMain, &oamSub, &oamMain, screens});
    ae::BroadcastEvent(Event::ConfigureUIMenu{textMenu, menus});
}
