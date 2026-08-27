#pragma once
#include "components/ui/UIScreen.hpp"
#include "core/globals.hpp"

#include "components/GraphicsComponent.hpp"
#include "components/ui/UIScreen.hpp"

#include <etl/array.h>
#include <nds.h>

#include <nds.h>

class MenuHUDScreen : public UIScreen
{
  public:
    static void create();
    static void destroy();
    static MenuHUDScreen* getInstance();

    void load();
    void unload();
    void renderSprites() override;
    void removeSprites() override;
    int onTouch(touchPosition* touch) override;

  private:
    MenuHUDScreen() : UIScreen(false) {};
    ~MenuHUDScreen() {};
    static MenuHUDScreen* instance;

    // NOTE: we can have max:
    // 1 moon
    // 1 day of the week
    // 4 numbers
    // 4 times
    // 18 skill progress items (all same sprite)

    // sprites
    etl::array<Sprite, 28> sprites = {}; // enough entries for moon, day, digits, times, and repeated skill markers
    etl::array<GraphicAsset, 4> numberSprites = {};
    etl::array<GraphicAsset, 4> timeSprites = {};
    etl::array<GraphicAsset, 18> skillSprites = {};
    GraphicAsset moonSprite = {};
    GraphicAsset dayOfWeekSprite = {};
    GraphicAsset slashSprite = {};

    bool bgLoaded;
    void loadBackground();

    ae::Entity* menuHUD = nullptr;
    GraphicsComponent* graphics = nullptr;
};
