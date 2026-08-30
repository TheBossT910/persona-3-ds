#pragma once
#include <aegis/types.hpp>
#include <maxmod9.h>
#include <nds.h>
#include <stdio.h>

#define AUDIO_SAMPLE_RATE 32000
#define AUDIO_CHANNELS 2
#define BYTES_PER_SAMPLE 2
#define BYTES_PER_FRAME (AUDIO_CHANNELS * BYTES_PER_SAMPLE)

class MusicController
{
  public:
    static void create();
    static void destroy();
    static MusicController* getInstance();

    // audio
    void init(const char* filePath,
              ae::q20_12_t loopStartSeconds = ae::q20_12_t{0},
              ae::q20_12_t loopEndSeconds = ae::q20_12_t{-1.0});
    void update();
    void pause();
    void resume();

    // audio for video streams
    void initVideoAudio();
    void pushVideoAudio(const u8* data, size_t size);
    ae::q20_12_t getVideoTime();

    // sfx
    void loadSFX(mm_word effectID);
    mm_sfxhand playSFX(mm_word effectID, int volume = 255, int panning = 128);
    void stopSFX(mm_sfxhand handle);

    void cleanup();

  private:
    MusicController() {};
    ~MusicController() {};
    static MusicController* instance;

    long getAudioStartOffset(FILE* file);
};
