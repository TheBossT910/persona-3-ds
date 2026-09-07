#pragma once
#include "components/DialogueComponent.hpp"
#include "components/menus/UIMenu.hpp"
#include "controllers/AnimationController.hpp"
#include "managers/UIManager.hpp"
#include "systems/CameraSystem.hpp"
#include <etl/array.h>

enum class DebugOption
{
    DISCLAIMER_VIEW = 0,
    INTRO_VIEW,
    MAIN_MENU_VIEW,
    IWATODAI_DORM_VIEW,
    IWATODAI_STREETS_VIEW,
    STATION_VIEW,
    SIGN_CONTRACT_VIEW,
    PAULOWNIA_MALL_VIEW,
    INTRO_VIDEO,
    CUTSCENE_1,
    CUTSCENE_2,
    DEBUG_DIALOGUE,
    TOGGLE_BILLBOARDS,
    TOGGLE_DEBUG_PRINT,
    PLAY_CHARACTER_ANIM,
    CYCLE_CAMERA_MODE
};

class PauseMenuComponent : public UIMenu
{
  private:
    PauseMenuComponent() {};
    virtual ~PauseMenuComponent() = default;
    static PauseMenuComponent* instance;

    CameraSystem& cameraSystem = CameraSystem::GetInstance();
    UIManager& ui = UIManager::GetInstance();

    etl::array<CameraMode, 4> cameraModes = {
        CameraMode::Free, CameraMode::Static, CameraMode::CCTV, CameraMode::Follow};

    etl::array<MenuOption, 8> menuOptions = {
        MenuOption{"Debug", -1, MENU_BIND(PauseMenuComponent, openDebugMenu)},
        {"Skill", -1, MENU_BIND(PauseMenuComponent, openSkillMenu)},
        {"Item", -1, MENU_BIND(PauseMenuComponent, openItemMenu)},
        {"Persona", -1, MENU_BIND(PauseMenuComponent, openPersonaMenu)},
        {"Equip", -1, MENU_BIND(PauseMenuComponent, openEquipMenu)},
        {"Status", -1, MENU_BIND(PauseMenuComponent, openStatusMenu)},
        {"S.Link", -1, MENU_BIND(PauseMenuComponent, openSLinkMenu)},
        {"System", -1, MENU_BIND(PauseMenuComponent, openSystemMenu)},
    };

    etl::array<MenuOption, 16> debugOptions = {
        MenuOption{"DisclaimerView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"IntroView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"MainMenuView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"IwatodaiDormView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"IwatodaiStreetView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"StationView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"SignContractView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"PaulowniaMallView", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"IntroVideo", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"Cutscene1", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"Cutscene2", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"Debug Dialogue", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"Toggle Billboards", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"Toggle Debug Print", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
        {"Play Character Animations", -1, MENU_BIND(PauseMenuComponent, openCharacterAnimMenu)},
        {"Cycle Camera Mode", -1, MENU_BIND(PauseMenuComponent, debugOptionSelected)},
    };

    etl::array<MenuOption, 9> skillOptions = {
        MenuOption{"Makoto", 0, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Yukari", 1, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Junpei", 3, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Akihiko", 2, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Mitsuru", -1, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Aigis", -1, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Ken", -1, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Koromaru", -1, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
        {"Shinjiro", -1, MENU_BIND(PauseMenuComponent, skillOptionSelected)},
    };

    etl::array<MenuOption, 3> itemOptions = {
        MenuOption{"Life Stone", -1, MENU_BIND(PauseMenuComponent, itemOptionSelected)},
        {"Medicine", -1, MENU_BIND(PauseMenuComponent, itemOptionSelected)},
        {"Bead", -1, MENU_BIND(PauseMenuComponent, itemOptionSelected)},
    };

    etl::array<MenuOption, 9> equipOptions = {
        MenuOption{"Makoto", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Yukari", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Junpei", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Akihiko", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Mitsuru", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Aigis", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Ken", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Koromaru", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
        {"Shinjiro", -1, MENU_BIND(PauseMenuComponent, equipOptionSelected)},
    };

    etl::array<MenuOption, 3> personaOptions = {
        MenuOption{"Jack Frost", -1, MENU_BIND(PauseMenuComponent, personaOptionSelected)},
        {"Black Frost", -1, MENU_BIND(PauseMenuComponent, personaOptionSelected)},
        {"King Frost", -1, MENU_BIND(PauseMenuComponent, personaOptionSelected)},
    };

    etl::array<MenuOption, 9> statsOptions = {
        MenuOption{"Makoto", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Yukari", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Junpei", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Akihiko", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Mitsuru", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Aigis", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Ken", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Koromaru", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
        {"Shinjiro", -1, MENU_BIND(PauseMenuComponent, statsOptionSelected)},
    };

    etl::array<MenuOption, 3> sLinkOptions = {
        MenuOption{"Fool", -1, MENU_BIND(PauseMenuComponent, sLinkOptionSelected)},
        {"Magician", -1, MENU_BIND(PauseMenuComponent, sLinkOptionSelected)},
        {"Emperor", -1, MENU_BIND(PauseMenuComponent, sLinkOptionSelected)},
    };

    etl::array<MenuOption, 6> systemOptions = {
        MenuOption{"Tutorial", -1, MENU_BIND(PauseMenuComponent, systemOptionSelected)},
        {"Config", -1, MENU_BIND(PauseMenuComponent, systemOptionSelected)},
        {"Dictionary", -1, MENU_BIND(PauseMenuComponent, systemOptionSelected)},
        {"Load Data", -1, MENU_BIND(PauseMenuComponent, systemOptionSelected)},
        {"Save Data", -1, MENU_BIND(PauseMenuComponent, systemOptionSelected)},
        {"Return to Title", -1, MENU_BIND(PauseMenuComponent, systemOptionSelected)},
    };

    etl::array<MenuOption, 2> skills = {
        MenuOption{"Skill 1", -1, nullptr},
        {"Skill 2", -1, nullptr},
    };

    etl::array<MenuOption, 25> characterAnimOptions = {
        MenuOption{"Toggle Auto Animations", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0000", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0001", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0002", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0003", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0004", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0005", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0006", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0007", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0008", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0009", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0010", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0011", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0012", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0013", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0014", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0015", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0016", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0017", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0018", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0019", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0020", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0021", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"0022", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
        {"root", -1, MENU_BIND(PauseMenuComponent, characterAnimOptionSelected)},
    };

    // menu navigation handlers
    ViewState openDebugMenu();
    ViewState openSkillMenu();
    ViewState openItemMenu();
    ViewState openPersonaMenu();
    ViewState openEquipMenu();
    ViewState openStatusMenu();
    ViewState openSLinkMenu();
    ViewState openSystemMenu();
    ViewState openCharacterAnimMenu();

    // selection handlers
    ViewState debugOptionSelected();
    ViewState skillOptionSelected();
    ViewState itemOptionSelected();
    ViewState equipOptionSelected();
    ViewState personaOptionSelected();
    ViewState statsOptionSelected();
    ViewState sLinkOptionSelected();
    ViewState systemOptionSelected();
    ViewState characterAnimOptionSelected();

    bool isDialogueStarted = false;
    bool isDialoguePrevActive = false;
    ae::Entity* pauseMenu = nullptr;
    DialogueComponent* dialogue = nullptr;
    AnimationController* animationCtrl = AnimationController::getInstance();

    void resetHook() override;
    void closeHook() override;

  public:
    static void create();
    static void destroy();
    static PauseMenuComponent* getInstance();

    void cleanup();

    ViewState updateHook() override;

    bool isClosed = false;
};
