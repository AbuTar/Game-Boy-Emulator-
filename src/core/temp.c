// ==================== 0x00 - 0x0F ====================
case 0x00: // NOP
    cpu->cycles += 4;
    break;

case 0x01: // LD BC, d16
    ld_pair_imm(cpu, set_bc);
    cpu->cycles += 12;
    break;

case 0x02: // LD (BC), A
    ld_reg_pair(cpu, get_bc(cpu), cpu->A);
    cpu->cycles += 8;
    break;

case 0x03: // INC BC
    inc_r16(cpu, get_bc, set_bc);
    cpu->cycles += 8;
    break;

case 0x04: // INC B
    inc_r8(cpu, &cpu->B);
    cpu->cycles += 4;
    break;

case 0x05: // DEC B
    dec_r8(cpu, &cpu->B);
    cpu->cycles += 4;
    break;

case 0x06: // LD B, d8
    ld_reg_imm(cpu, &cpu->B);
    cpu->cycles += 8;
    break;

case 0x07: // RLCA
    rlca(cpu);
    cpu->cycles += 4;
    break;

case 0x08: // LD (a16), SP
    {
        u8 low = memory_read(cpu->pc++);
        u8 high = memory_read(cpu->pc++);
        u16 addr = (high << 8) | low;
        memory_write(addr, cpu->sp & 0xFF);
        memory_write(addr + 1, (cpu->sp >> 8) & 0xFF);
        cpu->cycles += 20;
    }
    break;

case 0x09: // ADD HL, BC
    add_hl_r16(cpu, get_bc(cpu));
    cpu->cycles += 8;
    break;

case 0x0A: // LD A, (BC)
    ld_pair_reg(cpu, &cpu->A, get_bc(cpu));
    cpu->cycles += 8;
    break;

case 0x0B: // DEC BC
    dec_r16(cpu, get_bc, set_bc);
    cpu->cycles += 8;
    break;

case 0x0C: // INC C
    inc_r8(cpu, &cpu->C);
    cpu->cycles += 4;
    break;

case 0x0D: // DEC C
    dec_r8(cpu, &cpu->C);
    cpu->cycles += 4;
    break;

case 0x0E: // LD C, d8
    ld_reg_imm(cpu, &cpu->C);
    cpu->cycles += 8;
    break;

case 0x0F: // RRCA
    rrca(cpu);
    cpu->cycles += 4;
    break;

// ==================== 0x10 - 0x1F ====================
case 0x10: // STOP
    cpu->isHalted = true;
    cpu->pc++;  // STOP is 2 bytes
    cpu->cycles += 4;
    break;

case 0x11: // LD DE, d16
    ld_pair_imm(cpu, set_de);
    cpu->cycles += 12;
    break;

case 0x12: // LD (DE), A
    ld_reg_pair(cpu, get_de(cpu), cpu->A);
    cpu->cycles += 8;
    break;

case 0x13: // INC DE
    inc_r16(cpu, get_de, set_de);
    cpu->cycles += 8;
    break;

case 0x14: // INC D
    inc_r8(cpu, &cpu->D);
    cpu->cycles += 4;
    break;

case 0x15: // DEC D
    dec_r8(cpu, &cpu->D);
    cpu->cycles += 4;
    break;

case 0x16: // LD D, d8
    ld_reg_imm(cpu, &cpu->D);
    cpu->cycles += 8;
    break;

case 0x17: // RLA
    rla(cpu);
    cpu->cycles += 4;
    break;

case 0x18: // JR s8
    jr(cpu);
    cpu->cycles += 12;
    break;

case 0x19: // ADD HL, DE
    add_hl_r16(cpu, get_de(cpu));
    cpu->cycles += 8;
    break;

case 0x1A: // LD A, (DE)
    ld_pair_reg(cpu, &cpu->A, get_de(cpu));
    cpu->cycles += 8;
    break;

case 0x1B: // DEC DE
    dec_r16(cpu, get_de, set_de);
    cpu->cycles += 8;
    break;

case 0x1C: // INC E
    inc_r8(cpu, &cpu->E);
    cpu->cycles += 4;
    break;

case 0x1D: // DEC E
    dec_r8(cpu, &cpu->E);
    cpu->cycles += 4;
    break;

case 0x1E: // LD E, d8
    ld_reg_imm(cpu, &cpu->E);
    cpu->cycles += 8;
    break;

case 0x1F: // RRA
    rra(cpu);
    cpu->cycles += 4;
    break;

