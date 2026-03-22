#include <fstream>
#include <iostream>
#include "Chip.h"

void Chip::init() {
	std::memset(memory, 0, sizeof(memory));
	std::memset(V, 0, sizeof(V));
	std::memset(stack, 0, sizeof(stack));
	std::memset(keys, 0, sizeof(keys));
	std::memset(display, 0, sizeof(display));

	I = 0;
	programCounter = 0x200; // Most Chip-8 programs start at location 0x200 (512), but some begin at 0x600 (1536). Programs beginning at 0x600 are intended for the ETI 660 computer.
	stackPointer = 0;
	delayTimer = 0;
	soundTimer = 0;
}

bool Chip::loadProgram(const std::string& filename) {
	std::ifstream file(filename, std::ios::binary | std::ios::ate);

	// Check if ROM can be read
	if (!file.is_open()) {
		std::cerr << "Failed to open ROM file: " << filename << std::endl;
		return false;
	}

	std::streampos size = file.tellg();

	// Check if ROM is too big
	if (size > (memorySize - 0x200)) {
		std::cerr << "ROM is too big for Chip8 memory (Expected less than " << (memorySize - 0x200) << " bytes, got " << size << " bytes instead)." << std::endl;
		return false;
	}

	char* buffer = new char[size];

	file.seekg(0, std::ios::beg);
	file.read(buffer, size);
	file.close();

	// Load the buffer into the Chip's memory starting at 0x200
	for (int i = 0; i < size; ++i) {
		memory[0x200 + i] = static_cast<uint8_t>(buffer[i]);
	}

	delete[] buffer;

	return true;
}

void Chip::run() {
	// Fetch opcode
	uint16_t opcode = (memory[programCounter] << 8) | memory[programCounter + 1];
	std::printf("Opcode: 0x%04X\n", opcode);

	uint8_t arg1 = (opcode & 0xF000) >> 12;
	uint8_t arg2 = (opcode & 0x0F00) >> 8;
	uint8_t arg3 = (opcode & 0x00F0) >> 4;
	uint8_t arg4 = (opcode & 0x000F);

	std::printf("Opcode 1: 0x%04X\n", arg1);

	std::printf("Opcode 2: 0x%04X\n", arg2);

	std::printf("Opcode 3: 0x%04X\n", arg3);

	std::printf("Opcode 4: 0x%04X\n", arg4);

	// Decode opcode
	// Execute opcode
}