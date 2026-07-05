#pragma once
#include "controllers/MusicController.h"
#include "core/enums.h"
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
    VideoController() {};
    ~VideoController() {};
    static VideoController* instance;

    ViewState nextState;
    float fps;

    FILE* videoFile;
    bool fileEOF;
    int currentFrame;
    int bg;

    u8* ramBuffer;
    int readIndex;
    int writeIndex;
    int framesAvailable;

    // dynamic video variables
    u16 frameW;
    u16 frameH;
    u8 bpp;
    u32 frameSize;
    u32 bufferSize;

    u8 audioBuf[16384];

    void refillBuffer();

    MusicController* musicCtrl = MusicController::getInstance();
};
