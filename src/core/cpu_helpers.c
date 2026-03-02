#include "cpu.h"
#include "memory.h"
#include <stdint.h>

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

void ld_pair_imm(CPU* cpu, void(*setter)(CPU*, u16)){
    u8 lowerByte = memory_read(cpu->pc++);
    u8 upperByte = memory_read(cpu->pc++);
    setter(cpu, (upperByte<<8) | lowerByte);
}

void inc_reg(CPU* cpu, u8* reg){
    set_flag(cpu, FLAG_H, (*reg & 0xF) == 0xF); // Half carry check (Bits 0-3)
    (*reg)++;
    set_flag(cpu, FLAG_Z, *reg == 0);
    set_flag(cpu, FLAG_N, false);
    // Carry flag not affected
}

void dec_reg(CPU* cpu, u8* reg){
    set_flag(cpu, FLAG_H, (*reg & 0xF) == 0x0); 
    (*reg)--;
    set_flag(cpu, FLAG_Z, *reg == 0);
    set_flag(cpu, FLAG_N, true);
}

void inc_pair_val(CPU* cpu, u16 (*getter)(CPU*), void (*setter)(CPU*, u16)){
    setter(cpu, getter(cpu) + 1);
    
}

void dec_pair_val(CPU* cpu, u16 (*getter)(CPU*), void (*setter)(CPU*, u16)){
    setter(cpu, getter(cpu) - 1 );
    
}

void inc_hl_mem(CPU* cpu){
    u16 addr = get_hl(cpu);
    u8 val = memory_read(addr);
    set_flag(cpu, FLAG_H, (val & 0xF) == 0xF);
    val++;
    set_flag(cpu, FLAG_Z, val == 0);
    set_flag(cpu, FLAG_N, false);
    memory_write(addr, val);
}

void dec_hl_mem(CPU* cpu) {
    u16 addr = get_hl(cpu);
    u8 val = memory_read(addr);
    set_flag(cpu, FLAG_H, (val & 0xF) == 0x0);
    val--;
    set_flag(cpu, FLAG_Z, val == 0);
    set_flag(cpu, FLAG_N, true);
    memory_write(addr, val);
}

void add_hl_pair(CPU* cpu, u16 val){
    u16 hl = get_hl(cpu);
    u32 result = hl + val;

    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, ((hl & 0xFFF) + (val & 0xFFF)) > 0xFFF);
    set_flag(cpu, FLAG_C, result > 0xFFFF);
    // Z flag not affected
    set_hl(cpu, result & 0xFFFF);
}

void rlca(CPU* cpu) {   // Rotate A left, bit 7 → carry and bit 0

    u8 bit7 = (cpu->A >> 7) & 1;
    cpu->A = (cpu->A << 1) | bit7;
    set_flag(cpu, FLAG_Z, false);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, bit7);
}

void rrca(CPU* cpu) {   // Rotate A right, bit 0 → carry and bit 7

    u8 bit0 = cpu->A & 1;
    cpu->A = (cpu->A >> 1) | (bit0 << 7);
    set_flag(cpu, FLAG_Z, false);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, bit0);
}

void rla(CPU* cpu) {   // Rotate A left through carry

    u8 old_carry = get_flag(cpu, FLAG_C) ? 1 : 0;
    u8 bit7 = (cpu->A >> 7) & 1;
    cpu->A = (cpu->A << 1) | old_carry;
    set_flag(cpu, FLAG_Z, false);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, bit7);
}

void rra(CPU* cpu) {   // Rotate A right through carry

    u8 old_carry = get_flag(cpu, FLAG_C) ? 1 : 0;
    u8 bit0 = cpu->A & 1;
    cpu->A = (cpu->A >> 1) | (old_carry << 7);
    set_flag(cpu, FLAG_Z, false);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, bit0);
}

void jr(CPU* cpu){ // Jump relative

    s8 offset = (s8)memory_read(cpu->pc++);
    cpu->pc += offset;

}

