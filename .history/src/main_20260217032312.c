#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu.h"
#include "memory.h"


int main(int argc, char* argv[]){ // Usage : emulator.exe test.gb - takes 2 arguments
    CPU cpu;
    memory_init();
    cpu_init(&cpu);

    if (argc < 2){ 
        printf("Usage: emulator.exe <rom.gb>\n");
        return 1;
    }

    if (!load_rom(argv[1])){ // argv[1] is the ROM file path
        return 1;
    }

    printf("RUNNING ROM:.......\n");

    while (!cpu.isHalted && cpu.cycles < 1000000000){
        cpu_step(&cpu);

        u8 output = memory_read(0xFF01);
        if (output != 0){
            printf("%c", output);
            fflush(stdout);
            memory_write(0xFF01, 0);        
        }
    }

    printf("\n\n=== Execution Complete ===\n");
    printf("Total cycles: %llu\n", cpu.cycles);
    printf("Halted: %s\n", cpu.isHalted ? "Yes" : "No (timeout)");
    printf("Final PC: 0x%04X\n", cpu.pc);

    return 0;
}
