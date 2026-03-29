#include "Chip.h"
#include "Screen.h"
#include "Keypad.h"
#include "Sound.h"

void Chip::init(Screen& screenPtr, Keypad& keypadPtr, Sound& soundPtr, ChipType type) {
	this -> screen = &screenPtr;
	this -> keypad = &keypadPtr;
	this -> sound = &soundPtr;
	chipType = type;

	memset(memory, 0, sizeof(memory));
	memset(V, 0, sizeof(V));
	memset(stack, 0, sizeof(stack));
	memset(display, 0, sizeof(display));

	I = 0;
	programCounter = 0x200; // Most Chip-8 programs start at location 0x200 (512), but some begin at 0x600 (1536). Programs beginning at 0x600 are intended for the ETI 660 computer.
	stackPointer = 0;
	delayTimer = 0;
	soundTimer = 0;
	drawFlag = false;
	isKeyDown = false;

	memcpy(memory, fontset, sizeof(fontset));
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

	std::vector<char> buffer(size);

	file.seekg(0, std::ios::beg);
	file.read(buffer.data(), size);
	file.close();

	// Load the buffer into the Chip's memory starting at 0x200
	for (int i = 0; i < size; ++i) {
		memory[0x200 + i] = static_cast<uint8_t>(buffer[i]);
	}

	romPath = filename;

	return true;
}

void Chip::reset() {
	memset(memory, 0, sizeof(memory));
	memset(V, 0, sizeof(V));
	memset(stack, 0, sizeof(stack));
	memset(display, 0, sizeof(display));

	I = 0;
	programCounter = 0x200; // Most Chip-8 programs start at location 0x200 (512), but some begin at 0x600 (1536). Programs beginning at 0x600 are intended for the ETI 660 computer.
	stackPointer = 0;
	delayTimer = 0;
	soundTimer = 0;
	drawFlag = false;
	isKeyDown = false;

	memcpy(memory, fontset, sizeof(fontset));

	screen->clear();
}

void Chip::runTimers(uint8_t deltaTime) {
	// Both timers run at 60hz
	dtDeltaTime += deltaTime;
	stDeltaTime += deltaTime;
	uint8_t timeBetweenIncrements = 17; // 16.667

	if (dtDeltaTime >= timeBetweenIncrements) {
		if(delayTimer > 0) delayTimer--;
		dtDeltaTime = 0;
	}
	if (stDeltaTime >= timeBetweenIncrements) {
		if (soundTimer > 0) soundTimer--;
		stDeltaTime = 0;
	}
}

