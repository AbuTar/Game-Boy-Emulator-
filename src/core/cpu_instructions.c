#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "cpu.h"
#include "memory.h"

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
            inc_pair_val(cpu, get_hl, set_hl); // Increment DE
            cpu->cycles += 8;
            break;

        case 0x24:
            inc_reg(cpu, &cpu->H); // Increment H
            cpu->cycles += 4;
            break;

        case 0x25:
            dec_reg(cpu, &cpu->H); // Decrement H
            cpu->cycles += 4;
            break;

        case 0x26:
            ld_reg_imm(cpu, &cpu->H); // Immediate to H
            cpu->cycles += 8;
            break;

        case 0x27:
            adjust_acc(cpu); // DAA
            cpu->cycles += 4;
            break;

        case 0x28: 

            if (get_flag(cpu, FLAG_Z)){ // JR Z, s8 - Branch Taken
                jr(cpu);
                cpu->cycles += 12;
            }

            else{   // Branch not Taken
                cpu->pc++;
                cpu->cycles += 8;
            }
            break;

        case 0x29:
            add_hl_pair(cpu, get_hl(cpu)); // ADD Hl to HL
            cpu->cycles += 8;
            break;
            
        case 0x2A:
            lda_hl_reg_inc(cpu); // Load HL into A, increment HL
            cpu->cycles += 8;
            break;

        case 0x2B:
            dec_pair_val(cpu, get_hl, set_hl); // Decrement HL
            cpu->cycles += 8;
            break;

        case 0x2C:
            inc_reg(cpu, &cpu->L); // Increment L
            cpu->cycles += 4;
            break;

        case 0x2D:
            dec_reg(cpu, &cpu->L); // Decrement L
            cpu->cycles += 4;
            break;

        case 0x2E:
            ld_reg_imm(cpu, &cpu->L); // Immediate to L
            cpu->cycles += 8;
            break;

        case 0x2F:
            complement_acc(cpu); // Flip contents of ACC
            cpu->cycles += 4;
            break;

        // ==================== 0x30 - 0x3F ====================
        case 0x30: 

            if (!get_flag(cpu, FLAG_C)){ // If CY 0, JUMP - Branch Taken
                jr(cpu);
                cpu->cycles += 12;
            }

            else{ // Branch not taken
                cpu->pc++;
                cpu->cycles += 8;
            }

            break;


        case 0x31:
            ld_pair_imm(cpu, set_sp); // Immediate to SP
            cpu->cycles += 12;
            break;

        case 0x32:
            lda_reg_hl_dec(cpu); // Load HL to A, Decrement HL
            cpu->cycles += 8;
            break;

        case 0x33:
            cpu->sp++; // Increment SP
            cpu->cycles += 8;
            break;

        case 0x34:
            inc_hl_mem(cpu); // Increment HL (Mem)
            cpu->cycles += 12;
            break;

        case 0x35:
            dec_hl_mem(cpu); // Decrement HL (Mem)
            cpu->cycles += 12;
            break;

        case 0x36:
            memory_write(get_hl(cpu), memory_read(cpu->pc++)); // Immediate to HL
            cpu->cycles += 12;
            break;

        case 0x37:
            set_carry_cy(cpu); // Set Carry Flag 
            cpu->cycles += 4;
            break;

        case 0x38:
            if (get_flag(cpu, FLAG_C)){ // If Carry Flag 1 - Branch Taken
                jr(cpu);
                cpu->cycles += 12;
            }

            else{ // Branch not taken
                cpu->pc++;
                cpu->cycles += 8;
            }

            break;

        case 0x39:
            add_hl_pair(cpu, cpu->sp); // ADD SL to HL
            cpu->cycles += 8;
            break;

        case 0x3A:
            lda_reg_hl_dec(cpu); // HL to A, decrement HL
            cpu->cycles += 8;
            break;

        case 0x3B:
            cpu->sp--; // Decrement SP
            cpu->cycles += 8;
            break;

        case 0x3C:
            inc_reg(cpu, &cpu->A); // Increment A
            cpu->cycles += 4;
            break;

        case 0x3D:
            dec_reg(cpu, &cpu->A); // Decrement A
            cpu->cycles += 4;
            break;

        case 0x3E:
            ld_reg_imm(cpu, &cpu->A); // Immediate to A
            cpu->cycles += 8;
            break;

        case 0x3F:
            flip_cy(cpu); // Flip Carry Flag Bit
            cpu->cycles += 4;
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
            ld_pair_reg(cpu, &cpu->C, get_hl(cpu)); // HL to C
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
            if (!get_flag(cpu, FLAG_Z)){ // Branch Taken
                ret(cpu);
                cpu->cycles += 20;
            }

            else{
                cpu->cycles += 8; // Branch Not Taken
            }

            break;

        case 0xC1:  // POP BC
            set_bc(cpu, pop(cpu));
            cpu->cycles += 12;
            break;

        case 0xC2: // JP NZ, a16
            if (!get_flag(cpu, FLAG_Z)){
                jp(cpu);
                cpu->cycles += 16;
            }

            else{
                cpu->pc += 2; // Skip Bytes
                cpu->cycles += 12;
            }

            break;

        case 0xC3: // JUMP a16
            jp(cpu);
            cpu->cycles += 16;
            break;

        case 0xC4: // CALL NZ, a16
            if (!get_flag(cpu, FLAG_Z)) {
                call(cpu);
                cpu->cycles += 24;  
            } 
            
            else {
                cpu->pc += 2; // Skip Btye
                cpu->cycles += 12;  
            }
            
            break;

        case 0xC5: // PUSH BC
            push(cpu, get_bc(cpu));
            cpu->cycles += 16;
            break;

        case 0xC6: // ADD immediate to A
            add_reg(cpu, &cpu->A, memory_read(cpu->pc++));
            cpu->cycles += 8;
            break;

        case 0xC7: // RESET 0x00
            rst(cpu, 0x00);
            cpu->cycles += 16;
            break;

        case 0xC8: // RET Z
            if (get_flag(cpu, FLAG_Z)){
                ret(cpu);
                cpu->cycles += 20;
            }

            else{
                cpu->cycles += 8;
            }

            break;


        case 0xC9:
            ret(cpu); // RET
            cpu->cycles += 16;
            break;

        case 0xCA: // JUMP Z, a16
            if (get_flag(cpu, FLAG_Z)){
                jp(cpu);
                cpu->cycles += 16;
            }

            else{
                cpu->pc += 2;
                cpu->cycles += 12;
            }

            break;


        case 0xCB: // Extension for 256 CB-Prefixed Instructions
        {
            u8 cb_opcodes = memory_read(cpu->pc++);
            cpu_execute_cb(cpu, cb_opcodes);
            break;

        }
            
        case 0xCC: // CALL Z, a16
            if (get_flag(cpu, FLAG_Z)){
                call(cpu);
                cpu->cycles += 24;
            }

            else{
                cpu->pc += 2;
                cpu->cycles += 12;

            }

            break;

        case 0xCD: // CALL a16
            call(cpu);
            cpu->cycles += 24;
            break;

        case 0xCE: // ADC Immediate to A
            add_reg_carry(cpu, &cpu->A, memory_read(cpu->pc++));
            cpu->cycles += 8;
            break;

        case 0xCF: // RST 0x08
        rst(cpu, 0x08);
            cpu->cycles += 16;
            break;

        // ==================== 0xD0 - 0xDF ====================

        case 0xD0: // RET NC
            if (!get_flag(cpu, FLAG_C)){
                ret(cpu);
                cpu->cycles += 20;
            }

            else{
                cpu->cycles += 8;
            }

            break;
            
        case 0xD1:
            set_de(cpu, pop(cpu)); // POP DE
            cpu->cycles += 12;
            break;

        case 0xD2: // JP NC, a16
            if (!get_flag(cpu, FLAG_C)){ 
                jp(cpu);
                cpu->cycles =+ 16;
            }

            else{
                cpu->pc += 2;
                cpu->cycles += 12;
            }
            break;
        
        case 0xD3:
            UNIMPLEMENTED_OPCODES(0xD3, cpu->pc - 1);
            cpu->isHalted = true;  // STOP
            break;

        case 0xD4: // CALL NC< a16
            if (!get_flag(cpu, FLAG_C)){
                call(cpu);
                cpu->cycles += 24;
            }

            else{
                cpu->pc += 2;
                cpu->cycles += 12;
            }
            break;

        case 0xD5: // PUSH DE
            push(cpu, get_de(cpu));
            cpu->cycles += 16;
            break;

        case 0xD6: // SUB Immediate
            sub_reg(cpu, &cpu->A, memory_read(cpu->pc++));
            cpu->cycles += 8;
            break;

        case 0xD7: // RST 0x10
            rst(cpu, 0x10);
            cpu->cycles += 16;
            break;

        case 0xD8: // RET C
            if (get_flag(cpu, FLAG_C)){
                ret(cpu);
                cpu->cycles += 20;
            }

            else{
                cpu->cycles += 8;
            }

            break;

        case 0xD9: 
            ret(cpu);
            cpu->ime = true;  // Enable Interrupts
            cpu->cycles += 16;
            break;

        case 0xDA: // JP C, a16
            if (get_flag(cpu, FLAG_C)){
                jp(cpu);
                cpu->cycles += 16;
            }

            else{
                cpu->pc += 2;
                cpu->cycles += 12;
            }

            break;

        case 0xDB:
            UNIMPLEMENTED_OPCODES(0xDB, cpu->pc - 1);
            cpu->isHalted = true;  // STOP
            break;

        case 0xDC: // CALL C, a16
            if (get_flag(cpu, FLAG_C)){
                call(cpu);
                cpu->cycles += 24;
            }

            else{
                cpu->pc += 2;
                cpu->cycles += 12;
            }

            break;

        case 0xDD:
            UNIMPLEMENTED_OPCODES(0xDD, cpu->pc - 1);
            cpu->isHalted = true; //  STOP
            break;

        case 0xDE: // SBC Immediate from A
            sub_reg_carry(cpu, &cpu->A, memory_read(cpu->pc++));
            cpu->cycles += 8;
            break;

        case 0xDF: // RST 0x18
            rst(cpu, 0x18);
            cpu->cycles += 16;
            break;

        // ==================== 0xE0 - 0xEF ====================

        case 0xE0: // STORE A in 0xFF00 + imm OFFSET    
            memory_write(0xFF00 + memory_read(cpu->pc++), cpu->A);
            cpu->cycles += 12;
            break;

        case 0xE1: // POP HL
            set_hl(cpu, pop(cpu));
            cpu->cycles += 12;
            break;

        case 0xE2: // STORE A in 0xFF00 + C
            memory_write(0xFF00 + cpu->C, cpu->A);
            cpu->cycles += 8;
            break;
        case 0xE3:
            UNIMPLEMENTED_OPCODES(0xE3, cpu->pc - 1);
            cpu->isHalted = true;  // STOP
            break;

        case 0xE4:
            UNIMPLEMENTED_OPCODES(0xE4, cpu->pc - 1);
            cpu->isHalted = true;  // STOP
            break;

        case 0xE5: // PUSH HL
            push(cpu, get_hl(cpu));
            cpu->cycles += 16;
            break;

        case 0xE6: // AND with Immediate
            and_reg(cpu, &cpu->A, memory_read(cpu->pc++));
            cpu->cycles += 8;
            break;

        case 0xE7: // RST 0x20
            rst(cpu, 0x20);
            cpu->cycles += 16;
            break;

        case 0xE8: // ADD SP, s8
        {
            s8 offset = (s8)memory_read(cpu->pc++);
            u16 result = cpu->sp + offset;
            set_flag(cpu, FLAG_Z, false);
            set_flag(cpu, FLAG_N, false);
            set_flag(cpu, FLAG_H, ((cpu->sp & 0xF) + (offset & 0xF)) > 0xF);
            set_flag(cpu, FLAG_C, ((cpu->sp & 0xFF) + (offset & 0xFF)) > 0xFF);
            cpu->sp = result;
            cpu->cycles += 16;
        }
            break;

        case 0xE9: // JP to Address in HL
            cpu->pc = get_hl(cpu);
            cpu->cycles += 4;
            break;

        case 0xEA: // a16 to A
            {
                u8 lowerByte = memory_read(cpu->pc++);
                u8 upperByte = memory_read(cpu->pc++);
                memory_write((upperByte << 8) | lowerByte, cpu->A);
                cpu->cycles += 16;
            }

            break;
            
        case 0xEB:
            UNIMPLEMENTED_OPCODES(0xEB, cpu->pc - 1);
            cpu->isHalted = true;  // STOP
            break;

        case 0xEC:
            UNIMPLEMENTED_OPCODES(0xEC, cpu->pc - 1);
            cpu->isHalted = true;  // STOP
            break;

        case 0xED:
            UNIMPLEMENTED_OPCODES(0xED, cpu->pc - 1);
            cpu->isHalted = true;  // STOP
            break;

        case 0xEE: // XOR Immediate
            xor_reg(cpu, &cpu->A, memory_read(cpu->pc++));
            cpu->cycles += 8;
            break;

        case 0xEF: // RST 0x28
            rst(cpu, 0x28);
            cpu->cycles += 16;
            break;

        // ==================== 0xF0 - 0xFF ====================

        case 0xF0: // LOAD into A from 0xFF00 + Immediate
            cpu->A = memory_read(0xFF00 + memory_read(cpu->pc++));
            cpu->cycles += 12;
            break;

        case 0xF1: // POP AF
            set_af(cpu, pop(cpu) & 0xFFF0);
            cpu->cycles += 12;
            break;

        case 0xF2: // LOAD into A from 0xFF00 + C
            cpu->A = memory_read(0xFF00 + cpu->C);
            cpu->cycles += 8;
            break;

        case 0xF3: // DISABLE Interrupts
            cpu->ime = false;
            cpu->cycles += 4;
            break;

        case 0xF4:
            UNIMPLEMENTED_OPCODES(0xF4, cpu->pc - 1);
            cpu->isHalted = true;  // STOP
            break;

        case 0xF5: // PUSH AF
            push(cpu, get_af(cpu));
            cpu->cycles += 16;
            break;
        
        case 0xF6: // OR Immediate
            or_reg(cpu, &cpu->A, memory_read(cpu->pc++));
            cpu->cycles += 8;
            break;

        case 0xF7: // RST 0x30
            rst(cpu, 0x30);
            cpu->cycles +=16;
            break;

        case 0xF8: // LOAD SP+s8 into HL
        {
            s8 offset = (s8)memory_read(cpu->pc++);
            u16 result = cpu->sp + offset;
            set_flag(cpu, FLAG_Z, false);
            set_flag(cpu, FLAG_N, false);
            set_flag(cpu, FLAG_H, ((cpu->sp & 0xF) + (offset & 0xF)) > 0xF);
            set_flag(cpu, FLAG_C, ((cpu->sp & 0xFF) + (offset & 0xFF)) > 0xFF);
            set_hl(cpu, result);
            cpu->cycles += 12;
        }
        
            break;

        case 0xF9: // LOAD SP, HL
            cpu->sp = get_hl(cpu);
            cpu->cycles += 8;
            break;

        case 0xFA: // LOAD 16-bit Immediate into A
        {
            u8 lowerByte = memory_read(cpu->pc++);
            u8 upperByte = memory_read(cpu->pc++);
            cpu->A = memory_read((upperByte << 8) | lowerByte);
            cpu->cycles += 16;
        }
            break;

        case 0xFB: // Enable Interrupts
            cpu->ime = true;
            cpu->cycles += 4;
            break;

        case 0xFC:
            UNIMPLEMENTED_OPCODES(0xFC, cpu->pc - 1);
            cpu->isHalted = true;  // STOP
            break;

        case 0xFD:
            UNIMPLEMENTED_OPCODES(0xFD, cpu->pc - 1);
            cpu->isHalted = true;  // STOP
            break;

        case 0xFE: // CP Immediate
            cp_reg(cpu, &cpu->A, memory_read(cpu->pc++));
            cpu->cycles += 8;
            break;

        case 0xFF: // RST 0x38
            rst(cpu, 0x38);
            cpu->cycles += 16;
            break;

        default:
            printf("Unknown opcode: 0x%02X at PC: 0x%04X\n", opcode, cpu->pc - 1);
            break;

    }
}

