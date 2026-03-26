#include "Keypad.h"

uint8_t Keypad::read() {
    const bool* key_states = SDL_GetKeyboardState(NULL);

    bool newKeyStates[16];
                                                    // QWERTY --> Chip8
    newKeyStates[0x1] = key_states[SDL_SCANCODE_1]; // 1      --> 1
    newKeyStates[0x2] = key_states[SDL_SCANCODE_2]; // 2      --> 2
    newKeyStates[0x3] = key_states[SDL_SCANCODE_3]; // 3      --> 3
    newKeyStates[0xC] = key_states[SDL_SCANCODE_4]; // 4      --> C

    newKeyStates[0x4] = key_states[SDL_SCANCODE_Q]; // Q      --> 4
    newKeyStates[0x5] = key_states[SDL_SCANCODE_W]; // W      --> 5
    newKeyStates[0x6] = key_states[SDL_SCANCODE_E]; // E      --> 6
    newKeyStates[0xD] = key_states[SDL_SCANCODE_R]; // R      --> D


    newKeyStates[0x7] = key_states[SDL_SCANCODE_A]; // A      --> 7
    newKeyStates[0x8] = key_states[SDL_SCANCODE_S]; // S      --> 8
    newKeyStates[0x9] = key_states[SDL_SCANCODE_D]; // D      --> 9
    newKeyStates[0xE] = key_states[SDL_SCANCODE_F]; // F      --> E

    newKeyStates[0xA] = key_states[SDL_SCANCODE_D]; // Z      --> A
    newKeyStates[0x0] = key_states[SDL_SCANCODE_E]; // X      --> 0
    newKeyStates[0xB] = key_states[SDL_SCANCODE_F]; // C      --> B
    newKeyStates[0xF] = key_states[SDL_SCANCODE_0]; // V      --> F


    uint8_t keyChange = NULL;

    for (int i = 0; i < 16; i++) {
        if (key_states[i] != newKeyStates[i]) {
            keyChange = i;
            break;
        }
    }

    memcpy(keyStates, newKeyStates, sizeof(newKeyStates));

    return keyChange;
}