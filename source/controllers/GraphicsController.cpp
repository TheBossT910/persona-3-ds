#include "GraphicsController.h"
#include <malloc.h>
#include <nds.h>

GraphicsController* GraphicsController::instance = nullptr;

void GraphicsController::create()
{
    if (instance == nullptr)
    {
        instance = new GraphicsController();
    }
}

void GraphicsController::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }
    instance = nullptr;
}

GraphicsController* GraphicsController::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }
    return instance;
}

static std::string assetFilePath(const std::string& basePath, const char* suffix)
{
    std::string directPath = basePath + suffix;

    FILE* file = fopen(directPath.c_str(), "rb");
    if (file)
    {
        fclose(file);
        return directPath;
    }

    size_t end = basePath.find_last_not_of('/');
    if (end == std::string::npos)
    {
        return directPath;
    }

    size_t slash = basePath.find_last_of('/', end);
    std::string leaf = basePath.substr(slash == std::string::npos ? 0 : slash + 1,
                                       end - (slash == std::string::npos ? 0 : slash + 1) + 1);
    return basePath.substr(0, end + 1) + "/" + leaf + suffix;
}

GraphicAsset GraphicsController::loadGrit(const std::string& basePath)
{
    GraphicAsset asset = {NULL, 0, NULL, 0, NULL, 0};

    asset.tiles = io.loadToRAM(assetFilePath(basePath, ".img.bin"), &asset.tilesLen);
    asset.pal = io.loadToRAM(assetFilePath(basePath, ".pal.bin"), &asset.palLen);
    asset.map = io.loadToRAM(assetFilePath(basePath, ".map.bin"), &asset.mapLen);

    return asset;
}

void GraphicsController::unloadGrit(GraphicAsset& asset)
{
    io.unloadFromRAM(asset.tiles);
    io.unloadFromRAM(asset.pal);
    io.unloadFromRAM(asset.map);

    asset.tiles = NULL;
    asset.tilesLen = 0;
    asset.pal = NULL;
    asset.palLen = 0;
    asset.map = NULL;
    asset.mapLen = 0;
}
