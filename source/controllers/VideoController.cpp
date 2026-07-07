#include "VideoController.h"
#include "core/globals.h"
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
    nextState = iNextState;
    fps = iFps; // default fallback if no header exists
    fileEOF = false;

    std::string videoPath = fatBasePath + "video/" + iFileName;

    readIndex = 0;
    writeIndex = 0;
    framesAvailable = 0;
    currentFrame = 0;
    ramBuffer = nullptr;

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
        iprintf("ERR: %s", videoPath.c_str());
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
            iprintf("ERR: palette read failed");
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
        iprintf("ERR: malloc failed");
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
        fread(audioBuf, 1, safeSize, videoFile);
        musicCtrl->pushVideoAudio(audioBuf, safeSize);
        if (audioSize > safeSize)
            fseek(videoFile, audioSize - safeSize, SEEK_CUR); // Skip overflowing remainder
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

    for (int r = 0; r < READS_PER_UPDATE; r++)
    {
        refillBuffer();
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

    if (videoFile != nullptr)
    {
        fclose(videoFile);
        videoFile = nullptr;
    }
}
