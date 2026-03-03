#include "memory.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include "boot.h"
#include "sram.h"


// Memory Map

// 0x0000-0x3FFF  ROM Bank 0 (16KB, fixed)
// 0x4000-0x7FFF  ROM Bank 1-N (16KB, switchable with MBC)
// 0x8000-0x9FFF  Video RAM (VRAM) - 8KB
// 0xA000-0xBFFF  External RAM (cartridge RAM, 8KB)
// 0xC000-0xDFFF  Work RAM (WRAM) - 8KB
// 0xE000-0xFDFF  Echo RAM (mirror of 0xC000-0xDDFF)
// 0xFE00-0xFE9F  OAM (Object Attribute Memory) - sprites
// 0xFEA0-0xFEFF  Unusable (prohibited area)
// 0xFF00-0xFF7F  I/O Registers (hardware controls)
// 0xFF80-0xFFFE  High RAM (HRAM) - fast 127 bytes
// 0xFFFF         Interrupt Enable register


// static u8 memory [0x10000]; // 64 KB of Memory
static u8* cart_rom = NULL; // Think of this as the cartridge contents
static size_t rom_size = 0;

// Memory Banks can be their own arrays
static u8 eram[0x8000]; // external RAM
static u8 vram[0x2000]; // 0x8000 - 0x9FFFF
static u8 wram[0x2000];// 0xC000 - DxFFFF
static u8 oam[0xA0]; // 0xFE00 - FE9F
static u8 io[0x80]; // 0xFF00 - 0xFF7F
static u8 hram[0x7F]; // 0xFF80 - 0xFFFE
static u8 ie; // 0xFFFF

// Memory Banking Variables

static u8 mbc_type = 0; // 0 = None, 1 = MBC1 etc....
static bool enable_ram = false; // Is External Ram available
static u8 rom_bank = 1; // ROM BANK (1-127)
static u8 ram_bank = 0; // RAM BANK (0-3)
static u8 bank_mode = 0; // Either using RAM or ROM (1 or 0)
static u8 mbc_byte = 0x00;






void memory_init(void){
    // memset(memory, 0, 0x10000);
    memset(vram, 0, sizeof(vram));
    memset(wram, 0, sizeof(wram));
    memset(oam, 0, sizeof(oam));
    memset(io, 0, sizeof(io));
    memset(hram, 0, sizeof(hram));
    memset(eram, 0, sizeof(eram));

    io[0x0F] = 0x00; // IF and IE registers
    ie = 0x00;
    io[0x40] = 0x91;  // LCDC - LCD enabled, BG enabled
    io[0x47] = 0xFC;  // BGP - Palette (11 11 10 00)
    io[0x48] = 0xFF;  // OBP0
    io[0x49] = 0xFF;  // OBP1
    io[0x00] = 0xFF;  // Nothing pressed
}

u8 memory_read(u16 address){

    // Boot ROM (0x0000-0x00FF) - takes priority when active
    if (address < 0x0100 && boot_is_active()) {
        return boot_read(address);
    }

    // ROM Bank (0x0000-0x7FFF)
    if (address <=  0x7FFF){
        if (cart_rom == NULL)  return 0xFF; // ROM hasn't loaded or CPU read beyond ROM size

        if (address <= 0x3FFF){
            if (address < rom_size){ return cart_rom[address];}
        }

        else {
            size_t bank_offset = (rom_bank * 0x4000) + (address - 0x4000);

            if (bank_offset < rom_size){
                return cart_rom[bank_offset];
            }
        }

        return 0xFF;
        

    }

    
    // VRAM Bank (0x8000-0x9FFF)
    else if (address >= 0x8000 && address <= 0x9FFF){
        return vram[address - 0x8000];

    }

    // External RAM Bank (0xA000-0xBFFF)
    else if (address >= 0xA000 && address <= 0xBFFF){

        if (!enable_ram){
            return 0xFF;
        }

        if (mbc_type == 3){
            if (ram_bank <= 0x03){
                u16 offset = (ram_bank * 0x2000) + (address - 0xA000);
                return eram[offset];
            }

            else{
                return 0xFF;
            }
        }

        if (mbc_type == 1 && bank_mode == 1){
            return eram[address - 0xA000]; // Need to do ASAP
        }

        return eram[address - 0xA000];
        

    }

    // WRAM Bank (0xC000-0xDFFF)
    else if (address >= 0xC000 && address <= 0xDFFF){
        return wram[address - 0xC000];

    }

    // ECHO Bank (0xE000-0xFDFF)
    else if (address >= 0xE000 && address <= 0xFDFF){
        return wram[address - 0xE000];

    }

    // OAM (0xFE00-0xFE9F)
    else if (address >= 0xFE00 && address <= 0xFE9F){
        return oam[address - 0xFE00];

    }

    // UNUSABLE (0xFEA0-0xFEFF)
    else if (address >= 0xFEA0 && address <= 0xFEFF){
        return 0xFF;
        
    }
    
    // I/O (0xFF00-0xFF7F)
    else if (address >= 0xFF00 && address <= 0xFF7F){
        if (address == 0xFF00){
            return io[0x00];
        }
        return io[address - 0xFF00];
        
    }

    // HRAM (0xFF80-0xFFFE))
    else if (address >= 0xFF80 && address <= 0xFFFE){
        return hram[address - 0xFF80];

    }

    // IE (0xFFFF)
     else if (address == 0xFFFF){
        return ie;
    }

    return 0xFF;
}

