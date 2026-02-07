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

void cpu_execute(CPU* cpu, u8 opcode){
    switch(opcode){

        case 0x00: // NOP
            cpu->cycles += 4;
            break;
            

        case 0x3E: // LD A, d8
            cpu->A = memory_read(cpu->pc++);
            cpu->cycles += 8;
            break;
        
        case 0x06: // LD B, d8 
            cpu->B = memory_read(cpu->pc++);
            cpu->cycles += 8;
            break;
            
        case 0x0E: // LD C, d8 
            cpu->C = memory_read(cpu->pc++);
            cpu->cycles += 8;
            break;

        case 0x80: // ADD A, B
            cpu->A = cpu->A + cpu->B;
            cpu->cycles += 4;
            break;

        case 0x21: {
            u8 lowerByte = memory_read(cpu->pc++);
            u8 higherByte = memory_read(cpu->pc++);
            set_hl(cpu, (higherByte << 8) | lowerByte);
            cpu->cycles += 12;
            break;
        }

        case 0x77: // LD (HL), A
            memory_write(get_hl(cpu), cpu-> A);
            cpu->cycles += 8;
            break;

        case 0x76: // HALT
            cpu->isHalted = true;
            cpu->cycles += 4;
            break;

        default:
            printf("Unknown opcode: 0x%02X at PC: 0x%04X\n", opcode, cpu->pc - 1);
            break;

    }
}

void ld_8bit(CPU* cpu, u8 opcode, u8 dest_reg){
    cpu->dest_reg = memory_read(cpu->pc++);

}

// Instruction Helper Functions


// Flag Helper Functions (Setter and Getter)
void set_flag(CPU* cpu, u8 flag, bool set_flag){
    if (set_flag){
        cpu->F |= flag; // Sets the Flag bit
    }

    else{
        cpu->F &= ~flag; // Clears the Flag Bit
    }
}

bool get_flag(CPU* cpu, u8 flag) {
    return (cpu->F & flag) != 0;
}

// Getters for Paired Registers

u16 get_af(CPU* cpu) {
    return (cpu->A << 8) | cpu->F;
}

u16 get_bc(CPU* cpu) {
    return (cpu->B << 8) | cpu->C;
}

u16 get_de(CPU* cpu) {
    return (cpu->D << 8) | cpu->E;
}

u16 get_hl(CPU* cpu) {
    return (cpu->H << 8) | cpu->L;
}

// Setters for Paired Registers

void set_af(CPU* cpu, u16 val){
    cpu->A = (val >> 8) & 0xFF;
    cpu->F = val & 0xFF;
}

void set_bc(CPU* cpu, u16 val){
    cpu->B = (val >> 8) & 0xFF;
    cpu->C = val & 0xFF;
}

void set_de(CPU* cpu, u16 val){
    cpu->D = (val >> 8) & 0xFF;
    cpu->E = val & 0xFF;
}

void set_hl(CPU* cpu, u16 val) {
    cpu->H = (val >> 8) & 0xFF;
    cpu->L = val & 0xFF;
}


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

