#pragma once

#include "components/GraphicsComponent.hpp"
#include "components/ui/UIScreen.hpp"
#include "core/globals.hpp"

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
    void removeSprites() override;

  private:
    DialogueScreen() : UIScreen(false) {};
    ~DialogueScreen() {};
    static DialogueScreen* instance;

    // sprites
    // TODO: reduce allocated sprite/sprite registers
    Sprite sprites[50];
    GraphicAsset calendarSprite[2];
    GraphicAsset textBox[10];
    GraphicAsset nameTag[10];

    ae::Entity* dialogue = nullptr;
    GraphicsComponent* graphics = nullptr;

    void loadBackground();
};