void adjust_acc(CPU* cpu) {
    u8 a = cpu->A;
    u8 bcd_correction = 0;
    bool carry = false;

    if (!get_flag(cpu, FLAG_N)) { // After addition
        if (get_flag(cpu, FLAG_H) || (a & 0xF) > 9){
            bcd_correction += 0x06;
        }

        if (get_flag(cpu, FLAG_C) || a > 0x99){
            bcd_correction += 0x60;
            carry = true;
        }

        a += bcd_correction;

    } 
    
    else {  // After subtraction
        if (get_flag(cpu, FLAG_H)){
            bcd_correction += 0x06;
        }

        if (get_flag(cpu, FLAG_C)){
            bcd_correction += 0x60;
            carry = true;
        }

        a -= bcd_correction;
    }
    
    set_flag(cpu, FLAG_Z, a == 0);
    set_flag(cpu, FLAG_N, get_flag(cpu, FLAG_N));
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, carry);
    cpu->A = a;
}

void complement_acc(CPU* cpu) {            
    cpu->A = ~cpu->A;
    set_flag(cpu, FLAG_N, true);
    set_flag(cpu, FLAG_H, true);
}

void set_carry_cy(CPU* cpu) { // Set carry flag
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, true);
}

void flip_cy(CPU* cpu) { // complement of cy
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, !get_flag(cpu, FLAG_C));
}

void ld_a16_sp(CPU* cpu){
    u8 lowerByte = memory_read(cpu->pc++);
    u8 upperByte = memory_read(cpu->pc++);
    u16 addr = (upperByte << 8) | lowerByte;
    memory_write(addr, cpu->sp & 0xFF);
    memory_write(addr + 1, (cpu->sp >> 8) & 0xFF);
}

void lda_reg_hl_inc(CPU* cpu){ // 0x22
    u16 address = get_hl(cpu);
    memory_write(address, cpu->A);
    set_hl(cpu, address + 1) ;
}

void lda_reg_hl_dec(CPU* cpu){ // 0x32
    u16 address = get_hl(cpu);
    memory_write(address, cpu->A);
    set_hl(cpu, address - 1);
}

void lda_hl_reg_inc(CPU* cpu){ // 0x2A
    u16 address = get_hl(cpu);
    cpu->A = memory_read(address);
    set_hl(cpu, address+1);

}

void lda_hl_reg_dec(CPU* cpu){ // 0x3A
    u16 address = get_hl(cpu);
    cpu->A = memory_read(address);
    set_hl(cpu, address-1);
}

void add_reg(CPU* cpu, u8* dest_reg, u8 src) {
    u16 result = *dest_reg + src;
    
    set_flag(cpu, FLAG_Z, (result & 0xFF) == 0);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, ((*dest_reg & 0xF) + (src & 0xF)) > 0xF);
    set_flag(cpu, FLAG_C, result > 0xFF);
    
    *dest_reg = result & 0xFF;
}

void add_reg_carry(CPU* cpu, u8* dest_reg, u8 src){
    u8 carry = get_flag(cpu, FLAG_C) ? 1 : 0; // Set carry to 1 if true, 0 if false
    u16 result = *dest_reg + src + carry;

    set_flag(cpu, FLAG_Z, (result & 0xFF) == 0);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, ((*dest_reg & 0xF) + (src & 0xF) + carry) > 0xF);
    set_flag(cpu, FLAG_C, result > 0xFF);

    *dest_reg = (u8)(result & 0xFF);

}

void sub_reg(CPU* cpu, u8* dest_reg, u8 src){
    u8 val = *dest_reg;
    u16 result = val - src;
    set_flag(cpu, FLAG_Z, (result & 0xFF) == 0);
    set_flag(cpu, FLAG_N, true);
    set_flag(cpu, FLAG_H, (val & 0xF) < (src & 0xF)); // If lower nibble of src > dest_reg, it must have carried
    set_flag(cpu, FLAG_C, (val < src));

    *dest_reg = (u8)(result & 0xFF);

}