void memory_write(u16 address, u8 value){

    // ROM Bank (0x0000-0x7FFF)
    if (address <=  0x7FFF){
        
        // MBC 1 

        // 0x0000 - 0x1FFF is RAM enable
        if (mbc_type == 1){
            if (address <= 0x1FFF){
                enable_ram = ((value & 0x0F) == 0x0A);
            }

            // 0x2000 - 0x3FFF is ROM Bank Number, 5 least significant bits
            else if (address <= 0x3FFF){
                rom_bank = value & 0x1F;
                if (rom_bank == 0) rom_bank = 1;
            }

            else if (address <= 0x5FFF){
                if (bank_mode == 1){
                    ram_bank = 0x03 & value;
                }
            }

            else if(address <= 0x7FFF){
                bank_mode = value & 0x01;
            }
        }

        // MBC 2
        else if (mbc_type == 2) { // MBC2 handles RAM enable and ROM bank in the same address space
            if (address <= 0x3FFF) {
                // If bit 8 = 0:
                if ((address & 0x0100) == 0) {
                    enable_ram = ((value & 0x0F) == 0x0A);
                } 
                // If bit 8 == 1, ROM bank 
                else {
                    rom_bank = value & 0x0F; // Only 4 bits
                    if (rom_bank == 0) rom_bank = 1;
                }
            }
        }

        // MBC3

        else if (mbc_type == 3) {
            if (address <= 0x1FFF) {
                enable_ram = ((value & 0x0F) == 0x0A);
            }
            else if (address <= 0x3FFF) {
                rom_bank = value & 0x7F; // ROM BANK Number
                if (rom_bank == 0) rom_bank = 1;
            }
            else if (address <= 0x5FFF) {
                // RAM Bank Number (0x00-0x03) or RTC (0x08-0x0C)
                ram_bank = value;
            }
            else if (address <= 0x7FFF) {
                // Latch Clock Data (RTC) - not implemented
            }
            return;
        }

        return;

    }
    
    // VRAM Bank (0x8000-0x9FFF)
    else if (address >= 0x8000 && address <= 0x9FFF){
        vram[address - 0x8000] = value;

    }

    // External RAM Bank (0xA000-0xBFFF)
    else if (address >= 0xA000 && address <= 0xBFFF){
        if (!enable_ram) return;

        if (mbc_type == 1 && bank_mode == 1){
            eram[address - 0xA000] = value;
            sram_mark_dirty();
            return;
        }

        if (mbc_type == 3) {
            if (ram_bank <= 0x03) {
                u16 offset = (ram_bank * 0x2000) + (address - 0xA000);
                eram[offset] = value;
                sram_mark_dirty();
            }
            // TO-DO : Implement RTC registers
            return;
    }

        eram[address - 0xA000] = value;
        sram_mark_dirty();
    }

    // WRAM Bank (0xC000-0xDFFF)
    else if (address >= 0xC000 && address <= 0xDFFF){
        wram[address - 0xC000] = value;

    }

    // ECHO Bank (0xE000-0xFDFF)
    else if (address >= 0xE000 && address <= 0xFDFF){
        wram[address - 0xE000] = value;

    }

    // OAM (0xFE00-0xFE9F)
    else if (address >= 0xFE00 && address <= 0xFE9F){
        oam[address - 0xFE00] = value;

    }

    // UNUSABLE (0xFEA0-0xFEFF)
    else if (address >= 0xFEA0 && address <= 0xFEFF){
        return;
        
    }
    
    // I/O (0xFF00-0xFF7F)
    else if (address >= 0xFF00 && address <= 0xFF7F){

        if (address == 0xFF00) {
        // Only bits 4-5 are writable (selection). Low bits come from buttons.
        io[0x00] = (io[0x00] & 0xCF) | (value & 0x30);
        return;
    }

        if (address == 0xFF04){
            io[0x04] = 0;
            return;
        }

        // Boot ROM disable (0xFF50) - any write disables
        if (address == 0xFF50) {
            boot_disable();
            io[0x50] = value;
            return;
        }

        // OAM DMA transfer (FF46)
        if (address == 0xFF46) {
            io[0x46] = value;

            u16 src = ((u16)value) << 8;
            for (u16 i = 0; i < 0xA0; i++) {       
                oam[i] = memory_read(src + i);    
            }
            return;
        }
        
        io[address - 0xFF00] = value;
        
    }

    // HRAM (0xFF80-0xFFFE))
    else if (address >= 0xFF80 && address <= 0xFFFE){
        hram[address - 0xFF80] = value;

    }

    // IE (0xFFFF)
     else if (address == 0xFFFF){
        ie  = value;
    } 
}