// ==================== 0x20 - 0x2F ====================
case 0x20: // JR NZ, s8
    if (!get_flag(cpu, FLAG_Z)) {
        jr(cpu);
        cpu->cycles += 12;  // Branch taken
    } else {
        cpu->pc++;           // Skip offset byte
        cpu->cycles += 8;   // Branch not taken
    }
    break;

case 0x21: // LD HL, d16
    ld_pair_imm(cpu, set_hl);
    cpu->cycles += 12;
    break;

case 0x22: // LD (HL+), A
    lda_reg_hl_inc(cpu);
    cpu->cycles += 8;
    break;

case 0x23: // INC HL
    inc_r16(cpu, get_hl, set_hl);
    cpu->cycles += 8;
    break;

case 0x24: // INC H
    inc_r8(cpu, &cpu->H);
    cpu->cycles += 4;
    break;

case 0x25: // DEC H
    dec_r8(cpu, &cpu->H);
    cpu->cycles += 4;
    break;

case 0x26: // LD H, d8
    ld_reg_imm(cpu, &cpu->H);
    cpu->cycles += 8;
    break;

case 0x27: // DAA
    daa(cpu);
    cpu->cycles += 4;
    break;

case 0x28: // JR Z, s8
    if (get_flag(cpu, FLAG_Z)) {
        jr(cpu);
        cpu->cycles += 12;  // Branch taken
    } else {
        cpu->pc++;
        cpu->cycles += 8;   // Branch not taken
    }
    break;

case 0x29: // ADD HL, HL
    add_hl_r16(cpu, get_hl(cpu));
    cpu->cycles += 8;
    break;

case 0x2A: // LD A, (HL+)
    lda_hl_reg_inc(cpu);
    cpu->cycles += 8;
    break;

case 0x2B: // DEC HL
    dec_r16(cpu, get_hl, set_hl);
    cpu->cycles += 8;
    break;

case 0x2C: // INC L
    inc_r8(cpu, &cpu->L);
    cpu->cycles += 4;
    break;

case 0x2D: // DEC L
    dec_r8(cpu, &cpu->L);
    cpu->cycles += 4;
    break;

case 0x2E: // LD L, d8
    ld_reg_imm(cpu, &cpu->L);
    cpu->cycles += 8;
    break;

case 0x2F: // CPL
    cpl(cpu);
    cpu->cycles += 4;
    break;

// ==================== 0x30 - 0x3F ====================
case 0x30: // JR NC, s8
    if (!get_flag(cpu, FLAG_C)) {
        jr(cpu);
        cpu->cycles += 12;  // Branch taken
    } else {
        cpu->pc++;
        cpu->cycles += 8;   // Branch not taken
    }
    break;

case 0x31: // LD SP, d16
    ld_pair_imm(cpu, set_sp);
    cpu->cycles += 12;
    break;

case 0x32: // LD (HL-), A
    lda_reg_hl_dec(cpu);
    cpu->cycles += 8;
    break;

case 0x33: // INC SP
    cpu->sp++;
    cpu->cycles += 8;
    break;

case 0x34: // INC (HL)
    inc_mem_hl(cpu);
    cpu->cycles += 12;
    break;

case 0x35: // DEC (HL)
    dec_mem_hl(cpu);
    cpu->cycles += 12;
    break;

case 0x36: // LD (HL), d8
    memory_write(get_hl(cpu), memory_read(cpu->pc++));
    cpu->cycles += 12;
    break;

case 0x37: // SCF
    scf(cpu);
    cpu->cycles += 4;
    break;

case 0x38: // JR C, s8
    if (get_flag(cpu, FLAG_C)) {
        jr(cpu);
        cpu->cycles += 12;  // Branch taken
    } else {
        cpu->pc++;
        cpu->cycles += 8;   // Branch not taken
    }
    break;

case 0x39: // ADD HL, SP
    add_hl_r16(cpu, cpu->sp);
    cpu->cycles += 8;
    break;

case 0x3A: // LD A, (HL-)
    lda_hl_reg_dec(cpu);
    cpu->cycles += 8;
    break;

case 0x3B: // DEC SP
    cpu->sp--;
    cpu->cycles += 8;
    break;

case 0x3C: // INC A
    inc_r8(cpu, &cpu->A);
    cpu->cycles += 4;
    break;

case 0x3D: // DEC A
    dec_r8(cpu, &cpu->A);
    cpu->cycles += 4;
    break;

case 0x3E: // LD A, d8
    ld_reg_imm(cpu, &cpu->A);
    cpu->cycles += 8;
    break;

case 0x3F: // CCF
    ccf(cpu);
    cpu->cycles += 4;
    break;