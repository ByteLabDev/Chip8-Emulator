#pragma once
#include <cstdint>
#include <SDL3/SDL.h>
#include <iostream>
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_opengl3.h"
#include "imgui_impl_sdlrenderer3.h"
#include "Menu.h"

class Chip;

class Screen
{
	public:
		static const uint8_t width = 128;
		static const uint8_t height = 64;
		static const uint16_t windowWidth = 1024;
		static const uint16_t windowHeight = 512;
		bool init(Chip& chipPtr);
		bool setPixel(uint32_t x, uint32_t y);
		void clear();
		Menu::ScreenAction draw();
		void updateTexture();
		void setExtendedMode(bool mode);
		void scrollLeft();
		void scrollRight();
		void scrollDown(int n);

	private:
		Chip* chip;
		uint32_t pixels[width * height]; // Rendering
		bool logicalPixels[width * height] = {}; // Game logic & collisions

		SDL_Window* window = nullptr;
		SDL_Renderer* renderer = nullptr;

		SDL_Texture* texture = nullptr;

		uint32_t onColor = 0x58A35AFF;
		uint32_t offColor = 0x101410FF;

		bool hiRes = false;

		bool showSettings = false;
};