#pragma once
#include <cstring>
#include <cstdint>
#include <iostream>
#include "Screen.h"

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

		// I don't know how large the timers need to be, so I will use int for now.
		int delayTimer;
		int soundTimer;

		uint8_t keys[16];

		// The original implementation of the Chip-8 language used a 64x32-pixel monochrome display
		uint8_t display[64 * 32];

		Screen* screen;

	public:
		void init(Screen& screenPtr);
		void run();
		bool loadProgram(const std::string& filename);
};