#include <stdio.h>
#include "input.h"
#include "../core/memory.h"

static u8 button_state;

void input_init(void){
    button_state = 0xFF;
}
// Set Bits to High if unpressed, Set Bits to Low if pressed
void input_button_press(JoypadButton button){
    button_state &= ~button;
}

void input_button_release(JoypadButton button){
    button_state |= button;
}

void input_update_state(void){
    u8 joypad = memory_read(0xFF00);
    u8 joypad_out = 0x0F; // Lower Nibble

    // Bit 4 corresponds to D-pad 
    if (!(joypad & 0x10)){
        joypad_out &= (button_state >> 4) & 0x0F;
    }

    // Bit 5 corresponds to Buttons
    if ((joypad & 0x20) == 0) {
        // Returns Button State and shifts bits to lower nibble
        joypad_out &= button_state &0x0F;
    }

    joypad_out = (joypad & 0x30) | (joypad_out & 0x0F);

    // memory_write(0xFF00, joypad_out);
    memory_set_joypad_low(joypad_out);
}