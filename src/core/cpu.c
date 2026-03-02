#include <stdio.h>
#include <stdint.h>
#include "cpu.h"
#include "memory.h"
#include "ppu.h"

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
    cpu->ei = false;
    cpu->awake = false;
    cpu->cycles = 0;

    cpu->timer_counter = 0;
    cpu->div = 0;

    // // Pixel Processing Unit

    // PPU ppu;
    // ppu_init(&ppu);
}

void cpu_step(CPU* cpu, PPU* ppu){

    // Need to run interrupt logic first
    cpu_interrupt_handler(cpu);

    if (cpu->isHalted){
        cpu->cycles += 4;
        cpu_update_timer(cpu, 4);
        ppu_step(ppu, 4);
        return;
    }

    if (cpu->ei){ // If interrupts ie enabled for next instruction, set ime to true and reset ei 
        cpu->ime = true;
        cpu->ei = false;

    }

    // if (cpu->ime){
    //     cpu_interrupt_handler(cpu);
    // }
    
    u64 prev_cycles = cpu->cycles;

    u8 opcode = memory_read(cpu->pc++);
    cpu_execute(cpu, opcode);

    u64 delta = cpu->cycles - prev_cycles;
    u32 executed_t_cycles = (u32)delta;

    u8 executed_cycles = (u8)(executed_t_cycles > 255) ? 255 : (u8)executed_t_cycles; // Calculate cycles of most recently executed instruction
    cpu_update_timer(cpu, executed_cycles); // Update timer

    // Update Graphics
    ppu_step(ppu, executed_cycles);


}