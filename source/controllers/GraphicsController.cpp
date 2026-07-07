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

void* GraphicsController::loadToRAM(const std::string& filepath, u32* outSize)
{
    // open file
    FILE* file = fopen(filepath.c_str(), "rb");
    if (!file)
    {
        if (outSize)
        {
            *outSize = 0;
        }
        return NULL;
    }

    // get file size
    fseek(file, 0, SEEK_END);
    u32 size = ftell(file);
    // return to beginning of file
    rewind(file);

    if (size == 0)
    {
        fclose(file);
        if (outSize)
        {
            *outSize = 0;
        }
        return NULL;
    }

    // allocate buffer
    void* buffer = malloc(size);
    if (buffer)
    {
        fread(buffer, 1, size, file);
    }

    fclose(file);

    if (outSize)
    {
        *outSize = size;
    }

    return buffer;
}

void GraphicsController::unloadFromRAM(void* buffer)
{
    // free memory
    if (buffer)
    {
        free(buffer);
    }
}

GraphicAsset GraphicsController::loadGrit(const std::string& basePath)
{
    GraphicAsset asset = {NULL, 0, NULL, 0, NULL, 0};

    asset.tiles = loadToRAM(assetFilePath(basePath, ".img.bin"), &asset.tilesLen);
    asset.pal = loadToRAM(assetFilePath(basePath, ".pal.bin"), &asset.palLen);
    asset.map = loadToRAM(assetFilePath(basePath, ".map.bin"), &asset.mapLen);

    return asset;
}

void GraphicsController::unloadGrit(GraphicAsset& asset)
{
    unloadFromRAM(asset.tiles);
    unloadFromRAM(asset.pal);
    unloadFromRAM(asset.map);

    asset.tiles = NULL;
    asset.tilesLen = 0;
    asset.pal = NULL;
    asset.palLen = 0;
    asset.map = NULL;
    asset.mapLen = 0;
}