void sub_reg_carry(CPU* cpu, u8* dest_reg, u8 src) {
    u8 carry = get_flag(cpu, FLAG_C) ? 1 : 0; // Check if there is a carry
    u8 val = *dest_reg;
    
    int result = val - src - carry;
    set_flag(cpu, FLAG_Z, (result & 0xFF) == 0);
    set_flag(cpu, FLAG_N, true);
    set_flag(cpu, FLAG_H, ((val & 0xF) - (src & 0xF) - carry) < 0);
    set_flag(cpu, FLAG_C, result < 0);

    *dest_reg = (u8)(result & 0xFF);
}

void and_reg(CPU* cpu, u8* dest_reg, u8 src){
    *dest_reg &= src;

    set_flag(cpu, FLAG_Z,*dest_reg == 0);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, true);
    set_flag(cpu, FLAG_C, false);

}

void xor_reg(CPU* cpu, u8* dest_reg, u8 src){
    *dest_reg ^= src;

    set_flag(cpu, FLAG_Z, *dest_reg == 0);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, false);
}

void or_reg(CPU* cpu, u8* dest_reg, u8 src){
    *dest_reg |= src;

    set_flag(cpu, FLAG_Z, *dest_reg == 0);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, false);
}

void cp_reg(CPU* cpu, u8* dest_reg, u8 src){
    u8 val = *dest_reg;
    
    u16 result = val - src;

    set_flag(cpu, FLAG_Z, (result & 0xFF) == 0);
    set_flag(cpu, FLAG_N, true);
    set_flag(cpu, FLAG_H, (val & 0xF) < (src & 0xF));
    set_flag(cpu, FLAG_C, val < src);

    // Contents of the acc don't get updated
    // Don't update dest_reg
}

void push(CPU* cpu, u16 val){ // PUSH to, and POP from Stack
    cpu->sp--;
    memory_write(cpu->sp, (val >>8) & 0xFF); // Write Upper Byte 
    cpu->sp--;
    memory_write(cpu->sp, val &0xFF); // Write Lower Byte
}

u16 pop(CPU* cpu){
    u8 lowerByte = memory_read(cpu->sp++);
    u8 upperByte = memory_read(cpu->sp++);
    return (upperByte << 8) | lowerByte;
}

// PUSH PC to Stack and JUMP
void call(CPU* cpu){
    u8 lowerByte = memory_read(cpu->pc++);
    u8 upperByte = memory_read(cpu->pc++);
    u16 addr = (upperByte << 8) | lowerByte;
    push(cpu, cpu->pc);  
    cpu->pc = addr;
}

void ret(CPU* cpu){ // POP PC from stack
    cpu->pc = pop(cpu);
}

void jp(CPU* cpu){ // JUMP
    u8 lowerByte = memory_read(cpu->pc++);
    u8 upperByte = memory_read(cpu->pc++);
    cpu->pc = (upperByte << 8) | lowerByte;
}

void rst(CPU* cpu, u8 addr){ // Restart?
    push(cpu, cpu->pc);
    cpu->pc = addr;
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

void set_sp(CPU* cpu, u16 val){
    cpu->sp = val;
}


// -------------------- 16-bit (CB) Instruction Helper Functions --------------------

// Normal Register Helpers

void rl_reg(CPU* cpu, u8* reg){ // Rotate Left
    u8 carry = get_flag(cpu, FLAG_C) ? 1: 0; // Check for carry
    u8 bit7 = (*reg >> 7) & 1; // Bitwise extraction of Bit 7
    *reg = (*reg << 1) | carry;
    set_flag(cpu, FLAG_Z, *reg == 0);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, bit7);

}

void rr_reg(CPU* cpu, u8* reg){ // Rotate Right
    u8 carry = get_flag(cpu, FLAG_C) ? 1 : 0; // Check for carry
    u8 bit0 = *reg & 1; // Extract bit0
    *reg = (*reg >> 1) | (carry << 7);
    set_flag(cpu, FLAG_Z, *reg == 0);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, bit0);
}

void rlc_reg(CPU* cpu, u8* reg){ // Rotate Left Circular
    u8 bit7 = (*reg >> 7) & 1;
    *reg = (*reg << 1) | bit7;
    set_flag(cpu, FLAG_Z, *reg == 0);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, bit7);
}

