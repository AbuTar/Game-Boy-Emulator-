#include "memory.h"
#include <string.h>

static u8 memory [0x10000]; // 64 KB of Memory

void memory_init(void){
    memset(memory, 0, 0x10000);
}

u8 memory_read(u16 address){
    return memory[address];
}

void memory_write(u16 address, u8 value){
    memory[address] = value;
}