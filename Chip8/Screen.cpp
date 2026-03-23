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
	for (int i = 0; i < width * height; i++) {
		pixels[i] = 0x000000FF;
	}
}

bool Screen::setPixel(uint32_t x, uint32_t y) {
    // 1. Handle wrapping (standard CHIP-8 behavior)
    x %= width;
    y %= height;
    uint32_t index = x + (y * width);

    // 2. Check if the current pixel is "on" (White)
    // Assuming 0xFFFFFFFF is White and 0x000000FF is Black
    bool currentPixelOn = (pixels[index] == 0xFFFFFFFF);

    // 3. XOR logic: If the pixel was on, it now turns off (and vice-versa)
    if (currentPixelOn) {
        pixels[index] = 0x000000FF; // Turn Off
        return true; // Collision occurred (pixel flipped from on to off)
    } else {
        pixels[index] = 0xFFFFFFFF; // Turn On
        return false; // No collision
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