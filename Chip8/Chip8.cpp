// Chip8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Technical-Reference
// https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set
// https://github.com/Johnnei/Youtube-Tutorials/tree/master/emulator_chip8
// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM


#include <iostream>
#include "Chip.h"
#include "Screen.h"

int main()
{
	Chip chip;
	Screen screen;

	if (!screen.init()) {
		std::cerr << "Failed to initialize SDL" << std::endl;
		return -1;
	}

	chip.init();

	if (!chip.loadProgram("./Pong_alt.ch8")) {
		std::cerr << "Failed to load ROM" << std::endl;
		return -1;
	}

	// A complete 8x8 smiley face sprite
	uint8_t smiley[8] = {
		0b00111100, //    **** (Top border)
		0b01000010, //  * * (Sides)
		0b10100101, // * * * * (Eyes)
		0b10000001, // * * (Middle)
		0b10100101, // * * * * (Mouth corners)
		0b10011001, // * ** * (Smile bottom)
		0b01000010, //  * * (Bottom curve)
		0b00111100  //    **** (Bottom border)
	};

	// Testing: Draw the smiley at the top-left (0,0)
	for (int y = 0; y < 8; y++) {
		uint8_t row = smiley[y];
		for (int x = 0; x < 8; x++) {
			// Check if the bit at the current X position is set to 1
			// We shift 1 into the correct position and use bitwise AND
			bool pixelState = (row >> (7 - x)) & 1;

			// Convert 2D (x,y) to 1D index: (y * width + x)
			int index = (y * 128) + x;
			screen.setPixel(index, pixelState);
		}
	}

	bool quit = false;
	SDL_Event event;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) quit = true;
		}

		chip.run();
		screen.draw();

		SDL_Delay(1000 / 60); // Refresh at 60 fps
	}

	system("Pause");

	return 0;
}