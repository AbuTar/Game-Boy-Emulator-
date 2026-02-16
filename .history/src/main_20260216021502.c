#include <stdio.h>
#include "cpu.h"
#include "memory.h"


int main() {
    CPU cpu;

    // Intilialise Memory
    memory_init();

    // Initialise CPU
    cpu_init(&cpu);

    // Test Code below

    memory_write(0x0100, 0x3E);  // LD A 
    memory_write(0x0101, 0x05);  // Loads 5

    memory_write(0x0102, 0x06);  // LD B 
    memory_write(0x0103, 0x0A);  // Loads 10

    memory_write(0x0104, 0x80);  // ADD A, B

    
    memory_write(0x0105, 0x21);  // LD HL
    memory_write(0x0106, 0x02);  // Game Boy is little-endian 
    memory_write(0x0107, 0xC0);  // So stored in memory location 0xC002 

    memory_write(0x0108, 0x77);   // LD (HL), A
    memory_write(0x0109, 0x76);   // HALT

    while (!cpu.isHalted){
        cpu_step(&cpu);
    }

    u8 result = memory_read(0xC002);
    printf("Result in memory: %d (0x%02X)\n", result, result);
    
}

