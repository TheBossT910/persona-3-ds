#pragma once
#include "core/globals.h"
#include "views/BaseView.h"
#include <nds.h>

class DisclaimerView : public BaseView
{
  private:
    int bg[2];

    GraphicsController* graphicsCtrl = GraphicsController::getInstance();

  public:
    // override tells compiler we intend to override a virtual fn in a base class (i.e. View)
    void init() override;
    ViewState update() override;
    void cleanup() override;
};
