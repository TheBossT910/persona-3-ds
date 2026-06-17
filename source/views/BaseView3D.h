#pragma once
#include "core/globals.h"
#include "views/BaseView.h"
#include <nds.h>

class BaseView3D : public BaseView
{
  public:
    virtual void init() override;
    virtual void setupEnvironment() = 0;
    // update needs to be overridden
    // cleanup needs to be overridden

  private:
    // fog properties
    int shift = 1;
    // how thick (translucent) the fog is
    int mass = 1;
    // how far the fog is
    int depth = 0x4000;
};
