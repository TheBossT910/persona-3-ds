#pragma once
#include "views/EnvironmentView.hpp"

class IwatodaiDormView : public EnvironmentView
{
  public:
    IwatodaiDormView();

  protected:
    const EnvironmentDbEntry* getEnvironmentDbEntry() override
    {
        return g_environmentDb[0];
    }
    void setupMovement() override;
    void setupMusic() override;
    ViewState onTileCheck(TileType tile, u32 pressed) override;
    void setupDialogue() override;
    void setupCamera() override;
    void setupText() override;
    void setupUI() override;

  private:
    // movement and camera
    const Point2D<float> characterSize = Point2D<float>(0.1f, 0.1f);
    const float speed = 0.03f;

    // character position
    const Point2D<float> characterTranslate = Point2D<float>(0.4f, 2.8f);
    const float height = 0.0f;
    const float characterFacingAngle = 180.0f;

    std::string fontNameAlt = "noto-sans-jp-black";
    int fontSizeAlt = 16;
};
