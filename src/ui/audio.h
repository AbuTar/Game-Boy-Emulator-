#ifndef AUDIO_H
#define AUDIO_H
#include <SDL3/SDL.h>
#include "apu.h"

// Thin SDL3 audio backend. This is the ONLY audio file that talks to SDL -
// the APU core itself (apu.c) is platform-independent. When porting to the
// ESP32, this is the file to replace with an I2S backend; apu.c/apu.h can
// be reused as-is.
typedef struct {
    SDL_AudioStream* stream;
    SDL_AudioDeviceID device;
} Audio;

bool audio_init(Audio* audio);
void audio_pump(Audio* audio, APU* apu); // call once per frame: drains APU ring buffer into SDL
void audio_cleanup(Audio* audio);

#endif
