#pragma once
#include <cstring>
#include <cstdint>
#include <iostream>
#include "Screen.h"
#include "Keypad.h"
#include "Sound.h"

class Chip
{
	private:
		static const uint16_t memorySize = 4096;
		uint8_t memory[memorySize]; // 4096 bytes of RAM
		uint8_t V[16]; // 16 8-bit registers from V0 to VF
		uint16_t I; // 16 bit address register (Only uses [rightmost] 12 bits)
		uint16_t programCounter; // (PC) (16 bits) - Stores currently executing address

		uint16_t stack[16]; // The stack is an array of 16 16-bit values, used to store the address that the interpreter shoud return to when finished with a subroutine. 
		// Chip-8 allows for up to 16 levels of nested subroutines.
		uint8_t stackPointer; // (SP) (8 bits) - Points to topmost level of stack
		uint8_t delayTimer;
		uint8_t soundTimer;

		uint16_t dtDeltaTime;
		uint16_t stDeltaTime;

		// The original implementation of the Chip-8 language used a 64x32-pixel monochrome display
		uint8_t display[64 * 32];

		Screen* screen;
		Keypad* keypad;
		Sound* sound;

		bool isKeyDown; // Checked by Fx0A
		uint8_t downKey; // Checked by Fx0A

		// Programs may also refer to a group of sprites representing the hexadecimal digits 0 through F.
		// These sprites are 5 bytes long, or 8x5 pixels. The data should be stored in the interpreter area of Chip-8 memory (0x000 to 0x1FF).
		// Below is a listing of each character's bytes, in binary and hexadecimal:
		uint8_t fontset[80] = {
			0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
			0x20, 0x60, 0x20, 0x20, 0x70, // 1
			0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
			0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
			0x90, 0x90, 0xF0, 0x10, 0x10, // 4
			0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
			0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
			0xF0, 0x10, 0x20, 0x40, 0x40, // 7
			0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
			0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
			0xF0, 0x90, 0xF0, 0x90, 0x90, // A
			0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
			0xF0, 0x80, 0x80, 0x80, 0xF0, // C
			0xE0, 0x90, 0x90, 0x90, 0xE0, // D
			0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
			0xF0, 0x80, 0xF0, 0x80, 0x80, // F
		};

	public:
		void init(Screen& screenPtr, Keypad& keypadPtr, Sound& soundPtr);
		void run();
		bool loadProgram(const std::string& filename);
		void runTimers(uint8_t deltaTime);
		bool drawFlag;
		void reset();
		std::string romPath = "";
};