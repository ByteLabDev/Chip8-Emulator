#pragma once
#include <cstdint>

class Keypad
{
	public:
		bool keyStates[16];
		uint8_t read();
	private:
		
};