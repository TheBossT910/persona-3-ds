#pragma once
#include "controllers/MusicController.hpp"
#include "core/enums.hpp"
#include <nds.h>
#include <string>

#define FRAMES_TO_BUFFER 15
#define READS_PER_UPDATE 3

class VideoController
{
  public:
    static void create();
    static void destroy();
    static VideoController* getInstance();

    void init(std::string iFileName, float iFps, ViewState iNextState);
    ViewState update();
    void cleanup();

  private:
    VideoController() = default;
    ~VideoController()
    {
        cleanup();
    }
    static VideoController* instance;

    ViewState nextState = ViewState::DEFAULT;
    float fps = 0.0f;

    FILE* videoFile = nullptr;
    bool fileEOF = false;
    int currentFrame = 0;
    int bg = -1;

    u8* ramBuffer = nullptr;
    int readIndex = 0;
    int writeIndex = 0;
    int framesAvailable = 0;

    // dynamic video variables
    u16 frameW = 0;
    u16 frameH = 0;
    u8 bpp = 0;
    u32 frameSize = 0;
    u32 bufferSize = 0;

    u8 audioBuf[16384];

    void refillBuffer();

    MusicController* musicCtrl = MusicController::getInstance();
};
