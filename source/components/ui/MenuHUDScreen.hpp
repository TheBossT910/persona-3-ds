#pragma once
#include "components/ui/UIScreen.hpp"

#include "components/GraphicsComponent.hpp"
#include "components/ui/UIScreen.hpp"

#include <etl/array.h>
#include <nds.h>

#include <etl/array.h>
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

    // sprites
    Sprite moonSprite = {};
    Sprite dayOfWeekSprite = {};
    Sprite digitSprites[3] = {};
    Sprite timeSprites[4] = {};
    Sprite skillSprite = {};
    Sprite slashSprite = {};

    // graphics
    GraphicAsset moonGraphic = {};
    GraphicAsset dayOfWeekGraphic = {};
    GraphicAsset digitGraphics[3] = {};
    GraphicAsset timeGraphics[4] = {};
    GraphicAsset skillGraphic = {};
    GraphicAsset slashGraphic = {};

    bool bgLoaded;
    void loadBackground();

    ae::Entity* menuHUD = nullptr;
    GraphicsComponent* graphics = nullptr;

    // ---
    // sprite setup
    SpriteRenderState srs0 = {moonSprite, 202, -15, 1, 0, false, false, false, true};
    SpriteRenderState srs1 = {dayOfWeekSprite, 134, 143, 1, 0, false, false, false, true};
    SpriteRenderState srs2 = {digitSprites[0], -11, 141, 1, 0, false, false, false, true};
    SpriteRenderState srs3 = {digitSprites[1], 15, 141, 1, 0, false, false, false, true};
    SpriteRenderState srs4 = {digitSprites[0], 54, 141, 1, 0, false, false, false, true};
    SpriteRenderState srs5 = {digitSprites[2], 80, 141, 1, 0, false, false, false, true};
    SpriteRenderState srs6 = {timeSprites[0], -27, -5, 1, 0, false, false, false, true};
    SpriteRenderState srs7 = {timeSprites[1], 37, -5, 1, 0, false, false, false, true};
    SpriteRenderState srs8 = {timeSprites[2], 101, -5, 1, 0, false, false, false, true};
    SpriteRenderState srs9 = {timeSprites[3], 165, -5, 1, 0, false, false, false, true};
    SpriteRenderState srs10 = {skillSprite, 90, 77, 1, 0, false, false, false, true};
    SpriteRenderState srs11 = {slashSprite, 52, 157, 1, 0, false, false, false, true};

    etl::array<SpriteRenderState, 12> spriteRenderStates = {
        srs0, srs1, srs2, srs3, srs4, srs5, srs6, srs7, srs8, srs9, srs10, srs11};

    etl::array<SpriteTransform, 0> spriteTransforms = {};
    // ---
};
