#pragma once
#include "core/structs.h"
#include "managers/IOManager.hpp"
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

    GraphicAsset loadGrit(const std::string& basePath);
    void unloadGrit(GraphicAsset& asset);

  private:
    GraphicsController() {};
    ~GraphicsController() {};
    static GraphicsController* instance;
    IOManager& io = IOManager::GetInstance();
};
