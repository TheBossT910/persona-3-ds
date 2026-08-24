#include "VideoController.hpp"
#include "core/globals.hpp"
#include <malloc.h>
#include <nds.h>
#include <stdio.h>
#include <string.h> // for memcmp

VideoController* VideoController::instance = nullptr;

void VideoController::create()
{
    if (instance == nullptr)
    {
        instance = new VideoController();
    }
}

void VideoController::destroy()
{
    if (instance != nullptr)
    {
        delete instance;
    }
    instance = nullptr;
}

VideoController* VideoController::getInstance()
{
    if (instance == nullptr)
    {
        create();
    }
    return instance;
}

void VideoController::init(std::string iFileName, float iFps, ViewState iNextState)
{
    // FIX: free/close any previous allocation before overwriting the
    // pointers. Previously, calling init() twice without an intervening
    // cleanup() call leaked the memalign'd frame buffer and the file
    // handle every time.
    if (ramBuffer != nullptr)
    {
        free(ramBuffer);
        ramBuffer = nullptr;
    }
    if (videoFile != nullptr)
    {
        fclose(videoFile);
        videoFile = nullptr;
    }

    fileEOF = false;
    framesAvailable = 0;
    readIndex = 0;
    writeIndex = 0;
    currentFrame = 0;

    nextState = iNextState;
    fps = iFps; // default fallback if no header exists
    fileEOF = false;

    std::string videoPath = fatBasePath + "video/" + iFileName;

    readIndex = 0;
    writeIndex = 0;
    framesAvailable = 0;
    currentFrame = 0;

    videoSetMode(MODE_5_2D | DISPLAY_BG3_ACTIVE);
    videoSetModeSub(MODE_0_2D);

    vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
    vramSetBankD(VRAM_D_MAIN_BG_0x06020000);
    vramSetBankC(VRAM_C_SUB_BG);

    musicCtrl->initVideoAudio();

    videoFile = fopen(videoPath.c_str(), "rb");
    if (!videoFile)
    {
        consoleDemoInit();
        printf("ERR: %s", videoPath.c_str());
        while (1)
            swiWaitForVBlank();
    }

    // dynamic header reading
    u8 header[16];
    size_t hRead = fread(header, 1, 16, videoFile);

    // validate if the new magic "VID\0" Header is present
    if (hRead == 16 && memcmp(header, "VID\0", 4) == 0)
    {
        // bit-shifts safeguard against unaligned memory access crashes on the ARM9
        fps = (float)(header[4] | (header[5] << 8));
        bpp = header[6];
        frameW = header[8] | (header[9] << 8);
        frameH = header[10] | (header[11] << 8);
    }
    else
    {
        // fallback for older files without header (Assuming 16-bit, 256x192)
        fseek(videoFile, 0, SEEK_SET);
        bpp = 2;
        frameW = 256;
        frameH = 192;
    }

    frameSize = frameW * frameH * bpp;
    bufferSize = frameSize * FRAMES_TO_BUFFER;

    // configure DS backgrounds dynamically depending on parsed BPP
    if (bpp == 1)
    {
        bg = bgInit(3, BgType_Bmp8, BgSize_B8_256x256, 0, 0);

        u16 palette[256];
        size_t palRead = fread(palette, 2, 256, videoFile);
        if (palRead != 256)
        {
            consoleDemoInit();
            printf("ERR: palette read failed");
            while (1)
                swiWaitForVBlank();
        }

        for (int i = 0; i < 256; i++)
        {
            BG_PALETTE[i] = palette[i];
        }
    }
    else
    {
        bgExtPaletteEnable();
        bg = bgInit(3, BgType_Bmp16, BgSize_B16_256x256, 0, 0);
    }

    // clear memory
    dmaFillWords(0, bgGetGfxPtr(bg), frameSize);

    ramBuffer = (u8*)memalign(32, bufferSize);
    if (!ramBuffer)
    {
        consoleDemoInit();
        printf("ERR: malloc failed");
        while (1)
            swiWaitForVBlank();
    }

    refillBuffer();
    refillBuffer();
}

