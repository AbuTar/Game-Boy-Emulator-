#ifndef MEMORY_H
#define MEMORY_H
#include <stdint.h>
#include "types.h"

void memory_init(void);
u8 memory_read(u16 address);
void memory_write(u16 address, u8 value);
void memory_load_rom(u8* data, size_t size);
void clear_memory(void);
void request_interrupt(uint8_t interrupt_flag);
void memory_set_joypad_low(u8 low4);
void memory_set_io(u8 address, u8 integer);

// Saves support
void memory_load_sram(const u8* data, size_t size);
size_t memory_get_sram(u8* data_out);
void sram_mark_dirty(void);



#endif
