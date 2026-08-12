#pragma once
#include "components/ui/UIScreen.hpp"
#include "core/enums.hpp"
#include "core/globals.hpp"
#include "core/structs.hpp"
#include <nds.h>

#include "components/GraphicsComponent.hpp"

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
    DialogueScreen() {};
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

    void renderBackground();
};