void VideoController::refillBuffer()
{
    if (fileEOF || framesAvailable >= FRAMES_TO_BUFFER)
        return;

    u32 audioSize = 0;

    // read audio chunk size
    if (fread(&audioSize, 4, 1, videoFile) != 1)
    {
        fileEOF = true;
        return;
    }

    // clamp audio buffer to prevent potential stack buffer overflow
    if (audioSize > 0)
    {
        u32 safeSize = (audioSize > sizeof(audioBuf)) ? sizeof(audioBuf) : audioSize;

        // FIX: keep the pushed chunk aligned to whole stereo sample frames
        // (4 bytes: 2ch * 16-bit). If a truncated chunk ever lands on a
        // non-4-byte boundary, every sample pushed afterward permanently
        // shifts relative to true L/R boundaries - the channels desync
        // and it plays as persistent static/crunch that never
        // self-corrects. This keeps the ring buffer's contents always a
        // whole number of frames.
        safeSize -= (safeSize % BYTES_PER_FRAME);

        if (safeSize > 0)
        {
            fread(audioBuf, 1, safeSize, videoFile);
            musicCtrl->pushVideoAudio(audioBuf, safeSize);
        }

        u32 consumed = safeSize;
        if (audioSize > consumed)
            fseek(videoFile, audioSize - consumed, SEEK_CUR); // Skip overflowing/unaligned remainder
    }

    // read video frame utilizing dynamic frameSize
    u8* dest = &ramBuffer[writeIndex * frameSize];
    size_t bytes = fread(dest, 1, frameSize, videoFile);

    if (bytes == frameSize)
    {
        DC_FlushRange(dest, frameSize);
        writeIndex = (writeIndex + 1) % FRAMES_TO_BUFFER;
        framesAvailable++;
    }
    else
    {
        fileEOF = true;
    }
}

ViewState VideoController::update()
{
    musicCtrl->update();

    // FIX: interleave a stream update between each disk read, not just
    // once at the top. refillBuffer() can read up to ~frameSize bytes
    // (e.g. ~98KB for a 256x192 16bpp frame) per call, up to
    // READS_PER_UPDATE times. On a flashcart, that read latency can be
    // long enough that the DS sound hardware runs past what our
    // callback has filled and starts playing stale/uninitialized RAM -
    // which is what static/crunch actually is at the hardware level.
    // Calling update() here is cheap for the video-audio path (a
    // ring-buffer memcpy, no disk I/O), so doing it 2-3x extra per
    // frame costs effectively nothing.
    for (int r = 0; r < READS_PER_UPDATE; r++)
    {
        refillBuffer();
        musicCtrl->update();
    }

    int expectedFrame = (int)(musicCtrl->getVideoTime() * fps);

    if (currentFrame > expectedFrame && !fileEOF)
    {
        swiWaitForVBlank();
        return ViewState::KEEP_CURRENT;
    }

    int dropBudget = 3;
    while (currentFrame < expectedFrame - 1 && framesAvailable > 0 && dropBudget-- > 0)
    {
        readIndex = (readIndex + 1) % FRAMES_TO_BUFFER;
        framesAvailable--;
        currentFrame++;
    }

    if (fileEOF && framesAvailable == 0)
    {
        return nextState;
    }

    if (framesAvailable > 0)
    {
        swiWaitForVBlank();
        dmaCopy(&ramBuffer[readIndex * frameSize], bgGetGfxPtr(bg), frameSize);
        readIndex = (readIndex + 1) % FRAMES_TO_BUFFER;
        framesAvailable--;
        currentFrame++;
    }

    return ViewState::KEEP_CURRENT;
}

void VideoController::cleanup()
{
    if (musicCtrl != nullptr)
    {
        musicCtrl->cleanup();
    }

    if (ramBuffer != nullptr)
    {
        dmaFillWords(0, bgGetGfxPtr(bg), frameSize);
        free(ramBuffer);
        ramBuffer = nullptr;
    }

    if (bg >= 0)
    {
        bg = -1;
    }

    if (videoFile != nullptr)
    {
        fclose(videoFile);
        videoFile = nullptr;
    }

    fileEOF = false;
    framesAvailable = 0;
    readIndex = 0;
    writeIndex = 0;
    currentFrame = 0;
}
