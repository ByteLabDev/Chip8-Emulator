#pragma once
#include <cstdint>
#include <SDL3/SDL.h>
#include <iostream>

class Screen
{
	public:
		static const uint8_t width = 128;
		static const uint8_t height = 64;
		static const uint16_t windowWidth = 1024;
		static const uint16_t windowHeight = 512;
		bool init();
		bool setPixel(uint32_t x, uint32_t y);
		void clear();
		void draw();

	private:
		uint32_t pixels[width * height];

		SDL_FRect outRect;

		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;

		SDL_Texture* texture = nullptr;

		uint32_t onColor = 0x58A35AFF;
		uint32_t offColor = 0x101410FF;
};