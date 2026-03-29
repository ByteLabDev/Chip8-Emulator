#pragma once
#include <windows.h>
#include <shobjidl.h> 
#include <string>
#include <cstdint>
#include <iostream>
#include <sstream>
#include <SDL3/SDL.h>

class Chip;
class Screen;

class Menu {
	public:
		enum class ScreenAction {
			None,
			Quit,
			OpenROM,
			Reset
		};

		static std::string openFileDialog();
		static ScreenAction getKeystrokeAction();
		static bool handleAction(Menu::ScreenAction action, Chip& chip, Screen& screen);
};