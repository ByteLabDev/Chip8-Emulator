// Chip8.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

// https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Technical-Reference
// https://github.com/mattmikolay/chip-8/wiki/CHIP%E2%80%908-Instruction-Set
// https://github.com/Johnnei/Youtube-Tutorials/tree/master/emulator_chip8
// http://devernay.free.fr/hacks/chip8/C8TECH10.HTM


#include <iostream>
#include "Chip.h"

int main()
{
	Chip chip;

	chip.init();
	chip.run();
}