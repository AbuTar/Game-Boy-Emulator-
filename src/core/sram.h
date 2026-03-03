#ifndef SRAM_H
#define SRAM_H

#include <stdbool.h>
#include "memory.h"

void sram_init(const char* rom_path);
void sram_load(void);
void sram_save(void);
void sram_cleanup(void);
void sram_mark_dirty(void);

#endif