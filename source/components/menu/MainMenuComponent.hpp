#include "components/menu/UIMenu.hpp"

enum class MainMenuOptions
{
    LOAD_GAME = 0,
    SETTINGS,
    RETURN_TO_TITLE,
};

enum class LevelOptions
{
    START_GAME = 0,
    IWATODAI_DORM,
    IWATODAI_STREETS,
    STATION,
    PAULOWNIA_MALL,
    SIGN_CONTRACT,
};

enum class SettingIntroOptions
{
    ORIGINAL = 0,
    FES,
    PORTABLE,
    RELOAD
};

enum class SettingOptions
{
    CHANGE_INTRO_VIDEO = 0,
};

class MainMenuComponent : public UIMenu
{
  private:
    MainMenuComponent() {};
    virtual ~MainMenuComponent() = default;
    static MainMenuComponent* instance;
    MenuOption mainMenuOptions[3] = {
        {"Load Game", -1, MENU_BIND(MainMenuComponent, mainMenuOptionSelected)},
        {"Settings", -1, MENU_BIND(MainMenuComponent, mainMenuOptionSelected)},
        {"Return to Title", -1, MENU_BIND(MainMenuComponent, mainMenuOptionSelected)},
    };

    MenuOption levelOptions[6] = {
        {"Start Game", -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
        {"Iwatodai Dorm", -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
        {"Iwatodai Streets", -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
        {"Station", -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
        {"Paulownia Mall", -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
        {"Sign Contract", -1, MENU_BIND(MainMenuComponent, levelOptionSelected)},
    };

    MenuOption settingOptions[2] = {
        {"Change Intro Video", -1, MENU_BIND(MainMenuComponent, settingOptionSelected)},
        {"v1.1.0", -1, nullptr},
    };

    MenuOption settingIntroOptions[4] = {
        {"Original", -1, MENU_BIND(MainMenuComponent, settingIntroOptionSelected)},
        {"FES", -1, MENU_BIND(MainMenuComponent, settingIntroOptionSelected)},
        {"Portable", -1, MENU_BIND(MainMenuComponent, settingIntroOptionSelected)},
        {"Reload", -1, MENU_BIND(MainMenuComponent, settingIntroOptionSelected)},
    };

    // option handlers
    ViewState mainMenuOptionSelected();
    ViewState levelOptionSelected();
    ViewState settingOptionSelected();
    ViewState settingIntroOptionSelected();

    void resetHook() override;
    void closeHook() override;

    // helper
    void updateSave();

  public:
    static void create();
    static void destroy();
    static MainMenuComponent* getInstance();
};