void memory_load_rom(u8* data, size_t size) {
    free(cart_rom); // To be safe
    cart_rom = malloc(size);
    if (cart_rom == NULL){
        fprintf(stderr, "Failed to allocate %zu bytes for ROM\n", size);
        rom_size = 0;
        return;
    }

    memcpy(cart_rom, data, size); // Usage: memcpy(<destination>, <data>, <number of bytes>)
    rom_size = size;

    // To-Do read byte at 0x0147 in header to find out MBC type

    mbc_byte = data[0x0147];

    switch (mbc_byte){
        case 0x00: mbc_type = 0; break; // ROM Only
        case 0x01: mbc_type = 1; break; // MBC1
        case 0x02: mbc_type = 1; break; // MBC1 + RAM
        case 0x03: mbc_type = 1; break; // MBC1 + RAM + BATTERY
        case 0x05: mbc_type = 2; break; // MBC2 
        case 0x06: mbc_type = 2; break; // MBC2 + BATTERY
        case 0x0F: mbc_type = 3; break; // MBC3 + TIMER + BATTERY
        case 0x10: mbc_type = 3; break; // MBC3 + TIMER + RAM + BATTERY
        case 0x11: mbc_type = 3; break; // MBC3
        case 0x12: mbc_type = 3; break; // MBC3 + RAM
        case 0x13: mbc_type = 3; break; // MBC3 + RAM + BATTERY

        default: mbc_type = 0; break; // Unsupported so treat as ROM
    }

    // Initialise Banking State
    rom_bank = 1;
    ram_bank = 0;
    enable_ram = false;
    bank_mode = 0;
    
    printf("Cartridge type: 0x%02X (MBC%d)\n", mbc_byte, mbc_type);

}

void clear_memory(void){
    if (cart_rom != NULL){
        free(cart_rom);
        cart_rom = NULL;
        rom_size = 0;
    }
}

void request_interrupt(u8 interrupt_bit){
    u8 if_flag = memory_read(0xFF0F);
    memory_write(0xFF0F, if_flag | interrupt_bit);
}

void memory_set_joypad_low(u8 low4) {
    // Keep upper bits (selection + unused), replace only low nibble
    io[0x00] = (io[0x00] & 0xF0) | (low4 & 0x0F);
}

void memory_load_sram(const u8* data, size_t size) {
    if (size > 0x8000) size = 0x8000;
    memcpy(eram, data, size);
}

size_t memory_get_sram(u8* data_out) {
    // Depending on Cartridge, SRAM size varies
    size_t sram_size = 0;
    
    switch (mbc_byte) {
        case 0x00: sram_size = 0; break;
        case 0x01: sram_size = 0; break;
        case 0x05: sram_size = 0; break;
        case 0x0F: sram_size = 0; break; 
        case 0x11: sram_size = 0; break;
        
        // 2KB SRAM
        case 0x02: sram_size = 0x800; break;
        case 0x06: sram_size = 0x800; break;
        
        // 8KB SRAM
        case 0x03: sram_size = 0x8000; break;
        case 0x10: sram_size = 0x8000; break;
        case 0x12: sram_size = 0x8000; break;
        case 0x13: sram_size = 0x8000; break;
        
        default: sram_size = 0; break;
    }
    
    if (sram_size > 0 && data_out) {
        memcpy(data_out, eram, sram_size);
    }
    
    return sram_size;
}