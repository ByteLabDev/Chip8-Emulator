#include "Keypad.h"
#include <SDL3/SDL.h>
#include <iostream>
#include <stdio.h>

uint8_t Keypad::read() {
    const bool* key_states = SDL_GetKeyboardState(NULL);

    bool newKeyStates[16];

    newKeyStates[0x0] = key_states[SDL_SCANCODE_0];
    newKeyStates[0x1] = key_states[SDL_SCANCODE_1];
    newKeyStates[0x2] = key_states[SDL_SCANCODE_2];
    newKeyStates[0x3] = key_states[SDL_SCANCODE_3];
    newKeyStates[0x4] = key_states[SDL_SCANCODE_4];
    newKeyStates[0x5] = key_states[SDL_SCANCODE_5];
    newKeyStates[0x6] = key_states[SDL_SCANCODE_6];
    newKeyStates[0x7] = key_states[SDL_SCANCODE_7];
    newKeyStates[0x8] = key_states[SDL_SCANCODE_8];
    newKeyStates[0x9] = key_states[SDL_SCANCODE_9];
    newKeyStates[0xA] = key_states[SDL_SCANCODE_A];
    newKeyStates[0xB] = key_states[SDL_SCANCODE_B];
    newKeyStates[0xC] = key_states[SDL_SCANCODE_C];
    newKeyStates[0xD] = key_states[SDL_SCANCODE_D];
    newKeyStates[0xE] = key_states[SDL_SCANCODE_E];
    newKeyStates[0xF] = key_states[SDL_SCANCODE_F];

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