void cpu_execute_cb(CPU* cpu, u8 opcode){
    switch(opcode){

        // ==================== 0x00 - 0x07 (RLC) ====================
        case 0x00:
            rlc_reg(cpu, &cpu->B); // RLC B, C, D, E, H, L, HL, A
            cpu->cycles += 8; 
            break;

        case 0x01:
            rlc_reg(cpu, &cpu->C);
            cpu->cycles += 8; 
            break;

        case 0x02:
            rlc_reg(cpu, &cpu->D);
            cpu->cycles += 8; 
            break;

        case 0x03:
            rlc_reg(cpu, &cpu->E);
            cpu->cycles += 8; 
            break;

        case 0x04:
            rlc_reg(cpu, &cpu->H);
            cpu->cycles += 8; 
            break;

        case 0x05:
            rlc_reg(cpu, &cpu->L);
            cpu->cycles += 8; 
            break;

        case 0x06:
            rlc_hl(cpu);
            cpu->cycles += 16; 
            break;

        case 0x07:
            rlc_reg(cpu, &cpu->A);
            cpu->cycles += 8; 
            break;

        // ==================== 0x08 - 0x0F (RRC) ====================

        case 0x08: // RRC B, C, D, E, H, L, HL, A
            rrc_reg(cpu, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0x09:
            rrc_reg(cpu, &cpu->C);
            cpu->cycles += 8; 
            break;

        case 0x0A:
            rrc_reg(cpu, &cpu->D);
            cpu->cycles += 8; 
            break;

        case 0x0B:
            rrc_reg(cpu, &cpu->E);
            cpu->cycles += 8; 
            break;

        case 0x0C:
            rrc_reg(cpu, &cpu->H);
            cpu->cycles += 8; 
            break;

        case 0x0D:
            rrc_reg(cpu, &cpu->L);
            cpu->cycles += 8; 
            break;

        case 0x0E:
            rrc_hl(cpu);
            cpu->cycles += 16; 
            break;

        case 0x0F:
            rrc_reg(cpu, &cpu->A);
            cpu->cycles += 8; 
            break;

        // ==================== 0x10 - 0x17 (RL) ====================

        case 0x10:
            rl_reg(cpu, &cpu->B); // RL B, C, D, E, H, L, HL, A
            cpu->cycles += 8; 
            break;

        case 0x11:
            rl_reg(cpu, &cpu->C);
            cpu->cycles += 8; 
            break;

        case 0x12:
            rl_reg(cpu, &cpu->D);
            cpu->cycles += 8; 
            break;

        case 0x13:
            rl_reg(cpu, &cpu->E);
            cpu->cycles += 8; 
            break;

        case 0x14:
            rl_reg(cpu, &cpu->H);
            cpu->cycles += 8; 
            break;

        case 0x15:
            rl_reg(cpu, &cpu->L);
            cpu->cycles += 8; 
            break;

        case 0x16:
            rl_hl(cpu);
            cpu->cycles += 16; 
            break;

        case 0x17:
            rl_reg(cpu, &cpu->A);
            cpu->cycles += 8; 
            break;

        // ==================== 0x18 - 0x1F (RR) ====================

        case 0x18:
            rr_reg(cpu, &cpu->B); // RR B, C, D, E, H, L, HL, A
            cpu->cycles += 8; 
            break;

        case 0x19:
            rr_reg(cpu, &cpu->C);
            cpu->cycles += 8; 
            break;

        case 0x1A:
            rr_reg(cpu, &cpu->D);
            cpu->cycles += 8; 
            break;

        case 0x1B:
            rr_reg(cpu, &cpu->E);
            cpu->cycles += 8; 
            break;

        case 0x1C:
            rr_reg(cpu, &cpu->H);
            cpu->cycles += 8; 
            break;

        case 0x1D:
            rr_reg(cpu, &cpu->L);
            cpu->cycles += 8; 
            break;

        case 0x1E:
            rr_hl(cpu);
            cpu->cycles += 16; 
            break;

        case 0x1F:
            rr_reg(cpu, &cpu->A);
            cpu->cycles += 8; 
            break;

        // ==================== 0x20 - 0x27 (SLA) ====================
 
        case 0x20: 
            sla_reg(cpu, &cpu->B); // SLA B, C, D, E, H, L, HL, A
            cpu->cycles += 8; 
            break;

        case 0x21: 
            sla_reg(cpu, &cpu->C);
            cpu->cycles += 8; 
            break;

        case 0x22: 
            sla_reg(cpu, &cpu->D);
            cpu->cycles += 8; 
            break;

        case 0x23: 
            sla_reg(cpu, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0x24: 
            sla_reg(cpu, &cpu->H); 
            cpu->cycles += 8; 
            break;

        case 0x25: 
            sla_reg(cpu, &cpu->L); 
            cpu->cycles += 8; 
            break;

        case 0x26: 
            sla_hl(cpu); 
            cpu->cycles += 16; 
            break;

        case 0x27: 
            sla_reg(cpu, &cpu->A); 
            cpu->cycles += 8; 
            break;


        // ==================== 0x28 - 0x2F (SRA) ====================

        case 0x28: 
            sra_reg(cpu, &cpu->B); // SLA B, C, D, E, H, L, HL, A
            cpu->cycles += 8; 
            break;

        case 0x29: 
            sra_reg(cpu, &cpu->C);
            cpu->cycles += 8; 
            break;

        case 0x2A: 
            sra_reg(cpu, &cpu->D);
            cpu->cycles += 8; 
            break;

        case 0x2B: 
            sra_reg(cpu, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0x2C: 
            sra_reg(cpu, &cpu->H); 
            cpu->cycles += 8; 
            break;

        case 0x2D: 
            sra_reg(cpu, &cpu->L); 
            cpu->cycles += 8; 
            break;

        case 0x2E: 
            sra_hl(cpu); 
            cpu->cycles += 16; 
            break;

        case 0x2F: 
            sra_reg(cpu, &cpu->A); 
            cpu->cycles += 8; 
            break;

        // ==================== 0x30 - 0x37 (SWAP) ====================

        case 0x30:
            swap_reg(cpu, &cpu->B); // SWAP B, C, D, E, H, L, HL, A
            cpu->cycles += 8;
            break;

        case 0x31:
            swap_reg(cpu, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0x32:
            swap_reg(cpu, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0x33:
            swap_reg(cpu, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0x34:
            swap_reg(cpu, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0x35:
            swap_reg(cpu, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0x36:
            swap_hl(cpu);
            cpu->cycles += 16;
            break;

        case 0x37:
            swap_reg(cpu, &cpu->A);
            cpu->cycles += 8;
            break;


        // ==================== 0x38 - 0x3F (SRL) ====================

        case 0x38:
            srl_reg(cpu, &cpu->B); // SRL B, C, D, E, H, L, HL, A
            cpu->cycles += 8;
            break;

        case 0x39:
            srl_reg(cpu, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0x3A:
            srl_reg(cpu, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0x3B:
            srl_reg(cpu, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0x3C:
            srl_reg(cpu, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0x3D:
            srl_reg(cpu, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0x3E:
            srl_hl(cpu);
            cpu->cycles += 16;
            break;

        case 0x3F:
            srl_reg(cpu, &cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0x40 - 0x47 (BIT 0) ====================

        case 0x40:
            cpy_bit_reg(cpu, 0, cpu->B);
            cpu->cycles += 8;
            break;

        case 0x41:
            cpy_bit_reg(cpu, 0, cpu->C);
            cpu->cycles += 8;
            break;

        case 0x42:
            cpy_bit_reg(cpu, 0, cpu->D);
            cpu->cycles += 8;
            break;

        case 0x43:
            cpy_bit_reg(cpu, 0, cpu->E);
            cpu->cycles += 8;
            break;

        case 0x44:
            cpy_bit_reg(cpu, 0, cpu->H);
            cpu->cycles += 8;
            break;

        case 0x45:
            cpy_bit_reg(cpu, 0, cpu->L);
            cpu->cycles += 8;
            break;

        case 0x46:
            cpy_bit_reg(cpu, 0, memory_read(get_hl(cpu)));
            cpu->cycles += 12;
            break;

        case 0x47:
            cpy_bit_reg(cpu, 0, cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0x48 - 0x4F (BIT 1) ====================

        case 0x48:
            cpy_bit_reg(cpu, 1, cpu->B);
            cpu->cycles += 8;
            break;

        case 0x49:
            cpy_bit_reg(cpu, 1, cpu->C);
            cpu->cycles += 8;
            break;

        case 0x4A:
            cpy_bit_reg(cpu, 1, cpu->D);
            cpu->cycles += 8;
            break;

        case 0x4B:
            cpy_bit_reg(cpu, 1, cpu->E);
            cpu->cycles += 8;
            break;

        case 0x4C:
            cpy_bit_reg(cpu, 1, cpu->H);
            cpu->cycles += 8;
            break;

        case 0x4D:
            cpy_bit_reg(cpu, 1, cpu->L);
            cpu->cycles += 8;
            break;

        case 0x4E:
            cpy_bit_reg(cpu, 1, memory_read(get_hl(cpu)));
            cpu->cycles += 12;
            break;

        case 0x4F:
            cpy_bit_reg(cpu, 1, cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0x50 - 0x57 (BIT 2) ====================

        case 0x50:
            cpy_bit_reg(cpu, 2, cpu->B);
            cpu->cycles += 8;
            break;

        case 0x51:
            cpy_bit_reg(cpu, 2, cpu->C);
            cpu->cycles += 8;
            break;

        case 0x52:
            cpy_bit_reg(cpu, 2, cpu->D);
            cpu->cycles += 8;
            break;

        case 0x53:
            cpy_bit_reg(cpu, 2, cpu->E);
            cpu->cycles += 8;
            break;

        case 0x54:
            cpy_bit_reg(cpu, 2, cpu->H);
            cpu->cycles += 8;
            break;

        case 0x55:
            cpy_bit_reg(cpu, 2, cpu->L);
            cpu->cycles += 8;
            break;

        case 0x56:
            cpy_bit_reg(cpu, 2, memory_read(get_hl(cpu)));
            cpu->cycles += 12;
            break;

        case 0x57:
            cpy_bit_reg(cpu, 2, cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0x58 - 0x5F (BIT 3) ====================

        case 0x58: 
            cpy_bit_reg(cpu, 3, cpu->B);
            cpu->cycles += 8;
            break;

        case 0x59:
            cpy_bit_reg(cpu, 3, cpu->C);
            cpu->cycles += 8;
            break;

        case 0x5A:
            cpy_bit_reg(cpu, 3, cpu->D);
            cpu->cycles += 8;
            break;

        case 0x5B:
            cpy_bit_reg(cpu, 3, cpu->E);
            cpu->cycles += 8;
            break;

        case 0x5C:
            cpy_bit_reg(cpu, 3, cpu->H);
            cpu->cycles += 8;
            break;

        case 0x5D:
            cpy_bit_reg(cpu, 3, cpu->L);
            cpu->cycles += 8;
            break;

        case 0x5E:
            cpy_bit_reg(cpu, 3, memory_read(get_hl(cpu)));
            cpu->cycles += 12;
            break;

        case 0x5F:
            cpy_bit_reg(cpu, 3, cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0x60 - 0x67 (BIT 4) ====================

        case 0x60: 
            cpy_bit_reg(cpu, 4, cpu->B);
            cpu->cycles += 8;
            break;

        case 0x61:
            cpy_bit_reg(cpu, 4, cpu->C);
            cpu->cycles += 8;
            break;

        case 0x62:
            cpy_bit_reg(cpu, 4, cpu->D);
            cpu->cycles += 8;
            break;

        case 0x63:
            cpy_bit_reg(cpu, 4, cpu->E);
            cpu->cycles += 8;
            break;

        case 0x64:
            cpy_bit_reg(cpu, 4, cpu->H);
            cpu->cycles += 8;
            break;

        case 0x65:
            cpy_bit_reg(cpu, 4, cpu->L);
            cpu->cycles += 8;
            break;

        case 0x66:
            cpy_bit_reg(cpu, 4, memory_read(get_hl(cpu)));
            cpu->cycles += 12;
            break;

        case 0x67:
            cpy_bit_reg(cpu, 4, cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0x68 - 0x6F (BIT 5) ====================

        case 0x68: 
            cpy_bit_reg(cpu, 5, cpu->B);
            cpu->cycles += 8;
            break;

        case 0x69:
            cpy_bit_reg(cpu, 5, cpu->C);
            cpu->cycles += 8;
            break;

        case 0x6A:
            cpy_bit_reg(cpu, 5, cpu->D);
            cpu->cycles += 8;
            break;

        case 0x6B:
            cpy_bit_reg(cpu, 5, cpu->E);
            cpu->cycles += 8;
            break;

        case 0x6C:
            cpy_bit_reg(cpu, 5, cpu->H);
            cpu->cycles += 8;
            break;

        case 0x6D:
            cpy_bit_reg(cpu, 5, cpu->L);
            cpu->cycles += 8;
            break;

        case 0x6E:
            cpy_bit_reg(cpu, 5, memory_read(get_hl(cpu)));
            cpu->cycles += 12;
            break;

        case 0x6F:
            cpy_bit_reg(cpu, 5, cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0x70 - 0x77 (BIT 6) ====================

        case 0x70:
            cpy_bit_reg(cpu, 6, cpu->B);
            cpu->cycles += 8;
            break;

        case 0x71:
            cpy_bit_reg(cpu, 6, cpu->C);
            cpu->cycles += 8;
            break;

        case 0x72:
            cpy_bit_reg(cpu, 6, cpu->D);
            cpu->cycles += 8;
            break;

        case 0x73:
            cpy_bit_reg(cpu, 6, cpu->E);
            cpu->cycles += 8;
            break;

        case 0x74:
            cpy_bit_reg(cpu, 6, cpu->H);
            cpu->cycles += 8;
            break;

        case 0x75:
            cpy_bit_reg(cpu, 6, cpu->L);
            cpu->cycles += 8;
            break;

        case 0x76:
            cpy_bit_reg(cpu, 6, memory_read(get_hl(cpu)));
            cpu->cycles += 12;
            break;

        case 0x77:
            cpy_bit_reg(cpu, 6, cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0x78 - 0x7F (BIT 7) ====================

        case 0x78:
            cpy_bit_reg(cpu, 7, cpu->B);
            cpu->cycles += 8;
            break;

        case 0x79:
            cpy_bit_reg(cpu, 7, cpu->C);
            cpu->cycles += 8;
            break;

        case 0x7A:
            cpy_bit_reg(cpu, 7, cpu->D);
            cpu->cycles += 8;
            break;

        case 0x7B:
            cpy_bit_reg(cpu, 7, cpu->E);
            cpu->cycles += 8;
            break;

        case 0x7C:
            cpy_bit_reg(cpu, 7, cpu->H);
            cpu->cycles += 8;
            break;

        case 0x7D:
            cpy_bit_reg(cpu, 7, cpu->L);
            cpu->cycles += 8;
            break;

        case 0x7E:
            cpy_bit_reg(cpu, 7, memory_read(get_hl(cpu)));
            cpu->cycles += 12;
            break;

        case 0x7F:
            cpy_bit_reg(cpu, 7, cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0x80 - 0x87 (RES 0) ====================

        case 0x80: 
            rst_bit_reg(cpu, 0, &cpu->B); 
            cpu->cycles += 8; 
            break;

        case 0x81: 
            rst_bit_reg(cpu, 0, &cpu->C); 
            cpu->cycles += 8; 
            break;

        case 0x82: 
            rst_bit_reg(cpu, 0, &cpu->D); 
            cpu->cycles += 8; 
            break;

        case 0x83: 
            rst_bit_reg(cpu, 0, &cpu->E); 
            cpu->cycles += 8; 
            break;

        case 0x84: 
            rst_bit_reg(cpu, 0, &cpu->H); 
            cpu->cycles += 8;
            break;

        case 0x85: 
            rst_bit_reg(cpu, 0, &cpu->L); 
            cpu->cycles += 8; 
            break;

        case 0x86: 
            rst_bit_hl(cpu, 0);
            cpu->cycles += 16;
            break;

        case 0x87: 
            rst_bit_reg(cpu, 0, &cpu->A); 
            cpu->cycles += 8; 
            break;

        // ==================== 0x88 - 0x8F (RES 1) ====================

        case 0x88:
            rst_bit_reg(cpu, 1, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0x89:
            rst_bit_reg(cpu, 1, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0x8A:
            rst_bit_reg(cpu, 1, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0x8B:
            rst_bit_reg(cpu, 1, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0x8C:
            rst_bit_reg(cpu, 1, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0x8D:
            rst_bit_reg(cpu, 1, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0x8E: 
            rst_bit_hl(cpu, 1);
            cpu->cycles += 16; 
            break;

        case 0x8F:
            rst_bit_reg(cpu, 1, &cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0x90 - 0x97 (RES 2) ====================

        case 0x90:
            rst_bit_reg(cpu, 2, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0x91:
            rst_bit_reg(cpu, 2, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0x92:
            rst_bit_reg(cpu, 2, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0x93:
            rst_bit_reg(cpu, 2, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0x94:
            rst_bit_reg(cpu, 2, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0x95:
            rst_bit_reg(cpu, 2, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0x96: 
            rst_bit_hl(cpu, 2);
            cpu->cycles += 16;
            break;

        case 0x97:
            rst_bit_reg(cpu, 2, &cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0x98 - 0x9F (RES 3) ====================

        case 0x98:
            rst_bit_reg(cpu, 3, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0x99:
            rst_bit_reg(cpu, 3, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0x9A:
            rst_bit_reg(cpu, 3, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0x9B:
            rst_bit_reg(cpu, 3, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0x9C:
            rst_bit_reg(cpu, 3, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0x9D:
            rst_bit_reg(cpu, 3, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0x9E: 
            rst_bit_hl(cpu, 3);
            cpu->cycles += 16;
            break;

        case 0x9F:
            rst_bit_reg(cpu, 3, &cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0xA0 - 0xA7 (RES 4) ====================

        case 0xA0:
            rst_bit_reg(cpu, 4, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0xA1:
            rst_bit_reg(cpu, 4, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0xA2:
            rst_bit_reg(cpu, 4, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0xA3:
            rst_bit_reg(cpu, 4, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0xA4:
            rst_bit_reg(cpu, 4, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0xA5:
            rst_bit_reg(cpu, 4, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0xA6:
            rst_bit_hl(cpu, 4);
            cpu->cycles += 16;
            break;

        case 0xA7:
            rst_bit_reg(cpu, 4, &cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0xA8 - 0xAF (RES 5) ====================

        case 0xA8:
            rst_bit_reg(cpu, 5, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0xA9:
            rst_bit_reg(cpu, 5, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0xAA:
            rst_bit_reg(cpu, 5, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0xAB:
            rst_bit_reg(cpu, 5, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0xAC:
            rst_bit_reg(cpu, 5, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0xAD:
            rst_bit_reg(cpu, 5, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0xAE: 
            rst_bit_hl(cpu, 5);
            cpu->cycles += 16;
            break;

        case 0xAF:
            rst_bit_reg(cpu, 5, &cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0xB0 - 0xB7 (RES 6) ====================

        case 0xB0:
            rst_bit_reg(cpu, 6, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0xB1:
            rst_bit_reg(cpu, 6, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0xB2:
            rst_bit_reg(cpu, 6, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0xB3:
            rst_bit_reg(cpu, 6, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0xB4:
            rst_bit_reg(cpu, 6, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0xB5:
            rst_bit_reg(cpu, 6, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0xB6: 
            rst_bit_hl(cpu, 6);
            cpu->cycles += 16;
            break;

        case 0xB7:
            rst_bit_reg(cpu, 6, &cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0xB8 - 0xBF (RES 7) ====================

        case 0xB8:
            rst_bit_reg(cpu, 7, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0xB9:
            rst_bit_reg(cpu, 7, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0xBA:
            rst_bit_reg(cpu, 7, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0xBB:
            rst_bit_reg(cpu, 7, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0xBC:
            rst_bit_reg(cpu, 7, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0xBD:
            rst_bit_reg(cpu, 7, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0xBE:
            rst_bit_hl(cpu, 7);
            cpu->cycles += 16;
            break;

        case 0xBF:
            rst_bit_reg(cpu, 7, &cpu->A);
            cpu->cycles += 8;
            break;


        // ==================== 0xC0 - 0xC7 (SET 0) ====================

        case 0xC0:
            set_bit_reg(cpu, 0, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0xC1:
            set_bit_reg(cpu, 0, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0xC2:
            set_bit_reg(cpu, 0, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0xC3:
            set_bit_reg(cpu, 0, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0xC4:
            set_bit_reg(cpu, 0, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0xC5:
            set_bit_reg(cpu, 0, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0xC6: 
            set_bit_hl(cpu, 0);
            cpu->cycles += 16;
            break;

        case 0xC7:
            set_bit_reg(cpu, 0, &cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0xC8 - 0xCF (SET 1) ====================

        case 0xC8:
            set_bit_reg(cpu, 1, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0xC9:
            set_bit_reg(cpu, 1, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0xCA:
            set_bit_reg(cpu, 1, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0xCB:
            set_bit_reg(cpu, 1, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0xCC:
            set_bit_reg(cpu, 1, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0xCD:
            set_bit_reg(cpu, 1, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0xCE: 
            set_bit_hl(cpu, 1);
            cpu->cycles += 16;
            break;

        case 0xCF:
            set_bit_reg(cpu, 1, &cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0xD0 - 0xD7 (SET 2) ====================

        case 0xD0:
            set_bit_reg(cpu, 2, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0xD1:
            set_bit_reg(cpu, 2, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0xD2:
            set_bit_reg(cpu, 2, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0xD3:
            set_bit_reg(cpu, 2, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0xD4:
            set_bit_reg(cpu, 2, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0xD5:
            set_bit_reg(cpu, 2, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0xD6: 
            set_bit_hl(cpu, 2);
            cpu->cycles += 16;
            break;

        case 0xD7:
            set_bit_reg(cpu, 2, &cpu->A);
            cpu->cycles += 8;
            break;


        // ==================== 0xD8 - 0xDF (SET 3) ====================

        case 0xD8:
            set_bit_reg(cpu, 3, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0xD9:
            set_bit_reg(cpu, 3, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0xDA:
            set_bit_reg(cpu, 3, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0xDB:
            set_bit_reg(cpu, 3, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0xDC:
            set_bit_reg(cpu, 3, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0xDD:
            set_bit_reg(cpu, 3, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0xDE: 
            set_bit_hl(cpu, 3);
            cpu->cycles += 16;
            break;

        case 0xDF:
            set_bit_reg(cpu, 3, &cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0xE0 - 0xE7 (SET 4) ====================

        case 0xE0:
            set_bit_reg(cpu, 4, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0xE1:
            set_bit_reg(cpu, 4, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0xE2:
            set_bit_reg(cpu, 4, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0xE3:
            set_bit_reg(cpu, 4, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0xE4:
            set_bit_reg(cpu, 4, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0xE5:
            set_bit_reg(cpu, 4, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0xE6: 
            set_bit_hl(cpu, 4);
            cpu->cycles += 16;
            break;

        case 0xE7:
            set_bit_reg(cpu, 4, &cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0xE8 - 0xEF (SET 5) ====================

        case 0xE8:
            set_bit_reg(cpu, 5, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0xE9:
            set_bit_reg(cpu, 5, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0xEA:
            set_bit_reg(cpu, 5, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0xEB:
            set_bit_reg(cpu, 5, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0xEC:
            set_bit_reg(cpu, 5, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0xED:
            set_bit_reg(cpu, 5, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0xEE: 
            set_bit_hl(cpu, 5);
            cpu->cycles += 16;
            break;

        case 0xEF:
            set_bit_reg(cpu, 5, &cpu->A);
            cpu->cycles += 8;
            break;

        // ==================== 0xF0 - 0xF7 (SET 6) ====================

        case 0xF0:
            set_bit_reg(cpu, 6, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0xF1:
            set_bit_reg(cpu, 6, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0xF2:
            set_bit_reg(cpu, 6, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0xF3:
            set_bit_reg(cpu, 6, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0xF4:
            set_bit_reg(cpu, 6, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0xF5:
            set_bit_reg(cpu, 6, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0xF6: 
            set_bit_hl(cpu, 6);
            cpu->cycles += 16;
            break;

        case 0xF7:
            set_bit_reg(cpu, 6, &cpu->A);
            cpu->cycles += 8;
            break;



        // ==================== 0xF8 - 0xFF (SET 7) ====================

        case 0xF8:
            set_bit_reg(cpu, 7, &cpu->B);
            cpu->cycles += 8;
            break;

        case 0xF9:
            set_bit_reg(cpu, 7, &cpu->C);
            cpu->cycles += 8;
            break;

        case 0xFA:
            set_bit_reg(cpu, 7, &cpu->D);
            cpu->cycles += 8;
            break;

        case 0xFB:
            set_bit_reg(cpu, 7, &cpu->E);
            cpu->cycles += 8;
            break;

        case 0xFC:
            set_bit_reg(cpu, 7, &cpu->H);
            cpu->cycles += 8;
            break;

        case 0xFD:
            set_bit_reg(cpu, 7, &cpu->L);
            cpu->cycles += 8;
            break;

        case 0xFE: 
            set_bit_hl(cpu, 7);
            cpu->cycles += 16;
            break;

        case 0xFF:
            set_bit_reg(cpu, 7, &cpu->A);
            cpu->cycles += 8;
            break;

        default:
            printf("[UNKNOWN CB] Opcode: CB %02X at PC: 0x%04X\n", opcode, cpu->pc - 1);
            cpu->isHalted = true;
            break;
    }
    
}

