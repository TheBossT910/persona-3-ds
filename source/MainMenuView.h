#pragma once
#include "View.h"

// implementing from View
class MainMenuView : public View {
    private:
        int bg[3];
        
        // for silhouette animation
        int silhouetteX = -256;
        int silhouetteY = 192;
        
        // for bottom screen text animation
        int brightness = 0;
        
        // for fogBackground
        bool displayFog = false;
        int fogOpacity = 0;
        // NOTE: we use u16 to allow overflow (and naturally reset values back to 0)
        u16 waveAngle = 0;
        u16 currentRotation = 0;
        int baseSpeed = 20;
        int fluctuation = 50;
    public:
        // override tells compiler we intend to override a virtual fn in a base class (i.e. View)
        void Init() override;
        ViewState Update() override;
        void Cleanup() override;
};