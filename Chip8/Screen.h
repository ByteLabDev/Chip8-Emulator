#pragma once
#include <cstdint>
#include <SDL3/SDL.h>

class Screen
{
	public:
		static const uint8_t width = 128;
		static const uint8_t height = 64;
		bool init();
		bool setPixel(uint32_t x, uint32_t y);
		void clear();
		void draw();
	private:
		uint32_t pixels[width * height];

		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;

		SDL_Texture* texture = nullptr;
};