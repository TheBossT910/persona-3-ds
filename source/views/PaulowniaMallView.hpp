#pragma once

#include "views/EnvironmentView.hpp"

// data
#include "data/environmentDb.hpp"
// maps
#include "maps/paulownia_mall.hpp"

class PaulowniaMallView : public EnvironmentView
{
  public:
    PaulowniaMallView();

  protected:
    const EnvironmentDbEntry* getEnvironmentDbEntry() override
    {
        return g_environmentDb[2];
    }
    float getCameraYOffset() const override
    {
        return 0.3f;
    }
    void setMovementConfig() override;
    void setMusic() override;
    ViewState onTileCheck(TileType tile, u32 pressed) override;
    void setTextConfig() override;
    void setupUI() override;
    void setCameraConfig() override;

  private:
    // movement and camera
    const Point2D<ae::q20_12_t> characterSize{ae::q20_12_t{0.1}, ae::q20_12_t{0.1}};
    const ae::q20_12_t speed{0.03};

    // character position
    const Point2D<ae::q20_12_t> characterTranslate{ae::q20_12_t{0.0122}, ae::q20_12_t{2.3355}};
    const ae::q20_12_t height{0.2};
    const ae::q20_12_t characterFacingAngle{180};
};
