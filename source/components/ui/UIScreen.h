#pragma once
#include "core/structs.h"
#include <nds.h>
#include <vector>

class UIScreen
{
  public:
    // TODO: ensure bgId, oam is set before calling load()
    int bgId = -1;
    OamState* oam = nullptr;
    bool isLoaded = false;

    // load + draw?
    virtual void load() = 0;
    // cleanup?
    virtual void unload() = 0;
    // optional
    virtual void renderSprites(); // TODO: make protected
    virtual void removeSprites(); // TODO: make protected
    virtual int onTouch(touchPosition* touch);
    virtual ~UIScreen() = default;

  protected:
    // void loadSprite();
    // void unloadSprite();
    void moveSprite(int spriteId, int x, int y);
    void showSprite(int spriteId);
    void hideSprite(int spriteId);
};
