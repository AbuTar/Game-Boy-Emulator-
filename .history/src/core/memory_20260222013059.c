#include "memory.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


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
static u8 eram[0x2000]; // external RAM
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
    io[0x0F] = 0x00; // IF and IE registers
    ie = 0x00;
}

u8 memory_read(u16 address){

    // ROM Bank (0x0000-0x7FFF)
    if (address <=  0x7FFF){
        if (cart_rom == NULL)  return 0xFF; // ROM hasn't loaded or CPU read beyond ROM size

        if (address <= 0x3FFF){
            if (address < rom_size){ return cart_rom[address];}
        }

        else {
            size_t bank_offset = (rom_bank * 0x4000 + (address - 0x4000));

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

        if (!enable_ram) return 0xFF;

        if (mbc_byte == 1 && bank_mode == 1){
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
            return;
        }

        eram[address - 0xA000] = value;
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

        if (address == 0xFF04){
            io[0x04] = 0;
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