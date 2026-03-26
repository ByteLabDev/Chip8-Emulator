#pragma once
#include <cstdint>
#include <SDL3/SDL.h>
#include <iostream>
#include <stdio.h>

class Keypad
{
	public:
		bool keyStates[16];
		uint8_t read();
	private:
		
};