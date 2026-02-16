#include <stdio.h>
#include <stdint.h>
#include "cpu.h"
#include "memory.h"

void cpu_init(CPU* cpu){

    // Initialise General-Purpose Registers
    cpu-> A = 0x00;
    cpu-> B = 0x00;
    cpu-> C = 0x00;
    cpu-> D = 0x00;
    cpu-> E = 0x00;
    cpu-> F = 0x00;
    cpu-> H = 0x00;
    cpu-> L = 0x00;

    // Program Counter and Stack Pointer

    cpu-> pc = 0x0100; // GameBoy headers starts here
    cpu-> sp = 0xFFFE;

    // States

    cpu->isHalted = false;
    cpu->ime = false;
    cpu->awake = false;
    cpu->cycles = 0;
}

void cpu_step(CPU* cpu){

    if (cpu->isHalted){
        cpu->cycles += 4;
        return;
    }

    u8 opcode = memory_read(cpu->pc++);
    cpu_execute(cpu, opcode);


}