#pragma once

#include "controllers/MusicController.hpp"
#include "types/StateTypes.hpp"

#include <nds.h>
#include <string>

#define FRAMES_TO_BUFFER 15
#define READS_PER_UPDATE 3

/**
 * @brief Streams decoded video frames and synchronized audio from a video file.
 *
 * Frames are buffered in RAM and copied to a DS background as playback advances.
 * The controller owns its file handle and frame buffer and is a singleton.
 */
class VideoController
{
  public:
    /** @brief Creates the singleton instance if it does not exist. */
    static void create();
    /** @brief Destroys the singleton instance and releases video resources. */
    static void destroy();
    /** @return The process-wide video controller instance. */
    static VideoController* getInstance();

    /**
     * @brief Opens and prepares a video stream.
     * @param iFileName Video filename relative to the data video directory.
     * @param iFps Fallback frame rate when the file has no header.
     * @param iNextState View state returned after playback completes.
     */
    void init(std::string iFileName, float iFps, ViewState iNextState);
    /** @brief Advances playback and renders the next available frame. */
    ViewState update();
    /** @brief Stops playback and releases the file and frame buffer. */
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

    /** @brief Reads audio and one video frame into the playback buffers. */
    void refillBuffer();

    MusicController* musicCtrl = MusicController::getInstance();
};