void rrc_reg(CPU* cpu, u8* reg){ // Rotate Right Circular
    u8 bit0 = *reg & 1;
    *reg = (*reg >> 1) | (bit0 << 7);
    set_flag(cpu, FLAG_Z, *reg == 0);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, bit0);

}

void sla_reg (CPU* cpu, u8* reg){ // Shift Left Arithmetic
    u8 bit7 = (*reg >> 7) & 1;
    *reg = *reg << 1;
    set_flag(cpu, FLAG_Z, *reg == 0);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, bit7);

}

void sra_reg (CPU* cpu, u8* reg){ // Shift Right Arithmetic
    u8 bit0 = *reg & 1;
    u8 bit7 = *reg & 0x80;
    *reg = (*reg >> 1) | bit7;
    set_flag(cpu, FLAG_Z, *reg == 0);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, bit0);

}

void srl_reg(CPU* cpu, u8* reg){ // Shift Right Logical
    u8 bit0 = *reg & 1;
    *reg = *reg >> 1;
    set_flag(cpu, FLAG_Z, *reg == 0);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, bit0);

}

void swap_reg(CPU* cpu, u8* reg){ // Swap Upper Nbble and Lower Nibble
    *reg = ((*reg & 0x0F) << 4) | ((*reg & 0xF0) >> 4);
    set_flag(cpu, FLAG_Z, *reg == 0);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, false);
    set_flag(cpu, FLAG_C, false);

}

void cpy_bit_reg(CPU* cpu, u8 bit, u8 reg){ // Sets Z depending on bit N of Reg

    set_flag(cpu, FLAG_Z, (reg & (1 << bit)) == 0);
    set_flag(cpu, FLAG_N, false);
    set_flag(cpu, FLAG_H, true);
    // CARRY not affected
}

void rst_bit_reg(CPU* cpu, u8 bit, u8* reg){ // Clear Bit N
    *reg &= ~(1 << bit);
    // FLAGS not affected
}

void set_bit_reg(CPU* cpu, u8 bit, u8* reg){ // Set Bit N

    *reg |= (1 << bit);
    // FLAGS not affected

}

// HL Pair Register Helpers

void rlc_hl(CPU* cpu) {
    u16 addr = get_hl(cpu);
    u8 val = memory_read(addr);
    rlc_reg(cpu, & val);
    memory_write(addr, val);
}

void rrc_hl(CPU* cpu) {
    u16 addr = get_hl(cpu);
    u8 val = memory_read(addr);
    rrc_reg(cpu, &val);
    memory_write(addr, val);
}

void rl_hl(CPU* cpu) {
    u16 addr = get_hl(cpu);
    u8 val = memory_read(addr);
    rl_reg(cpu, &val);
    memory_write(addr, val);
}

void rr_hl(CPU* cpu) {
    u16 addr = get_hl(cpu);
    u8 val = memory_read(addr);
    rr_reg(cpu, &val);
    memory_write(addr, val);
}

void sla_hl(CPU* cpu) {
    u16 addr = get_hl(cpu);
    u8 val = memory_read(addr);
    sla_reg(cpu, &val);
    memory_write(addr, val);
}

void sra_hl(CPU* cpu) {
    u16 addr = get_hl(cpu);
    u8 val = memory_read(addr);
    sra_reg(cpu, &val);
    memory_write(addr, val);
}

void srl_hl(CPU* cpu) {
    u16 addr = get_hl(cpu);
    u8 val = memory_read(addr);
    srl_reg(cpu, &val);
    memory_write(addr, val);
}

void swap_hl(CPU* cpu) {
    u16 addr = get_hl(cpu);
    u8 val = memory_read(addr);
    swap_reg(cpu, &val);
    memory_write(addr, val);
}

void rst_bit_hl(CPU* cpu, u8 bit){
    u16 addr = get_hl(cpu);
    u8 val = memory_read(addr);
    rst_bit_reg(cpu, bit, &val);
    memory_write(addr, val);
}

void set_bit_hl(CPU* cpu, u8 bit) {
    u16 addr = get_hl(cpu);
    u8 val = memory_read(addr);
    set_bit_reg(cpu, bit, &val);
    memory_write(addr, val);
}

