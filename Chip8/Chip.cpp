#include <fstream>
#include <iostream>
#include "Chip.h"
#include <stdio.h>

void Chip::init() {
	memset(memory, 0, sizeof(memory));
	memset(V, 0, sizeof(V));
	memset(stack, 0, sizeof(stack));
	memset(keys, 0, sizeof(keys));
	memset(display, 0, sizeof(display));

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

	programCounter += 2;

	// Decode opcode

	uint8_t arg1 = (opcode & 0xF000) >> 12;
	uint8_t arg2 = (opcode & 0x0F00) >> 8;
	uint8_t arg3 = (opcode & 0x00F0) >> 4;
	uint8_t arg4 = (opcode & 0x000F);

	//std::printf("Opcode 1: 0x%04X\n", arg1);

	//std::printf("Opcode 2: 0x%04X\n", arg2);

	//std::printf("Opcode 3: 0x%04X\n", arg3);

	//std::printf("Opcode 4: 0x%04X\n", arg4);

	uint8_t op = (opcode & 0xF000) >> 12;
	uint8_t x = (opcode & 0x0F00) >> 8;
	uint8_t y = (opcode & 0x00F0) >> 4;
	uint8_t n = (opcode & 0x000F);
	uint8_t kk = (opcode & 0x00FF);
	uint8_t nnn = (opcode & 0x0FFF);

	// SYS addr
	if (op == 0x0) {
		// Ignored by modern interpreters
		return;
	}

	// CLS (0x00E0)
	if (opcode == 0x00E0) {
		// Add later
		return;
	}

	// RET (0x00EE)
	if (opcode == 0x00EE) {
		stackPointer--;
		programCounter = stack[stackPointer];
		return;
	}

	// JP addr (1nnn)
	if (op == 0x1) {
		programCounter = nnn;
		return;
	}

	// CALL addr
	if (op == 0x2) {
		if (stackPointer < 15) {
			stack[stackPointer] = programCounter + 2; // Put the PC on top of the stack (2 bytes ahead = 1 instruction ahead)
			stackPointer++;
			programCounter = opcode & 0x0FFF;
		}
		else {
			std::cerr << "Stack Overflow" << std::endl;
		}
		return;
	}

	// SE Vx, byte (3xkk)
	if (op == 0x3) {
		if (V[x] == kk) {
			// Skip next instruction
			programCounter += 2;
		}
		return;
	}

	// SNE Vx, byte (4xkk)
	if (op == 0x4) {
		if (V[x] != kk) {
			programCounter += 2;
		}
		return;
	}

	// SE Vx, Vy (5xy0)
	if (op == 0x5) {
		if (V[x] == V[y]) {
			programCounter += 2;
		}
		return;
	}

	// LD Vx, byte (6xkk)
	if (op == 0x6) {
		V[x] = kk;
		return;
	}

	// ADD Vx, byte 7xkk
	if (op == 0x7) {
		V[x] = V[x] + kk;
		return;
	}

	// 8xyn
	if (op == 0x8) {
		switch (n) {
			case 0x0:{ // 8xy0 - LD Vx, Vy
				V[x] = V[y];
				break;
			}

			case 0x1: {
				// 8xy1 - OR Vx, Vy
				V[x] = V[x] | V[y];
				break;
			}

			case 0x2: { // 8xy2 - AND Vx, Vy
				V[x] = V[x] & V[y];
				break;
			}
			case 0x3: { // 8xy3 - XOR Vx, Vy
				V[x] = V[x] ^ V[y];
				break;
			}
			case 0x4: { // 8xy4 - ADD Vx, Vy
				uint16_t sum = V[x] + V[y];
				V[0xF] = (sum > 255) ? 1 : 0;
				V[x] = (uint8_t)(sum & 0xFF);
				break;
			}
			case 0x5: { // 8xy5 - SUB Vx, Vy
				uint8_t borrow = (V[x] >= V[y]) ? 0 : 1;
				V[x] = V[x] - V[y];
				V[0xF] = 1 - borrow;
				break;
			}
			case 0x6: { // 8xy6 - SHR Vx {, Vy}
				V[0xF] = V[x] & 0x1;
				V[x] = V[x] / 2;
				break;
			}
			case 0x7: { // 8xy7 - SUBN Vx, Vy
				// Set Vx = Vy - Vx, set VF = NOT borrow.
				uint8_t borrow = (V[y] >= V[x]) ? 0 : 1;
				V[x] = V[y] - V[x];
				V[0xF] = 1 - borrow;
				break;
			}
			case 0xE: { // 8xyE - SHL Vx {, Vy}
				V[0xF] = (V[x] & 0x80) >> 7;
				V[x] = V[x] * 2;
				break;
			}
		}

		// TODO: Use switch-case for op, finish rest of opcodes, access screen through chip.
	}

	// LX Vx, byte (6xkk)
	if ((opcode & 0xF000) == 0x6000) {
		V[x] = kk;
		return;
	}
}