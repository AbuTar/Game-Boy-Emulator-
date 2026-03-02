#include "ppu.h"
#include "memory.h"

u8 ppu_map_palette(u8 palette, u8 color) {
    // Palette format: bits 0-1 (Colour0), bits 2-3 (Colour1), bits 4-5 (Colour 2) and bits 6-7 (Colour3)
    u8 shift = (color & 0x03) * 2;
    return (palette >> shift) & 0x03;
}

u16 background_tile_address(u8 tile_index, bool unsigned_mode){
    // BG/Window tile data base depends on LCDC bit 4:
    //  - 1: $8000-$8FFF, tile index is unsigned
    //  - 0: $8800-$97FF, tile index is signed (base effectively $9000)
    if (unsigned_mode){
        return 0x8000 + ((u16)tile_index * 16);
    }
    return (u16)(0x9000 + ((s8)tile_index * 16));
}

u8 ppu_get_pixel_tile_address(u16 tile_location, u8 pixel_x, u8 pixel_y){
    // Every Tile is 16 Bytes (8 Rows, 2 Bytes per Row)
    u16 row_location = tile_location + ((u16)pixel_y * 2); // Extract Bytes in the Row
    u8 lower_byte = memory_read(row_location);
    u8 upper_byte = memory_read(row_location + 1);

    // Each pixel, in each row, is 2 bits
    u8 bit_pos = 7 - pixel_x; // bit 7 is first pixel
    u8 lower_bit = (lower_byte >> bit_pos) & 1;
    u8 upper_bit = (upper_byte >> bit_pos) & 1;

    // combine the bits to return colour
    return (upper_bit << 1) | lower_bit;
}

// STAT helpers

void ppu_set_mode(u8 mode){
    // STAT bits 0-1 = mode
    u8 stat = memory_read(0xFF41);
    stat = (stat & 0xFC) | (mode & 0x03);
    memory_write(0xFF41, stat);
}

void ppu_check_lyc_interrupt(void){
    // STAT bit 2 = LYC==LY flag, STAT bit 6 enables interrupt on coincidence
    u8 ly  = memory_read(0xFF44);
    u8 lyc = memory_read(0xFF45);

    u8 stat = memory_read(0xFF41);
    bool eq = (ly == lyc);

    if (eq) stat |=  (1 << 2);
    else    stat &= ~(1 << 2);
    memory_write(0xFF41, stat);

    if (eq && (stat & (1 << 6))){
        request_interrupt(0x02); // LCD STAT interrupt
    }
}

void ppu_request_mode_interrupt(u8 mode){
    // Mode interrupt enables in STAT:
    // bit3: mode0 HBlank
    // bit4: mode1 VBlank
    // bit5: mode2 OAM
    u8 stat = memory_read(0xFF41);

    if (mode == PPU_MODE_HBLANK){
        if (stat & (1 << 3)) request_interrupt(0x02);
    } else if (mode == PPU_MODE_VBLANK){
        if (stat & (1 << 4)) request_interrupt(0x02);
    } else if (mode == PPU_MODE_OAM){
        if (stat & (1 << 5)) request_interrupt(0x02);
    }
}

void ppu_sort_sprites(Sprite* sprites, int count){
    // Sort sprites by ascending x value, then ascending OAM
    for (int i = 0; i < count - 1; i++){
        for (int j = i + 1; j < count; j++){
            bool swap = false;

            if (sprites[j].x < sprites[i].x) swap = true;
            else if (sprites[j].x == sprites[i].x && sprites[j].oam_index < sprites[i].oam_index) swap = true;

            if (swap){
                Sprite tmp = sprites[i];
                sprites[i] = sprites[j];
                sprites[j] = tmp;
            }
        }
    }
}