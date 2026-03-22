#include "Screen.h"

bool Screen::init() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return false;
	}

	window = SDL_CreateWindow("CHIP-8 Emulator", 1024, 512, 0);
	renderer = SDL_CreateRenderer(window, NULL);

	texture = SDL_CreateTexture(renderer,
		SDL_PIXELFORMAT_RGBA8888,
		SDL_TEXTUREACCESS_STREAMING,
		width, height);

	if (!window || !renderer || !texture) return false;

	SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST);

	return true;
}

void Screen::clear() {

}

void Screen::setPixel(uint32_t index, bool state) {
	uint32_t x = index % width;
	uint32_t y = index / width;

	if (index < width * height) {
		pixels[index] = state ? 0xFFFFFFFF : 0x000000FF;
	}
}

void Screen::draw() {
	// Upload the pixel array to the GPU and draw
	SDL_UpdateTexture(texture, NULL, pixels, 128 * sizeof(uint32_t));
	SDL_RenderClear(renderer);
	SDL_RenderTexture(renderer, texture, NULL, NULL);
	//SDL_RenderCopy(renderer, texture, NULL, NULL); // SDL scales it to window size automatically
	SDL_RenderPresent(renderer);
}