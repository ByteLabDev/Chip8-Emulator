#include "Sound.h"

bool Sound::init() {
    // 1. Define the format (SDL3 Specs are simpler)
    SDL_AudioSpec spec;
    spec.format = SDL_AUDIO_S16;
    spec.channels = 1;
    spec.freq = 44100;

    if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL could not initialize audio! SDL_Error: %s\n", SDL_GetError());
        return false;
    }

    // 2. Open the default playback device
    dev = SDL_OpenAudioDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &spec);
    if (dev == 0) return false;

    // 3. Create a stream
    stream = SDL_CreateAudioStream(&spec, &spec);
    SDL_BindAudioStream(dev, stream);

    // 4. Pre-fill the stream with a square wave "beep"
    // We'll push about 0.1 seconds of audio and tell SDL to loop it
    int numSamples = 44100 / 10;
    int toneHz = 440;
    int period = 44100 / toneHz;

    for (int i = 0; i < numSamples; i++) {
        beepBuffer[i] = ((i / (period / 2)) % 2 == 0) ? 3000 : -3000;
    }

    SDL_PutAudioStreamData(stream, beepBuffer, sizeof(beepBuffer));

    // Pause the stream initially
    SDL_PauseAudioStreamDevice(stream);

    return true;
}

void Sound::update(int soundTimer) {
    if (soundTimer > 0) {
        SDL_ResumeAudioStreamDevice(stream);

        int chunkSizeBytes = sizeof(beepBuffer);

        if (SDL_GetAudioStreamAvailable(stream) < chunkSizeBytes * 3) {
            // Push another 0.1s of the wave
            SDL_PutAudioStreamData(stream, beepBuffer, sizeof(beepBuffer));
        }
        SDL_ResumeAudioStreamDevice(stream);
    }
    else {
        //SDL_ClearAudioStream(stream);
        SDL_PauseAudioStreamDevice(stream);
    }
}

Sound::~Sound() {
    SDL_DestroyAudioStream(stream);
    SDL_CloseAudioDevice(dev);
}
