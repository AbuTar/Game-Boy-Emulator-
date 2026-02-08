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

        // ==================== 0x00 - 0x0F ====================
        case 0x00: // NOP
            cpu->cycles += 4; 
            break;

        case 0x01:
            ld_reg_pair(cpu, get_bc(cpu), cpu-> A ); // A to HL
            cpu->cycles += 8;
            break;

        case 0x02:
            break;
        case 0x03:
            break;
        case 0x04:
            break;
        case 0x05:
            break;
        case 0x06:
            break;
        case 0x07:
            break;
        case 0x08:
            break;
        case 0x09:
            break;
        case 0x0A:
            break;
        case 0x0B:
            break;
        case 0x0C:
            break;
        case 0x0D:
            break;
        case 0x0E:
            break;
        case 0x0F:
            break;

        // 0x10 - 0x1F
        // 0x20 - 0x2F
        // 0x30 - 0x3F

        // ==================== 0x40 - 0x4F ====================
        case 0x40:
            ld_reg_reg(cpu, &cpu->B, cpu->B);// B to B
            cpu->cycles += 4;
            break;
        
        case 0x41:
            ld_reg_reg(cpu, &cpu->B, cpu->C); // C to B
            cpu->cycles += 4;
            break;

        case 0x42:
            ld_reg_reg(cpu, &cpu->B, cpu->D); // D to B
            cpu->cycles += 4;
            break;

        case 0x43:
            ld_reg_reg(cpu, &cpu->B, cpu->E); // E to B
            cpu->cycles += 4;
            break;

        case 0x44:
            ld_reg_reg(cpu, &cpu->B, cpu->H); // H to B
            cpu->cycles += 4;
            break;

        case 0x45:
            ld_reg_reg(cpu, &cpu->B, cpu->L); // L to B
            cpu->cycles += 4;
            break;

        case 0x46:
            ld_pair_reg(cpu, &cpu->B, get_hl(cpu)); // HL to B
            cpu->cycles += 8;
            break;

        case 0x47:
            ld_reg_reg(cpu, &cpu->B, cpu->A); // A to B
            cpu->cycles += 4;
            break;

        case 0x48:
            ld_reg_reg(cpu, &cpu->C, cpu->B); // B to C
            cpu->cycles += 4;
            break;

        case 0x49:
            ld_reg_reg(cpu, &cpu->C, cpu->C); // C to C
            cpu->cycles += 4;
            break;

        case 0x4A:
            ld_reg_reg(cpu, &cpu->C, cpu->D); // D to C
            cpu->cycles += 4;
            break;

        case 0x4B:
            ld_reg_reg(cpu, &cpu->C, cpu->E); // E to C
            cpu->cycles += 4;
            break;

        case 0x4C:
            ld_reg_reg(cpu, &cpu->C, cpu->H); // H to C
            cpu->cycles += 4;
            break;

        case 0x4D:
            ld_reg_reg(cpu, &cpu->C, cpu->L); // L to C
            cpu->cycles += 4;
            break;

        case 0x4E:
            ld_pair_reg(cpu, &cpu->B, get_hl(cpu)); // HL to C
            cpu->cycles += 4;
            break;

        case 0x4F:
            ld_reg_reg(cpu, &cpu->C, cpu->A); // A to C
            cpu->cycles += 4;
            break;

        // ==================== 0x50 - 0x5F ====================

        case 0x50:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;
        
        case 0x51:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;
        
        case 0x52:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;

        case 0x53:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;

        case 0x54:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;

        case 0x55:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;

        case 0x56:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;

        case 0x57:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;

        case 0x58:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;

        case 0x59:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;

        case 0x5A:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;

        case 0x5B:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;

        case 0x5C:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;

        case 0x5D:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;

        case 0x5E:
            ld_reg_reg(cpu, &cpu->D, cpu->D);
            cpu->cycles += 4;
            break;
        
        case 0x5F:
            break;
        

        // 0x50 - 0x5F
        // 0x60 - 0x6F
        // 0x70 - 0x7F
        // 0x80 - 0x8F
        // 0x90 - 0x9F
        // 0xA0 - 0xAF
        // 0xB0 - 0xBF
        // 0xC0 - 0xCF
        // 0xD0 - 0xDF
        // 0xE0 - 0xEF
        // 0xF0 - 0xFF

        
            

        case 0x3E: // LD A, d8
            cpu->A = memory_read(cpu->pc++);
            cpu->cycles += 8;
            break;
        
        // case 0x96: // LD B, d8 
        //     cpu->B = memory_read(cpu->pc++);
        //     cpu->cycles += 8;
        //     break;
            
        // case 0x8E: // LD C, d8 
        //     cpu->C = memory_read(cpu->pc++);
        //     cpu->cycles += 8;
        //     break;

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

// -------------------- 8-bit Opcode Instruction Helper Functions --------------------

// LD Helper Functions
void ld_reg_reg(CPU* cpu, u8* dest_reg, u8 src_reg){
    *dest_reg = src_reg;
}

void ld_pair_reg(CPU* cpu, u8* dest_reg, u16 addr){
    *dest_reg = memory_read(addr);
}

void ld_reg_pair(CPU* cpu, u16 addr, u8 src_reg){
    memory_write(addr, src_reg);
}

void ld_reg_imm(CPU* cpu, u8* dest_reg){
    *dest_reg = memory_read(cpu->pc++);
}





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

