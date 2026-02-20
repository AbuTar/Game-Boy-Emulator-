#ifndef MEMORY_H
#define MEMORY_H

#include "types.h"

void memory_init(void);
u8 memory_read(u16 address);
void memory_write(u16 address, u8 value);
void memory_load_rom(u8* data, size_t size);


#endif
