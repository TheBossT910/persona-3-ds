#pragma once
#include "core/structs.hpp"
#include <nds.h>
#include <vector>

class AnimationController
{
  public:
    static void create();
    static void destroy();
    static AnimationController* getInstance();

    // Load the MDL2 (or legacy MDL1) binary produced by obj2model.py.
    bool loadModel(const char* filepath);

    // Upload textures to VRAM after loadModel().
    // Call the generated <modelname>_loadTextures() helper from your .h file
    // rather than calling this directly — it supplies the correct sizes/formats.
    //
    //   count   : number of texture slots (== MODEL_XXX_TEX_COUNT)
    //   bitmaps : array of pointers to grit-generated bitmap data (one per slot)
    //   widths  : NDS-valid pixel widths  (power-of-two, 8–1024)
    //   heights : NDS-valid pixel heights (power-of-two, 8–1024)
    //   isRGBA  : true → GL_RGBA (16-bit), false → GL_RGB
    bool loadTextures(
        int count, const unsigned int** bitmaps, const int* widths, const int* heights, const bool* isRGBA);

    // Animation control
    void set(int animIndex, bool loop = true);
    void play();
    void stop();
    void pause();

    // Adjust animation speed
    void setAnimationSpeed(float speedMultiplier = 1.0f)
    {
        animSpeedFP = (int)(speedMultiplier * 128.0f);
    }

    // Call once per frame (advances the frame counter)
    void update();

    // Call once per frame (renders all nodes into the NDS geometry engine)
    void render();

    bool isAnimationPlaying() const
    {
        return isPlaying;
    }
    int getCurrentAnimIndex() const
    {
        return currentAnimIndex;
    }

  private:
    AnimationController();
    ~AnimationController();
    static AnimationController* instance;

    void renderNode(int nodeId);
    Keyframe getInterpolatedFrame(const AnimTrack& track, int timeFP, int nodeId);
    static int textureSizeToEnum(int px);

    std::vector<AnimNode> modelNodes;
    std::vector<Animation> animations;
    std::vector<int> trackIndices; // per-node cached search position
    std::vector<int> rootNodes;

    // One GL texture ID per texture slot.  Empty if loadTextures() not called.
    std::vector<int> textureIDs;

    int currentAnimIndex = -1;
    int currentFrameFP = 0;
    int animSpeedFP = 128; // 128 = 0.5 frames per tick (converts 30fps to 60fps)
    bool isPlaying = false;
    bool isFinishing = false;
    bool isLooping = true;
};
