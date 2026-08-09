#include "IOManager.hpp"

extern std::string fatBasePath;
void IOManager::Init()
{
    basePath = fatBasePath;
}

// TODO: update to not use malloc/free
void* IOManager::loadToRAM(const std::string& filePath, u32* outSize)
{
    // open file
    // TODO: remove codebase-wide use of fatBasePath so it can automatically be appended in IOManager
    // std::string path = basePath + filePath;
    FILE* file = fopen(filePath.c_str(), "rb");
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

// TODO: update to not use malloc/free
void IOManager::unloadFromRAM(void* buffer)
{
    // free memory
    if (buffer)
    {
        free(buffer);
    }
}
