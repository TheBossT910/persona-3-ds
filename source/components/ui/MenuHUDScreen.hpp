#pragma once
#include "components/ui/UIScreen.hpp"
#include "core/globals.hpp"

#include "components/GraphicsComponent.hpp"

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
    Sprite sprites[28]; // enough entries for moon, day, digits, times, and repeated skill markers
    GraphicAsset moonSprite;
    GraphicAsset dayOfWeekSprite;
    GraphicAsset numberSprites[4];
    GraphicAsset timeSprites[4];
    GraphicAsset skillSprites[18];
    GraphicAsset slashSprite;
    bool bgLoaded;
    void loadBackground();

    ae::Entity* menuHUD = nullptr;
    GraphicsComponent* graphics = nullptr;
};
