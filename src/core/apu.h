#ifndef APU_H
#define APU_H

#include <stdbool.h>
#include "types.h"

typedef struct{
    // Channel 1
    bool ch1_enabled;
    u16 ch1_volume;
    u8 ch1_frequency;
    u8 ch1_duty;

    // Channel 2
    bool ch2_enabled;
    u16 ch2_volume;
    u8 ch2_frequency;
    u8 ch2_duty;

    // Channel 3
    bool ch1_enabled;
    u16 ch1_volume;
    u8 ch1_frequency;
    u8 ch1_duty;

    // Channel 4
}APU;

#endif