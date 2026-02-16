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
            ld_pair_imm(cpu, set_bc); // Immediate to BC
            cpu->cycles += 12;
            break;
            
        case 0x02:
            ld_reg_pair(cpu, get_bc(cpu), cpu-> A ); // A to HL
            cpu->cycles += 8;
            break;

        case 0x03:
            inc_pair_val(cpu, get_bc, set_bc); // Increment BC
            cpu->cycles += 8;
            break;

        case 0x04:
            inc_reg(cpu, &cpu->B); // Increment B
            cpu->cycles += 4;
            break;

        case 0x05:
            dec_reg(cpu, &cpu->B); // Decrement B
            cpu->cycles += 4;
            break;

        case 0x06:
            ld_reg_imm(cpu, &cpu->B); // Immediate to B 
            cpu->cycles += 8;
            break;

        case 0x07:
            rlca(cpu); // Rotate ACC Left
            cpu->cycles += 4;
            break;

        case 0x08:
            ld_a16_sp(cpu); // 
            cpu->cycles += 20;
            break;

        case 0x09:
            add_hl_pair(cpu, get_bc(cpu)); // Add BC to HL
            cpu->cycles += 8;
            break;

        case 0x0A:
            ld_pair_reg(cpu, &cpu->A, get_bc(cpu)); // BC to A
            cpu->cycles += 8;
            break;

        case 0x0B:
            dec_pair_val(cpu, get_bc, set_bc); // Decrement BC
            cpu->cycles += 8;
            break;

        case 0x0C:
            inc_reg(cpu, &cpu->C); // Increment C
            cpu->cycles += 4;
            break;

        case 0x0D:
            dec_reg(cpu, &cpu->C); // Decrement C
            cpu->cycles += 4;
            break;

        case 0x0E:
            ld_reg_imm(cpu, &cpu->C); // Immediate to C
            cpu->cycles += 8;
            break;

        case 0x0F:
            rrca(cpu); // Rotate ACC Right
            cpu->cycles += 4;
            break;

        // ==================== 0x10 - 0x1F ====================

        case 0x10: // STOP
            cpu->isHalted = true;
            cpu->pc++; // STOP instruction is 2 Bytes
            cpu->cycles += 4;
            break;

        case 0x11:
            ld_pair_imm(cpu, set_de); // Immediate to DE
            cpu->cycles += 12;
            break;
            
        case 0x12:
            ld_reg_pair(cpu, get_de(cpu), cpu-> A ); // DE to A
            cpu->cycles += 8;
            break;

        case 0x13:
            inc_pair_val(cpu, get_de, set_de); // Increment DE
            cpu->cycles += 8;
            break;

        case 0x14:
            inc_reg(cpu, &cpu->D); // Increment D
            cpu->cycles += 4;
            break;

        case 0x15:
            dec_reg(cpu, &cpu->D); // Decrement D
            cpu->cycles += 4;
            break;

        case 0x16:
            ld_reg_imm(cpu, &cpu->D); // Immediate to D
            cpu->cycles += 8;
            break;

        case 0x17:
            rla(cpu); // RLA
            cpu->cycles += 4;
            break;

        case 0x18:
            jr(cpu);
            cpu->cycles += 12; // Jr
            break;

        case 0x19:
            add_hl_pair(cpu, get_de(cpu)); // Add DE to HL
            cpu->cycles += 8;
            break;

        case 0x1A:
            ld_pair_reg(cpu, &cpu->A, get_de(cpu)); // DE to A
            cpu->cycles += 8;
            break;

        case 0x1B:
            dec_pair_val(cpu, get_de, set_de); // Decrement DE
            cpu->cycles += 8;
            break;

        case 0x1C:
            inc_reg(cpu, &cpu->E); // Increment E
            cpu->cycles += 4;
            break;

        case 0x1D:
            dec_reg(cpu, &cpu->E); // Decrement E
            cpu->cycles += 4;
            break;

        case 0x1E:
            ld_reg_imm(cpu, &cpu->E);
            cpu->cycles += 8; // Immediate to E
            break;

        case 0x1F:
            rra(cpu); // RRA
            cpu->cycles += 4;
            break;
        // ==================== 0x20 - 0x2F ====================

        case 0x20: 
            if (!get_flag(cpu, FLAG_Z)) {
                jr(cpu);
                cpu->cycles += 12;  // Branch taken
            }
             
            else {
                    cpu->pc++;           // Skip offset byte
                    cpu->cycles += 8;   // Branch not taken
            }

            break;
            
        case 0x21:
            ld_pair_imm(cpu, set_hl); // Immediate to HL
            cpu->cycles += 12;
            break;

        case 0x22:
            lda_reg_hl_inc(cpu); // A to HL and Increment HL
            cpu->cycles += 8;
            break;
        case 0x23:
            break;
        case 0x24:
            break;
        case 0x25:
            break;
        case 0x26:
            break;
        case 0x27:
            break;
        case 0x28:
            break;
        case 0x29:
            break;
        case 0x2A:
            lda_hl_reg_inc(cpu);
            cpu->cycles += 8;
            break;
        case 0x2B:
            break;
        case 0x2C:
            break;
        case 0x2D:
            break;
        case 0x2E:
            break;
        case 0x2F:
            break;
        // ==================== 0x30 - 0x3F ====================
        case 0x30: 
            break;
        case 0x31:
            ld_pair_imm(cpu, set_sp);
            cpu->cycles += 12;

        case 0x32:
            lda_reg_hl_dec(cpu);
            cpu->cycles += 8;
            break;

        case 0x33:
            break;
        case 0x34:
            break;
        case 0x35:
            break;
        case 0x36:
            break;
        case 0x37:
            break;
        case 0x38:
            break;
        case 0x39:
            break;
        case 0x3A:
            lda_reg_hl_dec(cpu);
            cpu->cycles += 8;
            break;

        case 0x3B:
            break;
        case 0x3C:
            break;
        case 0x3D:
            break;
        case 0x3E:
            break;
        case 0x3F:
            break;

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
            cpu->cycles += 8;
            break;

        case 0x4F:
            ld_reg_reg(cpu, &cpu->C, cpu->A); // A to C
            cpu->cycles += 4;
            break;

        // ==================== 0x50 - 0x5F ====================

        case 0x50:
            ld_reg_reg(cpu, &cpu->D, cpu->B); // B to D
            cpu->cycles += 4;
            break;
        
        case 0x51:
            ld_reg_reg(cpu, &cpu->D, cpu->C); // C to D
            cpu->cycles += 4;
            break;
        
        case 0x52:
            ld_reg_reg(cpu, &cpu->D, cpu->D); // D to D
            cpu->cycles += 4;
            break;

        case 0x53:
            ld_reg_reg(cpu, &cpu->D, cpu->E); // E to D
            cpu->cycles += 4;
            break;

        case 0x54:
            ld_reg_reg(cpu, &cpu->D, cpu->H); // H to D
            cpu->cycles += 4;
            break;

        case 0x55:
            ld_reg_reg(cpu, &cpu->D, cpu->L); // L to D
            cpu->cycles += 4;
            break;

        case 0x56:
            ld_pair_reg(cpu, &cpu->D, get_hl(cpu)); // HL to D
            cpu->cycles += 8;
            break;

        case 0x57:
            ld_reg_reg(cpu, &cpu->D, cpu->A); // A to D
            cpu->cycles += 4;
            break;

        case 0x58:
            ld_reg_reg(cpu, &cpu->E, cpu->B); // B to E
            cpu->cycles += 4;
            break;

        case 0x59:
            ld_reg_reg(cpu, &cpu->E, cpu->C); // C to E
            cpu->cycles += 4;
            break;

        case 0x5A:
            ld_reg_reg(cpu, &cpu->E, cpu->D); // D to E
            cpu->cycles += 4;
            break;

        case 0x5B:
            ld_reg_reg(cpu, &cpu->E, cpu->E); // E to E
            cpu->cycles += 4;
            break;

        case 0x5C:
            ld_reg_reg(cpu, &cpu->E, cpu->H); // H to E
            cpu->cycles += 4;
            break;

        case 0x5D:
            ld_reg_reg(cpu, &cpu->E, cpu->L); // L to E
            cpu->cycles += 4;
            break;

        case 0x5E:
            ld_pair_reg(cpu, &cpu->E, get_hl(cpu)); // HL to E
            cpu->cycles += 8;
            break;
        
        case 0x5F:
            ld_reg_reg(cpu, &cpu->E, cpu->A); // A to E
            cpu->cycles += 4;
            break;


        // ==================== 0x60 - 0x6F ====================

        case 0x60:
            ld_reg_reg(cpu, &cpu->H, cpu->B); // B to H
            cpu->cycles += 4;
            break;
        
        case 0x61:
            ld_reg_reg(cpu, &cpu->H, cpu->C); // C to H
            cpu->cycles += 4;
            break;
        
        case 0x62:
            ld_reg_reg(cpu, &cpu->H, cpu->D); // D to H
            cpu->cycles += 4;
            break;

        case 0x63:
            ld_reg_reg(cpu, &cpu->H, cpu->E); // E to H
            cpu->cycles += 4;
            break;

        case 0x64:
            ld_reg_reg(cpu, &cpu->H, cpu->H); // H to H
            cpu->cycles += 4;
            break;

        case 0x65:
            ld_reg_reg(cpu, &cpu->H, cpu->L); // L to H
            cpu->cycles += 4;
            break;

        case 0x66:
            ld_pair_reg(cpu, &cpu->H, get_hl(cpu)); // HL to H
            cpu->cycles += 8;
            break;

        case 0x67:
            ld_reg_reg(cpu, &cpu->H, cpu->A); // A to H
            cpu->cycles += 4;
            break;

        case 0x68:
            ld_reg_reg(cpu, &cpu->L, cpu->B); // B to L
            cpu->cycles += 4;
            break;

        case 0x69:
            ld_reg_reg(cpu, &cpu->L, cpu->C); // C to L
            cpu->cycles += 4;
            break;

        case 0x6A:
            ld_reg_reg(cpu, &cpu->L, cpu->D); // D to L
            cpu->cycles += 4;
            break;

        case 0x6B:
            ld_reg_reg(cpu, &cpu->L, cpu->E); // E to L
            cpu->cycles += 4;
            break;

        case 0x6C:
            ld_reg_reg(cpu, &cpu->L, cpu->H); // H to L
            cpu->cycles += 4;
            break;

        case 0x6D:
            ld_reg_reg(cpu, &cpu->L, cpu->L); // L to L
            cpu->cycles += 4;
            break;

        case 0x6E:
            ld_pair_reg(cpu, &cpu->L, get_hl(cpu)); // HL to L
            cpu->cycles += 8;
            break;
        
        case 0x6F:
            ld_reg_reg(cpu, &cpu->L, cpu->A); // A to L
            cpu->cycles += 4;
            break;
        
            
        // ==================== 0x70 - 0x7F ====================

        case 0x70:
            ld_reg_pair(cpu, get_hl(cpu), cpu->B); // B to HL
            cpu->cycles += 8;
            break;
        
        case 0x71:
            ld_reg_pair(cpu, get_hl(cpu), cpu->C); // C to HL
            cpu->cycles += 8;
            break;
        
        case 0x72:
            ld_reg_pair(cpu, get_hl(cpu), cpu->D); // D to HL
            cpu->cycles += 8;
            break;

        case 0x73:
            ld_reg_pair(cpu, get_hl(cpu), cpu->E); // E to HL
            cpu->cycles += 8;
            break;

        case 0x74:
            ld_reg_pair(cpu, get_hl(cpu), cpu->H); // H to HL
            cpu->cycles += 8;
            break;

        case 0x75:
            ld_reg_pair(cpu, get_hl(cpu), cpu->L); // L to HL
            cpu->cycles += 8;
            break;

        case 0x76:
        // Need to implement fix for famous HLAT bug
            cpu->isHalted = true; // HALT CPU
            cpu->cycles += 4;
            break;

        case 0x77:
            ld_reg_pair(cpu, get_hl(cpu), cpu->A); // A to HL
            cpu->cycles += 8;
            break;

        case 0x78:
            ld_reg_reg(cpu, &cpu->A, cpu->B); // B to A
            cpu->cycles += 4;
            break;

        case 0x79:
            ld_reg_reg(cpu, &cpu->A, cpu->C); // C to A
            cpu->cycles += 4;
            break;

        case 0x7A:
            ld_reg_reg(cpu, &cpu->A, cpu->D); // D to A
            cpu->cycles += 4;
            break;

        case 0x7B:
            ld_reg_reg(cpu, &cpu->A, cpu->E); // E to A
            cpu->cycles += 4;
            break;

        case 0x7C:
            ld_reg_reg(cpu, &cpu->A, cpu->H); // H to A
            cpu->cycles += 4;
            break;

        case 0x7D:
            ld_reg_reg(cpu, &cpu->A, cpu->L); // L to A
            cpu->cycles += 4;
            break;

        case 0x7E:
            ld_pair_reg(cpu, &cpu->A, get_hl(cpu)); // HL to A
            cpu->cycles += 8;
            break;
        
        case 0x7F:
            ld_reg_reg(cpu, &cpu->A, cpu->A); // A to A
            cpu->cycles += 4;
            break;

        // ==================== 0x80 - 0x8F ====================

        case 0x80: 
            add_reg(cpu, &cpu->A, cpu->B); // Add B to A
            cpu->cycles += 4; 
            break;

        case 0x81:
            add_reg(cpu, &cpu->A, cpu->C); // Add C to A
            cpu->cycles += 4; 
            break;

        case 0x82:
            add_reg(cpu, &cpu->A, cpu->D); // Add D to A
            cpu->cycles += 4; 
            break;
            
        case 0x83:
            add_reg(cpu, &cpu->A, cpu->E); // Add E to A
            cpu->cycles += 4; 
            break;
            
        case 0x84:
            add_reg(cpu, &cpu->A, cpu->H); // Add H to A
            cpu->cycles += 4; 
            break;
            
        case 0x85:
            add_reg(cpu, &cpu->A, cpu->L); // Add L to A
            cpu->cycles += 4; 
            break;
            
        case 0x86:
            add_reg(cpu, &cpu->A, memory_read(get_hl(cpu))); // Add HL to A
            cpu->cycles += 8; 
            break;
            
        case 0x87:
            add_reg(cpu, &cpu->A, cpu->A); // Add A to A  
            cpu->cycles += 4; 
            break;
            
        case 0x88:
            add_reg_carry(cpu, &cpu->A, cpu->B); // Add B to A w Carry
            cpu->cycles += 4; 
            break;
            
        case 0x89:
            add_reg_carry(cpu, &cpu->A, cpu->C); // Add C to A w Carry
            cpu->cycles += 4; 
            break;
            
        case 0x8A:
            add_reg_carry(cpu, &cpu->A, cpu->D); // Add D to A w Carry
            cpu->cycles += 4; 
            break;
            
        case 0x8B:
            add_reg_carry(cpu, &cpu->A, cpu->E); // Add E to A w Carry
            cpu->cycles += 4; 
            break;
            
        case 0x8C:
            add_reg_carry(cpu, &cpu->A, cpu->H); // Add H to A w Carry
            cpu->cycles += 4;
            break;
            
        case 0x8D:
            add_reg_carry(cpu, &cpu->A, cpu->L); // Add L to A w Carry
            cpu->cycles += 4;
            break;
            
        case 0x8E:
            add_reg_carry(cpu, &cpu->A, memory_read(get_hl(cpu))); // Add HL to A w Carry
            cpu->cycles += 8; 
            break;
            
        case 0x8F:
            add_reg_carry(cpu, &cpu->A, cpu->A); // Add A to A w Carry
            cpu->cycles += 4;
            break;
            

        // ==================== 0x90 - 0x9F ====================

        case 0x90:
            sub_reg(cpu, &cpu->A, cpu->B);  // Sub B from A
            cpu->cycles += 4;
            break;

        case 0x91:
            sub_reg(cpu, &cpu->A, cpu->C); // Sub C from A
            cpu->cycles += 4;
            break;

        case 0x92:
            sub_reg(cpu, &cpu->A, cpu->D); // Sub D from A
            cpu->cycles += 4;
            break;

        case 0x93:
            sub_reg(cpu, &cpu->A, cpu->E); // Sub E from A 
            cpu->cycles += 4;
            break;

        case 0x94:
            sub_reg(cpu, &cpu->A, cpu->H); // Sub H from A
            cpu->cycles += 4;
            break;

        case 0x95:
            sub_reg(cpu, &cpu->A, cpu->L); // Sub L from A
            cpu->cycles += 4;
            break;

        case 0x96:
            sub_reg(cpu, &cpu->A, memory_read(get_hl(cpu))); // Sub HL from A
            cpu->cycles += 8;
            break;

        case 0x97:
            sub_reg(cpu, &cpu->A, cpu->A); // Sub A from A
            cpu->cycles += 4;
            break;

        case 0x98:
            sub_reg_carry(cpu, &cpu->A, cpu->B); // Sub B from A w Carry
            cpu->cycles += 4;
            break;

        case 0x99:
            sub_reg_carry(cpu, &cpu->A, cpu->C); // Sub C from A w Carry
            cpu->cycles += 4;
            break;

        case 0x9A:
            sub_reg_carry(cpu, &cpu->A, cpu->D); // Sub D from A w Carry
            cpu->cycles += 4;
            break;

        case 0x9B:
            sub_reg_carry(cpu, &cpu->A, cpu->E); // Sub E from A w Carry
            cpu->cycles += 4;
            break;

        case 0x9C:
            sub_reg_carry(cpu, &cpu->A, cpu->H); // Sub H from A w Carry
            cpu->cycles += 4;
            break;

        case 0x9D:
            sub_reg_carry(cpu, &cpu->A, cpu->L); // Sub L from A w Carry
            cpu->cycles += 4;
            break;

        case 0x9E:
            sub_reg_carry(cpu, &cpu->A, memory_read(get_hl(cpu))); // Sub HL from A w Carry
            cpu->cycles += 8;
            break;

        case 0x9F:
            sub_reg_carry(cpu, &cpu->A, cpu->A); // Sub A from A w Carry
            cpu->cycles += 4;
            break;

        // ==================== 0xA0 - 0xAF ====================

        case 0xA0: 
            and_reg(cpu, &cpu->A, cpu->B); // A . B
            cpu->cycles += 4 ;
            break;

        case 0xA1:
            and_reg(cpu, &cpu->A, cpu->C); // A . C
            cpu->cycles += 4 ;
            break;

        case 0xA2:
            and_reg(cpu, &cpu->A, cpu->D); // A . D
            cpu->cycles += 4 ;
            break;

        case 0xA3:
            and_reg(cpu, &cpu->A, cpu->E); // A . E
            cpu->cycles += 4 ;
            break;

        case 0xA4:
            and_reg(cpu, &cpu->A, cpu->H); // A . H
            cpu->cycles += 4 ;
            break;

        case 0xA5:
            and_reg(cpu, &cpu->A, cpu->L); // A . L
            cpu->cycles += 4 ;
            break;

        case 0xA6:
            and_reg(cpu, &cpu->A, memory_read(get_hl(cpu))); // A . HL
            cpu->cycles += 8 ;
            break;

        case 0xA7:
            and_reg(cpu, &cpu->A, cpu->A); // A . A
            cpu->cycles += 4 ;
            break;

        case 0xA8:
            xor_reg(cpu, &cpu->A, cpu->B); // A XOR B
            cpu->cycles += 4 ;
            break;

        case 0xA9:
            xor_reg(cpu, &cpu->A, cpu->C); // A XOR C
            cpu->cycles += 4 ;
            break;

        case 0xAA:
            xor_reg(cpu, &cpu->A, cpu->D); // A XOR D
            cpu->cycles += 4 ;
            break;

        case 0xAB:
            xor_reg(cpu, &cpu->A, cpu->E); // A XOR E
            cpu->cycles += 4 ;
            break;

        case 0xAC:
            xor_reg(cpu, &cpu->A, cpu->H); // A XOR H
            cpu->cycles += 4 ;
            break;

        case 0xAD:
            xor_reg(cpu, &cpu->A, cpu->L); // A XOR L
            cpu->cycles += 4 ;
            break;

        case 0xAE:
            xor_reg(cpu, &cpu->A, memory_read(get_hl(cpu))); // A XOR HL
            cpu->cycles += 8 ;
            break;

        case 0xAF:
            xor_reg(cpu, &cpu->A, cpu->A); // A XOR A
            cpu->cycles += 4 ;
            break;


        // ==================== 0xB0 - 0xBF ====================

        case 0xB0:
            or_reg(cpu, &cpu->A, cpu->B); // A OR B
            cpu->cycles += 4;
            break;

        case 0xB1:
            or_reg(cpu, &cpu->A, cpu->C); // A OR C
            cpu->cycles += 4;
            break;

        case 0xB2:
            or_reg(cpu, &cpu->A, cpu->D); // A OR D
            cpu->cycles += 4;
            break;

        case 0xB3:
            or_reg(cpu, &cpu->A, cpu->E); // A OR E
            cpu->cycles += 4;
            break;

        case 0xB4:
            or_reg(cpu, &cpu->A, cpu->H); // A OR H
            cpu->cycles += 4;
            break;

        case 0xB5:
            or_reg(cpu, &cpu->A, cpu->L); // A OR L
            cpu->cycles += 4;
            break;

        case 0xB6:
            or_reg(cpu, &cpu->A, memory_read(get_hl(cpu))); // A OR HL
            cpu->cycles += 8;
            break;

        case 0xB7:
            or_reg(cpu, &cpu->A, cpu->A); // A OR A
            cpu->cycles += 4;
            break;

        case 0xB8:
            cp_reg(cpu, &cpu->A, cpu->B); // COPY B to A
            cpu->cycles += 4;
            break;

        case 0xB9:
            cp_reg(cpu, &cpu->A, cpu->C); // COPY C to A
            cpu->cycles += 4;
            break;

        case 0xBA:
            cp_reg(cpu, &cpu->A, cpu->D); // COPY D to A
            cpu->cycles += 4;
            break;

        case 0xBB:
            cp_reg(cpu, &cpu->A, cpu->E); // COPY E to A
            cpu->cycles += 4;
            break;

        case 0xBC:
            cp_reg(cpu, &cpu->A, cpu->H); // COPY H to A
            cpu->cycles += 4;
            break;

        case 0xBD:
            cp_reg(cpu, &cpu->A, cpu->L); // COPY L to A
            cpu->cycles += 4;
            break;

        case 0xBE:
            cp_reg(cpu, &cpu->A, memory_read(get_hl(cpu))); // COPY HL to A
            cpu->cycles += 8;
            break;

        case 0xBF:
            cp_reg(cpu, &cpu->A, cpu->A); // COPY A to A
            cpu->cycles += 4;
            break;


        // ==================== 0xC0 - 0xCF ====================

        case 0xC0: 
            break;
        case 0xC1:
            break;
        case 0xC2:
            break;
        case 0xC3:
            break;
        case 0xC4:
            break;
        case 0xC5:
            break;
        case 0xC6:
            break;
        case 0xC7:
            break;
        case 0xC8:
            break;
        case 0xC9:
            break;
        case 0xCA:
            break;
        case 0xCB:
            break;
        case 0xCC:
            break;
        case 0xCD:
            break;
        case 0xCE:
            break;
        case 0xCF:
            break;

        // ==================== 0xD0 - 0xDF ====================

        case 0xD0: 
            break;
        case 0xD1:
            break;
        case 0xD2:
            break;
        case 0xD3:
            break;
        case 0xD4:
            break;
        case 0xD5:
            break;
        case 0xD6:
            break;
        case 0xD7:
            break;
        case 0xD8:
            break;
        case 0xD9:
            break;
        case 0xDA:
            break;
        case 0xDB:
            break;
        case 0xDC:
            break;
        case 0xDD:
            break;
        case 0xDE:
            break;
        case 0xDF:
            break;
        // ==================== 0xE0 - 0xEF ====================

        case 0xE0: 
            break;
        case 0xE1:
            break;
        case 0xE2:
            break;
        case 0xE3:
            break;
        case 0xE4:
            break;
        case 0xE5:
            break;
        case 0xE6:
            break;
        case 0xE7:
            break;
        case 0xE8:
            break;
        case 0xE9:
            break;
        case 0xEA:
            break;
        case 0xEB:
            break;
        case 0xEC:
            break;
        case 0xED:
            break;
        case 0xEE:
            break;
        case 0xEF:
            break;

        // ==================== 0xF0 - 0xFF ====================

        case 0xF0: 
            break;
        case 0xF1:
            break;
        case 0xF2:
            break;
        case 0xF3:
            break;
        case 0xF4:
            break;
        case 0xF5:
            break;
        case 0xF6:
            break;
        case 0xF7:
            break;
        case 0xF8:
            break;
        case 0xF9:
            break;
        case 0xFA:
            break;
        case 0xFB:
            break;
        case 0xFC:
            break;
        case 0xFD:
            break;
        case 0xFE:
            break;
        case 0xFF:
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

        
            

        // case 0x3E0: // LD A, d8
        //     cpu->A = memory_read(cpu->pc++);
        //     cpu->cycles += 8;
        //     break;
        
        // case 0x96: // LD B, d8 
        //     cpu->B = memory_read(cpu->pc++);
        //     cpu->cycles += 8;
        //     break;
            
        // case 0x8E: // LD C, d8 
        //     cpu->C = memory_read(cpu->pc++);
        //     cpu->cycles += 8;
        //     break;

        // case 0x800: // ADD A, B
        //     cpu->A = cpu->A + cpu->B;
        //     cpu->cycles += 4;
        //     break;

        // case 0x210: {
        //     u8 lowerByte = memory_read(cpu->pc++);
        //     u8 higherByte = memory_read(cpu->pc++);
        //     set_hl(cpu, (higherByte << 8) | lowerByte);
        //     cpu->cycles += 12;
        //     break;
        // }

        // case 0x770: // LD (HL), A
        //     memory_write(get_hl(cpu), cpu-> A);
        //     cpu->cycles += 8;
        //     break;

        // case 0x760: // HALT
        //     cpu->isHalted = true;
        //     cpu->cycles += 4;
        //     break;

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

void ld_pair_imm(CPU* cpu, void(*setter)(CPU*, u16)){
    u8 lowerByte = memory_read(cpu->pc++);
    u8 upperByte = memory_read(cpu->pc++);
    setter(cpu, (upperByte<<8) | lowerByte);
}

// below this is new

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

// unitl here




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

// -------------------- 16-bit Opcode (CB) Instruction Helper Functions --------------------








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