void Chip::run() {
	// Play sound
	sound->update(soundTimer);

	// Fetch opcode
	uint16_t opcode = (memory[programCounter] << 8) | memory[programCounter + 1];
	//std::printf("PC: 0x%03X | Opcode: 0x%04X\n", programCounter, opcode);

	programCounter += 2;

	drawFlag = false;

	// Decode opcode
	uint16_t op = (opcode & 0xF000) >> 12; // 16 bits
	uint8_t x = (opcode & 0x0F00) >> 8; // 4 bits
	uint8_t y = (opcode & 0x00F0) >> 4; // 4 bits
	uint8_t n = (opcode & 0x000F); // 4 bits
	uint8_t kk = (opcode & 0x00FF); // 8 bits
	uint16_t nnn = (opcode & 0x0FFF); // 16 bits

	// CLS (0x00E0)
	if (opcode == 0x00E0) {
		screen->clear();
		drawFlag = true;
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

	// CALL addr (2nnn)
	if (op == 0x2) {
		if (stackPointer < 15) {
			stack[stackPointer] = programCounter; // Put the PC on top of the stack (2 bytes ahead = 1 instruction ahead)
			stackPointer++;
			programCounter = nnn;
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
				V[x] = (uint8_t)(sum & 0xFF);
				V[0xF] = (sum > 255) ? 1 : 0; // Set carry
				break;
			}
			case 0x5: { // 8xy5 - SUB Vx, Vy
				uint8_t borrow = (V[x] >= V[y]) ? 0 : 1;
				V[x] = V[x] - V[y];
				V[0xF] = 1 - borrow;
				break;
			}
			case 0x6: { // 8xy6 - SHR Vx {, Vy}
				uint8_t lsb = V[x] & 0x1; // Least significant bit = 0x0000 0001
				V[x] = (V[x] >> 1); // Divide by 2 by shifting right once
				V[0xF] = lsb;
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
				uint8_t msb = V[x] & 0b10000000; // Most significant bit = 1000 0000
				V[x] = V[x] * 2;
				V[0xF] = (msb) >> 7; // 1000 0000
				break;
			}
		}

		return;
	}

	// SNE Vx, Vy (9xy0)
	if (op == 0x9) {
		if (V[x] != V[y]) {
			programCounter += 2;
		}
		return;
	}

	// LD I, addr (Annn)
	if (op == 0xA) {
		I = nnn;
		return;
	}

	// JP V0, addr (Bnnn)
	if (op == 0xB) {
		programCounter = nnn + V[0];
		return;
	}

	// RND Vx, byte (Cxkk)
	if (op == 0xC) {
		uint8_t randNumber = rand() % 256; // Generates random number between 0 and 255
		V[x] = (randNumber & kk);
		return;
	}

	// DRW Vx, Vy, nibble (Dxyn)
	if (op == 0xD) {
		drawFlag = true;
		V[0xF] = 0; // Set to 0 if no XOR is detected
		for (int row = 0; row < n; row++) {
			uint8_t spriteByte = memory[I + row];
			for (int col = 0; col < 8; col++) {
				if ((spriteByte & (0x80 >> col)) != 0) { // Check if the pixel is on
					if (screen->setPixel(V[x] + col, V[y] + row)) { // XOR detected
						V[0xF] = 1; // Collision detected
					}
				}
			}
		}
		return;
	}

	// SKP Vx (Ex9E)
	if (op == 0xE && kk == 0x9E) {
		uint8_t key = V[x];
		if (keypad->keyStates[key]) {
			// True = Down, False = Up
			programCounter += 2;
		}
		return;
	}

	// SKNP Vx (ExA1)
	if (op == 0xE && kk == 0xA1) {
		uint8_t key = V[x];
		if (!keypad->keyStates[key]) {
			// True = Down, False = Up
			programCounter += 2;
		}
		return;
	}

	if (op == 0xF) {
		switch (kk) {
			case 0x07: { // Fx07 - LD Vx, DT
				V[x] = delayTimer;
				break;
			}
			case 0x0A: { // Fx0A - LD Vx, K
				programCounter -= 2; // Go back 1 instruction to repeat this one (prevents program from hanging).

				bool localIsKeyDown = false;

				for (int i = 0; i < 16; i++) {
					if (keypad->keyStates[i]) { // Key is down
						downKey = i;
						localIsKeyDown = true;
						break;
					}
				}

				if (isKeyDown && !localIsKeyDown) {
					// Key has been released, break the loop.
					programCounter += 2; // Go forward 1 instruction (break the loop).
					V[x] = downKey;
				}

				isKeyDown = localIsKeyDown;

				break;
			}
			case 0x15: { // Fx15 - LD DT, Vx
				delayTimer = V[x];
				break;
			}
			case 0x18: { //  Fx18 - LD ST, Vx
				soundTimer = V[x];
				break;
			}
			case 0x1E: { // Fx1E - ADD I, Vx
				I = I + V[x];
				break;
			}
			case 0x29: { // Fx29 - LD F, Vx
				I = V[x] * 5;
				break;
			}
			case 0x33: { // Fx33 - LD B, Vx
				uint8_t ones = V[x] % 10;
				uint8_t tens = (V[x] / 10) % 10;
				uint8_t hundreds = (V[x] / 100) % 10;

				memory[I] = hundreds;
				memory[I + 1] = tens;
				memory[I + 2] = ones;
				break;
			}
			case 0x55: { // Fx55 - LD [I], Vx
				for (int i = 0; i <= x; i++) {
					memory[I + i] = V[i];
				}
				break;
			}
			case 0x65: { // Fx65 - LD Vx, [I]
				for (int i = 0; i <= x; i++) {
					V[i] = memory[I + i];
				}
				break;
			}
		}
		return;
	}

	// LX Vx, byte (6xkk)
	if ((opcode & 0xF000) == 0x6000) {
		V[x] = kk;
		return;
	}

	// This point forward, all the opcodes are for Super-Chip
	// Reference: http://devernay.free.fr/hacks/chip8/schip.txt

	// Todo: I'll finish these tomorrow.

	if(chipType != ChipType::Super_Chip) return;

	// 00CN*    Scroll display N lines down
	if ((opcode & 0xFFF0) == 0x00C0) {
		screen->scrollDown(n);
		return;
	}

	// 00FB*    Scroll display 4 pixels right
	if (opcode == 0x00FB) {
		screen->scrollRight();
		return;
	}
	
	// 00FC*    Scroll display 4 pixels left
	if (opcode == 0x00FC) {
		screen->scrollLeft();
		return;
	}
	
	// 00FD*    Exit CHIP interpreter

	// 00FE*    Disable extended screen mode
	if (opcode == 0x00FE) {
		screen->setExtendedMode(false);
		return;
	}

	// 00FF*    Enable extended screen mode for full - screen graphics
	if (opcode == 0x00FF) {
		screen->setExtendedMode(true);
		return;
	}

	// DXYN*    Show N-byte sprite from M(I) at coords (VX,VY), VF := collision.If N = 0 and extended mode, show 16x16 sprite.
	
	// FX30*    Point I to 10-byte font sprite for digit VX (0..9)

	// FX75*    Store V0..VX in RPL user flags(X <= 7)

	// FX85*    Read V0..VX from RPL user flags(X <= 7)
}