// Chip8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// Main reference:
// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
// Other references
// https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Technical-Reference
// https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set
// https://github.com/Johnnei/Youtube-Tutorials/tree/master/emulator_chip8

#include <iostream>
#include "Chip.h"
#include "Screen.h"
#include "Keypad.h"
#include "Sound.h"

int main()
{
	Chip chip;
	Screen screen;
	Keypad keypad;
	Sound sound;

	if (!screen.init()) {
		std::cerr << "Failed to initialize SDL" << std::endl;
		return -1;
	}

	if (!sound.init()) {
		std::cerr << "Failed to initialize sound" << std::endl;
		return -1;
	}

	chip.init(screen, keypad, sound);

	if (!chip.loadProgram("./Pong_alt.ch8")) {
		std::cerr << "Failed to load ROM" << std::endl;
		return -1;
	}

	screen.clear();

	bool quit = false;
	SDL_Event event;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) quit = true;
		}

		keypad.read();
		chip.runTimers(1);
		chip.run();
		screen.draw();

		SDL_Delay(1); // Refresh at 1000hz
	}
	return 0;
}