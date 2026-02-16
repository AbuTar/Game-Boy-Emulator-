#include "cpu.h"
#include "memory.h"

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

    if (!get_flag(cpu, FLAG_N)) { // After addition
        if (get_flag(cpu, FLAG_H) || (a & 0xF) > 9)  a += 0x06;
        if (get_flag(cpu, FLAG_C) || a > 0x99) { a += 0x60; set_flag(cpu, FLAG_C, true); }

    } 
    
    else {  // After subtraction
        if (get_flag(cpu, FLAG_H)) a -= 0x06;
        if (get_flag(cpu, FLAG_C)) a -= 0x60;
    }
    set_flag(cpu, FLAG_Z, a == 0);
    set_flag(cpu, FLAG_H, false);
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