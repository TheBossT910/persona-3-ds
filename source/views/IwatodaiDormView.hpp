#pragma once

#include "views/EnvironmentView.hpp"

// data
#include "data/environmentDb.hpp"
// maps
#include "maps/iwatodai_dorm_floor_1.hpp"
// dialogue
#include "dialogue/demo_dialogue.hpp"

class IwatodaiDormView : public EnvironmentView
{
  public:
    // TODO: dont forget to clear in future
    IwatodaiDormView();

  protected:
    const EnvironmentDbEntry* getEnvironmentDbEntry() override
    {
        return g_environmentDb[0];
    }
    void setMovementConfig() override;
    void setMusic() override;
    ViewState onTileCheck(TileType tile, u32 pressed) override;
    void setDialogueConfig() override;
    void setCameraConfig() override;
    void setTextConfig() override;
    void setupUI() override;

  private:
    // movement and camera
    const Point2D<ae::q20_12_t> characterSize{ae::q20_12_t{0.1}, ae::q20_12_t{0.1}};
    const ae::q20_12_t speed{0.03};

    // character position
    const Point2D<ae::q20_12_t> characterTranslate{ae::q20_12_t{0.4}, ae::q20_12_t{2.8}};
    const ae::q20_12_t height{0};
    const ae::q20_12_t characterFacingAngle{180};
};
