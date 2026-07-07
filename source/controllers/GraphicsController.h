#pragma once
#include "core/structs.h"
#include <nds.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

class GraphicsController
{
  public:
    static void create();
    static void destroy();
    static GraphicsController* getInstance();

    void* loadToRAM(const std::string& filepath, u32* outSize = NULL);
    void unloadFromRAM(void* buffer);

    GraphicAsset loadGrit(const std::string& basePath);
    void unloadGrit(GraphicAsset& asset);

  private:
    GraphicsController() {};
    ~GraphicsController() {};
    static GraphicsController* instance;
};
