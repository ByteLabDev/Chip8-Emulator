#pragma once
#include <SDL3/SDL.h>
#include <cstdint>
#include <iostream>
#include <stdio.h>

class Sound
{
	public:
		bool init();
		void update(int soundTimer);
		~Sound();
	private:
		SDL_AudioDeviceID dev;
		SDL_AudioStream* stream;
		void generateSquareWave();
		int16_t beepBuffer[4410];
};

