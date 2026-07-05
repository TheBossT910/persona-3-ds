#include "SpriteController.h"
#include "core/globals.h"
#include "data/spriteDb.h"

SpriteController* SpriteController::instance = nullptr;

void SpriteController::create()
{
    if (instance == nullptr)
    {
        instance = new SpriteController();
    }
}

void SpriteController::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}

SpriteController* SpriteController::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }
    return instance;
}

bool SpriteController::switchSpriteImpl(SpriteType type, int spriteId, SpriteRegister* out)
{
    std::string filename = getSpriteFilename(type, spriteId);
    if (filename.empty())
    {
        return false;
    }

    // TODO: pass path
    GraphicAsset asset = graphicsCtrl->loadGrit(fatBasePath + spritePath + filename);
    loadedAssets.push_back(asset);

    out->id = spriteId;
    out->tiles = asset.tiles;
    out->tilesLen = asset.tilesLen;
    out->pal = asset.pal;
    out->palLen = asset.palLen;

    return true;
}

void SpriteController::unloadAll()
{
    for (size_t i = 0; i < loadedAssets.size(); ++i)
    {
        graphicsCtrl->unloadGrit(loadedAssets[i]);
    }
    loadedAssets.clear();
}
