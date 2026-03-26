// Chip8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
// Main reference:
// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM
// Additional references
// https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Technical-Reference
// https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set
// https://github.com/Johnnei/Youtube-Tutorials/tree/master/emulator_chip8

#include <iostream>
#include "Chip.h"
#include "Screen.h"
#include "Keypad.h"
#include "Sound.h"
#include "Menu.h"

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

	screen.clear();
	screen.updateTexture();
	screen.draw();

	bool quit = false;
	SDL_Event event;
	while (!quit) {
		while (SDL_PollEvent(&event)) {
			ImGui_ImplSDL3_ProcessEvent(&event);
			if (event.type == SDL_EVENT_QUIT) quit = true;
		}

		keypad.read();
		chip.runTimers(1);
		chip.run();

		if(chip.drawFlag){
			screen.updateTexture();
		}

		Menu::ScreenAction action = screen.draw();

		switch (action) {
			case Menu::ScreenAction::OpenROM: {
				chip.reset();
				screen.updateTexture();
				screen.draw();
				std::string filePath = Menu::openFileDialog();
				if (!chip.loadProgram(filePath)) {
					std::cerr << "Failed to load ROM at " << filePath.c_str() << std::endl;
				}
				break;
			}
			case Menu::ScreenAction::Quit: {
				quit = true;
				break;
			}
			case Menu::ScreenAction::Reset: {
				std::string filePath = chip.romPath;
				chip.reset();
				screen.updateTexture();
				screen.draw();
				chip.loadProgram(filePath);
				break;
			}
		}

		SDL_Delay(2); // Refresh at 500hz
	}
	return 0;
}