// Timer Handler:

void cpu_update_timer(CPU* cpu, u8 cycles){

    cpu->div += cycles;
    if (cpu->div >= 256) { // If > 256 cycles elapse, DIV reg increments
        cpu->div -= 256; // Accounting for overflow
        u8 div = memory_read(0xFF04);
        memory_write(0xFF04, div + 1);
    }

    // Need to update frequency based on whether the timer is enabled

    u8 timer_control_tac = memory_read(0xFF07);
    if (timer_control_tac & 0x04){
        cpu->timer_counter += cycles;

        u16 threshold_frequency;
        switch (timer_control_tac & 0x03){
            case 0:
                threshold_frequency = 1024; // 4096 Hz
                break;
            
            case 1:
                threshold_frequency = 16; // 262144 Hz
                break;

            case 2:
                threshold_frequency = 64; // 65536 Hz
                break;

            case 3:
                threshold_frequency = 256; // 16384 Hz
                break;
        }

        // After executing long instructions, TIMA may need multiple increments
        while(cpu->timer_counter >= threshold_frequency){
            cpu->timer_counter -= threshold_frequency;

            u8 tima_val = memory_read(0xFF05);

            if (tima_val == 0xFF){
                 // Overflow: Reset TMA and Interrupt Request
                u8 timer_modulo_tma = memory_read(0xFF06);
                memory_write(0xFF05, timer_modulo_tma);
                
                u8 if_flag = memory_read(0xFF0F);
                memory_write(0xFF0F, if_flag | 0x04);
            }

            else{
                memory_write(0xFF05, tima_val + 1); // Increment
            }
        }
    }

    
}

static inline void cpu_tick(CPU* cpu, PPU* ppu, u8 tcycles) {
    cpu->cycles += tcycles;
    cpu_update_timer(cpu, tcycles);
    ppu_step(ppu, tcycles);
}

// Interrupts handler:

void cpu_interrupt_handler(CPU* cpu){
    u8 ie = memory_read(0xFFFF); //interrupt enable
    u8 interrupt_flag = memory_read(0xFF0F); // interrupt flag

    u8 pending_interrupt = (ie & interrupt_flag);

    if (pending_interrupt == 0){
        return;
    }

    if (cpu->isHalted){
        cpu->isHalted = false;
    }

    if (!cpu->ime){
        return;
    }

    cpu->ime = false;

    // Interrupt Priority is highest at Bit 0, Lowest at Bit 4

    if (pending_interrupt & (0x01)){ // VBlank bit 0
        memory_write(0xFF0F, interrupt_flag & ~(0x01)); // Clear IF
        push(cpu, cpu->pc);
        cpu->pc = 0x0040; // Jump to Vblank Handler
        cpu->cycles += 20;
    } 

    else if (pending_interrupt & (0x02)){ // LCD bit 1
        memory_write(0xFF0F, interrupt_flag & ~(0x02)); // Clear IF
        push(cpu, cpu->pc);
        cpu->pc = 0x0048; // JUMP to LCD Handler
        cpu->cycles += 20;
    } 

    else if (pending_interrupt & (0x04)){ // Timer bit 2
        memory_write(0xFF0F, interrupt_flag & ~(0x04)); // Clear IF
        push(cpu, cpu->pc);
        cpu->pc = 0x0050; // JUMP to TIMER Handler
        cpu->cycles += 20;
    } 

    else if (pending_interrupt & (0x08)){ // Serial bit 3
        memory_write(0xFF0F, interrupt_flag & ~(0x08)); // Clear IF
        push(cpu, cpu->pc);
        cpu->pc = 0x0058; // JUMP to Serial Handler
        cpu->cycles += 20;
    } 

    else if (pending_interrupt & (0x10)){ // Joypad bit 4
        memory_write(0xFF0F, interrupt_flag & ~(0x10)); // Clear IF
        push(cpu, cpu->pc);
        cpu->pc = 0x0060; // JUMP to Joypad Handler
        cpu->cycles += 20;
    } 

     
    //

}
