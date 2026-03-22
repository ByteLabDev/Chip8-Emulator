#pragma once
#include <cstdint>
#include <SDL3/SDL.h>

class Screen
{
	private:
		static const uint8_t width = 128;
		static const uint8_t height = 64;
		uint32_t pixels[width * height];

		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;

		SDL_Texture* texture = nullptr;

	public:
		bool init();
		void setPixel(uint32_t index, bool state);
		void clear();
		void draw();
};