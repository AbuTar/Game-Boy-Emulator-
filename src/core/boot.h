#ifndef BOOT_H
#define BOOT_H

#include "types.h"
#include <stdbool.h>

#define BOOT_ROM_SIZE 256

void boot_init(void);
bool boot_is_active(void);
u8 boot_read(u16 address);
void boot_disable(void);

#endif