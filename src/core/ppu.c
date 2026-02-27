#include <string.h>
#include "ppu.h"
#include "memory.h"

void ppu_init(PPU* ppu){
    // Initialises the PPU
    memset(ppu, 0, sizeof(PPU));
    ppu->lcd_enabled = true;
    ppu->mode = PPU_MODE_OAM;
    ppu->current_scanline = 0;
    ppu->scan_line_counter = 0;
}

u8 get_pixel_tile(u8 tile_pos, u8 pixel_x, u8 pixel_y){
    // Every Tile is 16 Bytes (8 Rows, 2 Bytes per Row)
    u16 tile_location = 0x8000 + (tile_pos * 16);

    u16 row_location = tile_location + (pixel_y * 2); // Extract Bytes in the Row
    u8 lower_byte = memory_read(row_location);
    u8 upper_byte = memory_read(row_location + 1);

    // Each pixel, in each row, is 2 bits
    u8 bit_pos = 7 - pixel_x; // bit 7 is first pixel
    u8 lower_bit = (lower_byte >> bit_pos) & 1;
    u8 upper_bit = (upper_byte >> bit_pos) & 1;

    // combine thw bits to return colour
    return (upper_bit << 1) | lower_bit;
}

u8 apply_palette(u8 color) {
    u8 palette = memory_read(0xFF47);  // BGP - Background Palette
    
    // Palette format: bits 0-1 (Colour0), bits 2-3 (Colour1), bits 4-5 (Colour 2) and bits 6-7 (Colour3)
    u8 shift = color * 2;
    return (palette >> shift) & 0x03;
}

void ppu_step(PPU* ppu, u8 cycle_count){
    
    if (!ppu->lcd_enabled) return;

    // Inititalise cycle count
    ppu->scan_line_counter += cycle_count;

    if (ppu->scan_line_counter >= 456){
        ppu->scan_line_counter -= 456;
        ppu->current_scanline++;

        // Updates current_scaneline register (LY)
         memory_write(0xFF44, ppu->current_scanline);

        if (ppu->current_scanline < 144){
            ppu_render_scanline(ppu);
        }

        else if (ppu->current_scanline == 144){
            ppu->mode = PPU_MODE_VBLANK;
            request_interrupt(0x01);
        }

        else if (ppu->current_scanline >= 154){
            ppu->current_scanline = 0;
            memory_write(0xFF44, 0);
        }
    }

}

void ppu_render_scanline(PPU* ppu){
    u8 LCDC_reg = memory_read(0xFF40);

    if (!(LCDC_reg & 0x01)){
        // Bit 0 of LCDC enables background
        for (int x = 0; x < SCREEN_WIDTH; x++){
            ppu->frame_buffer[ppu->current_scanline][x] = 0;
        }
        return;
    }

    // Read Scroll Headers (i.e. camera offsets)
    u8 scroll_y = memory_read(0xFF42);
    u8 scroll_x = memory_read(0xFF43);

    u16 background_map = (LCDC_reg & 0x08) ? 0x9C00 : 0x9800; // Two tile maps, one at 0x9C00 and one at 0x9800
    u8 background_y = (ppu->current_scanline + scroll_y) & 0xFF; // Find Y position in background
    u8 tile_row = background_y / 8; // Tiles are 8 pixels tall so we can find tile row
    u8 tile_y = background_y % 8;

    // Rendering Lines
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        u8 background_x = (scroll_x + x) & 0xFF;
        u8 tile_column = background_x / 8;
        u8 tile_x = background_x % 8;

        u16 tile_map_location = background_map + (tile_row * 32) + tile_column; // x32 since background maps are 32x32
        u8 tile_index = memory_read(tile_map_location);

        u8 colour = get_pixel_tile(tile_index, tile_x, tile_y);
        u8 palette = apply_palette(colour);

        ppu->frame_buffer[ppu->current_scanline][x] = palette;

    }
    
}