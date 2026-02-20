#include "memory.h"
#include <string.h>
#include <stdlib.h>


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
static u8 vram[0x2000]; // 0x8000 - 0x9FFFF
static u8 wram[0x2000];// 0xC000 - DxFFFF
static u8 oam[0xA0]; // 0xFE00 - FE9F
static u8 io[0x80]; // 0xFF00 - 0xFF7F
static u8 hram[0x7F]; // 0xFF80 - 0xFFFE
static u8 ie; // 0xFFFF





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

    // ROM Segement
    if (address <=  0x7FFF){
        if (cart_rom != NULL && address < rom_size){
            return cart_rom[address];
        }

        return 0xFF; // ROM hasn't loaded or CPU read beyond ROM size

    }

    // VRAM Segement
    else if (address >= 0x8000 && address <= 0x9FFF){
        return vram[address - 0x8000];

    }

    // External RAM Segement
    else if (address >= 0xA000 && address <= 0xBFFF){
        return 0xFF; // Need to do ASAP

    }

    // WRAM Segement
    else if (address >= 0xC000 && address <= 0xDFFF){
        return wram[address - 0xC000];

    }

    // ECHO 
    else if (address >= 0xE000 && address <= 0xFDFF){
        return wram[address - 0xE000];

    }

    // OAM 
    else if (address >= 0xFE00 && address <= 0xFE9F){
        return oam[address - 0xFE00];

    }

    // UNUSABLE 
    else if (address >= 0xFEA0 && address <= 0xFEFF){
        return 0xFF;
        
    }
    
    // I/O
    else if (address >= 0xFF00 && address <= 0xFF7F){
        return io[address - 0xFF00];
        
    }

    // HRAM 
    else if (address >= 0xFF80 && address <= 0xFFFE){
        return hram[address - 0xFF80];

    }

    // IE
     else if (address == 0xFFFF){
        return ie;
    }

    return 0xFF;
}

void memory_write(u16 address, u8 value){

    // ROM Segement
    if (address <=  0x7FFF){
        return;

    }
    
    // VRAM Segement
    else if (address >= 0x8000 && address <= 0x9FFF){
        vram[address - 0x8000] = value;

    }

    // External RAM Segement
    else if (address >= 0xA000 && address <= 0xBFFF){
        return;

    }

    // WRAM Segement
    else if (address >= 0xC000 && address <= 0xDFFF){
        wram[address - 0xC000] = value;

    }

    // ECHO 
    else if (address >= 0xE000 && address <= 0xFDFF){
        wram[address - 0xE000] = value;

    }

    // OAM 
    else if (address >= 0xFE00 && address <= 0xFE9F){
        oam[address - 0xFE00] = value;

    }

    // UNUSABLE 
    else if (address >= 0xFEA0 && address <= 0xFEFF){
        return;
        
    }
    
    // I/O
    else if (address >= 0xFF00 && address <= 0xFF7F){

        if (address == 0xFF04){
            io[0x04] = 0;
            return;
        }
        io[address - 0xFF00] = value;
        
    }

    // HRAM 
    else if (address >= 0xFF80 && address <= 0xFFFE){
        hram[address - 0xFF80] = value;

    }

    // IE
     else if (address == 0xFFFF){
        ie  = value;
    } 
}

void memory_load_rom(u8* data, size_t size) {
    free(cart_rom); // To be safe
    cart_rom = malloc(size);
    if (cart_rom == NULL){
        rom_size = 0;
        return;
    }

    memcpy(cart_rom, data, size); // Usage: memcpy(<destination>, <data>, <number of bytes>)
    rom_size = size;
}