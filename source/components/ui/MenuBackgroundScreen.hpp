#pragma once

#include <nds.h>

#include "components/ui/UIScreen.hpp"
#include "core/structs.hpp"

#include "components/GraphicsComponent.hpp"

#define MENU_BACKGROUND_SCREEN_INVALID_BG_INDEX -1

class MenuBackgroundScreen : public UIScreen
{
  public:
    static void create();
    static void destroy();
    static MenuBackgroundScreen* getInstance();

    void load() override;
    void unload() override;

    void showBackground(int bgIndex);

    std::string resolveBgName(int bgIndex) const;

  private:
    MenuBackgroundScreen() {};
    ~MenuBackgroundScreen() {};

    static MenuBackgroundScreen* instance;

    int loadedBgIndex = MENU_BACKGROUND_SCREEN_INVALID_BG_INDEX;

    ae::Entity* menuBackground = nullptr;
    GraphicsComponent* graphics = nullptr;
};
