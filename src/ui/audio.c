#include "audio.h"
#include <stdio.h>

bool audio_init(Audio* audio) {
    if (!SDL_InitSubSystem(SDL_INIT_AUDIO)) {
        printf("SDL_InitSubSystem(AUDIO) failed: %s\n", SDL_GetError());
        return false;
    }

    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_S16LE;
    spec.channels = 2;
    spec.freq = APU_SAMPLE_RATE;

    // NULL callback: we push samples manually each frame via audio_pump,
    // pulling from the APU's own ring buffer (apu_read_samples).
    audio->stream = SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec, NULL, NULL);
    if (!audio->stream) {
        printf("SDL_OpenAudioDeviceStream failed: %s\n", SDL_GetError());
        return false;
    }

    audio->device = SDL_GetAudioStreamDevice(audio->stream);

    // Devices opened via SDL_OpenAudioDeviceStream start paused.
    if (!SDL_ResumeAudioStreamDevice(audio->stream)) {
        printf("SDL_ResumeAudioStreamDevice failed: %s\n", SDL_GetError());
        return false;
    }

    printf("SDL3 audio initialized: %d Hz, stereo, 16-bit\n", APU_SAMPLE_RATE);
    return true;
}

void audio_pump(Audio* audio, APU* apu) {
    if (!audio->stream) return;

    // Cap how much we drain in one go so a slow frame doesn't try to dump
    // an enormous backlog into SDL in one call.
    s16 buffer[2048 * 2];
    u32 available = apu_available_samples(apu);

    while (available > 0) {
        u32 chunk = available > 2048 ? 2048 : available;
        u32 copied = apu_read_samples(apu, buffer, chunk);
        if (copied == 0) break;

        SDL_PutAudioStreamData(audio->stream, buffer, (int)(copied * 2 * sizeof(s16)));
        available -= copied;
    }
}

void audio_cleanup(Audio* audio) {
    if (audio->stream) {
        SDL_DestroyAudioStream(audio->stream);
        audio->stream = NULL;
    }
    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    printf("SDL3 audio cleaned up\n");
}
