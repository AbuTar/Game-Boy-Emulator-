#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "types.h"
#include <stdbool.h>

// CPU Flags
#define FLAG_Z 0x80
#define FLAG_N 0x40
#define FLAG_H 0x20
#define FLAG_C 0x10

// CPU Components

typedef struct {
    // 8-bit Registers

    u8 A; // Accumulator
    u8 F; // Flags Register
    u8 B, C, D, E, H, L; // General-purpose Registers

    // 16-bit Registers
    u16 sp; // Stack Pointer
    u16 pc; // Program Counter

    // CPU States

    bool isHalted; // HALT instruction
    bool ime; // Interrupt Enabled

    u64 cycles; // Total clock cycles executed

} CPU;

// Register Pair Getters

u16 get_af(CPU* cpu);
u16 get_bc(CPU* cpu);
u16 get_de(CPU* cpu);
u16 get_hl(CPU* cpu);

// Register Pair Setters

void set_af(CPU* cpu, u16 val);
void set_bc(CPU* cpu, u16 val);
void set_de(CPU* cpu, u16 val);
void set_hl(CPU* cpu, u16 val);

// CPU Functions

void cpu_init(CPU* cpu); // Initialises CPU
void cpu_step(CPU* cpu); // Increments PC
void cpu_execute(CPU* cpu, u8 opcode); // Executes instruction based on Opcode input
void cpu_execute_cb(CPU* cpu, u8 opcode); // Executes special instructions 

// Flag Getters / Setters

void set_flag(CPU* cpu, u8 flag, bool set);
bool get_flag(CPU* cpu, u8 flag);

// Function Declarations

void ld_reg_mem(CPU cpu, u8* dest, u16 addr);
void ld_reg_reg(CPU* cpu, u8* dest_reg, u8 src_reg);
void ld_pair_reg(CPU* cpu, u8* dest_reg, u16 addr);
void ld_reg_pair(CPU* cpu, u16 addr, u8 src_reg);
void ld_reg_imm(CPU* cpu, u8* dest_reg);

#endif