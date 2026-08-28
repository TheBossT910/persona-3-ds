#pragma once
#include "components/GraphicsComponent.hpp"
#include "components/ui/UIScreen.hpp"
#include "core/globals.hpp"
#include <etl/array.h>
#include <nds.h>

class DialogueScreen : public UIScreen
{
  public:
    static void create();
    static void destroy();
    static DialogueScreen* getInstance();

    void load();
    void unload();
    void renderSprites() override;

  private:
    DialogueScreen() : UIScreen(false) {};
    ~DialogueScreen() {};
    static DialogueScreen* instance;

    // sprites
    // TODO: reduce allocated sprite/sprite registers
    etl::array<Sprite, 50> sprites = {};
    etl::array<GraphicAsset, 2> calendarSprite = {};
    etl::array<GraphicAsset, 10> textBox = {};
    etl::array<GraphicAsset, 10> nameTag = {};

    ae::Entity* dialogue = nullptr;
    GraphicsComponent* graphics = nullptr;

    void loadBackground();
};
