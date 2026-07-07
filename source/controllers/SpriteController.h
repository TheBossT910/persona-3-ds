#pragma once
#include "controllers/GraphicsController.h"
#include "core/enums.h"
#include "core/structs.h"
#include <nds.h>
#include <vector>

class SpriteController
{
  public:
    static void create();
    static void destroy();
    static SpriteController* getInstance();

    std::string spritePath;
    template <typename SpriteID> bool switchSprite(SpriteType type, SpriteID spriteId, SpriteRegister* out)
    {
        return switchSpriteImpl(type, static_cast<int>(spriteId), out);
    }

    void unloadAll();

  private:
    SpriteController() {};
    ~SpriteController() {};
    static SpriteController* instance;

    std::vector<GraphicAsset> loadedAssets;
    bool switchSpriteImpl(SpriteType type, int spriteId, SpriteRegister* out);
    GraphicsController* graphicsCtrl = GraphicsController::getInstance();
};
