#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include "types.h"
#include <stdbool.h>

// LOG 
#define UNIMPLEMENTED_OPCODES(opcode,pc) \
    printf("UNIMPLEMENTED OPCODE: 0x%02X at PC: 0x%04X\n", opcode, pc);

// CPU Flags
#define FLAG_Z 0x80 // 1000 0000
#define FLAG_N 0x40 // 0100 0000
#define FLAG_H 0x20 // 0010 0000
#define FLAG_C 0x10 // 0001 0000

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
    bool awake; // Flag used to exit HALT
    bool ime; // Interrupt Mastee Enable
    bool ei; // Interrupts after next instruction
    u16 div; // Divider Register
    u16 timer_counter; // TIMA


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
void set_sp(CPU* cpu, u16 val);
// CPU Functions

void cpu_init(CPU* cpu); // Initialises CPU
void cpu_step(CPU* cpu); // Increments PC
void cpu_execute(CPU* cpu, u8 opcode); // Executes instruction based on Opcode input
void cpu_execute_cb(CPU* cpu, u8 opcode); // Executes special instructions 
void cpu_interrupt_handler(CPU* cpu); // Handles Interrupts

// Flag Getters / Setters

void set_flag(CPU* cpu, u8 flag, bool set);
bool get_flag(CPU* cpu, u8 flag);

// Function Declarations (For Opcodes 0x00 - 0xFF)

void ld_reg_mem(CPU* cpu, u8* dest, u16 addr);
void ld_reg_reg(CPU* cpu, u8* dest_reg, u8 src_reg);
void ld_pair_reg(CPU* cpu, u8* dest_reg, u16 addr);
void ld_reg_pair(CPU* cpu, u16 addr, u8 src_reg);
void ld_reg_imm(CPU* cpu, u8* dest_reg);
void ld_imm_pair(CPU* cpu, u16 addr);
void ld_pair_imm(CPU* cpu, void (*setter)(CPU*, u16));
void lda_reg_hl_inc(CPU* cpu);
void lda_reg_hl_dec(CPU* cpu);
void lda_hl_reg_inc(CPU* cpu);
void lda_hl_reg_dec(CPU* cpu);
void add_reg(CPU* cpu, u8* dest_reg, u8 src_reg);
void add_reg_carry(CPU* cpu, u8* dest_reg, u8 src_reg);
void sub_reg(CPU* cpu, u8* dest_reg, u8 src);
void sub_reg_carry(CPU* cpu, u8* dest_reg, u8 src);
void and_reg(CPU* cpu, u8* dest_reg, u8 src);
void xor_reg(CPU* cpu, u8* dest_reg, u8 src);
void or_reg(CPU* cpu, u8* dest_reg, u8 src);
void cp_reg(CPU* cpu, u8* dest_reg, u8 src);
void inc_reg(CPU* cpu, u8* reg);
void dec_reg(CPU* cpu, u8* reg);
void inc_pair_val(CPU* cpu, u16 (*getter)(CPU*), void (*setter)(CPU*, u16));
void dec_pair_val(CPU* cpu, u16 (*getter)(CPU*), void (*setter)(CPU*, u16));
void inc_hl_mem(CPU* cpu);
void dec_hl_mem(CPU* cpu);
void add_hl_pair(CPU* cpu, u16 val);
void rlca(CPU* cpu);
void rrca(CPU* cpu);
void rla(CPU* cpu);
void rra(CPU* cpu);
void jr(CPU* cpu);
void adjust_acc(CPU* cpu);
void complement_acc(CPU* cpu);
void set_carry_cy(CPU* cpu);
void flip_cy(CPU* cpu);
void ld_a16_sp(CPU* cpu);
void push(CPU* cpu, u16 val);
u16 pop(CPU* cpu);
void call(CPU* cpu);
void ret(CPU* cpu);
void jp(CPU* cpu);
void rst(CPU* cpu, u8 addr);

// Function Declarations (For CB Pre-fixes Opcodes)

void rl_reg(CPU* cpu, u8* reg);
void rr_reg(CPU* cpu, u8* reg);
void rlc_reg(CPU* cpu, u8* reg);
void rrc_reg(CPU* cpu, u8* reg);
void sla_reg (CPU* cpu, u8* reg);
void sra_reg (CPU* cpu, u8* reg);
void srl_reg(CPU* cpu, u8* reg);
void swap_reg(CPU* cpu, u8* reg);
void cpy_bit_reg(CPU* cpu, u8 bit, u8 reg);
void rst_bit_reg(CPU* cpu, u8 bit, u8* reg);
void set_bit_reg(CPU* cpu, u8 bit, u8* reg);
void rlc_hl(CPU* cpu);
void rrc_hl(CPU* cpu);
void rl_hl(CPU* cpu);
void rr_hl(CPU* cpu);
void sla_hl(CPU* cpu);
void sra_hl(CPU* cpu);
void srl_hl(CPU* cpu);
void swap_hl(CPU* cpu);
void rst_bit_hl(CPU* cpu, u8 bit);
void set_bit_hl(CPU* cpu, u8 bit);



#endif