#ifndef INPUT_H
#define INPUT_H

#include <stdbool.h>
#include "../core/types.h"

typedef enum {
    BUTTON_A = 0x01,
    BUTTON_B = 0x02,
    BUTTON_SELECT = 0x04,
    BUTTON_START = 0x08,
    BUTTON_RIGHT = 0x10,
    BUTTON_LEFT = 0x20,
    BUTTON_UP = 0x40,
    BUTTON_DOWN = 0x80 
} JoypadButton;

void input_init(void);
void input_button_press(JoypadButton button);
void input_button_release(JoypadButton button);
void input_update_state(void); // This is called every frame

#endif