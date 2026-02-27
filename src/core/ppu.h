#ifndef PPU_H
#define PPU_H

#include "types.h"
#include <stdbool.h>

// Screen dimensions
#define SCREEN_WIDTH  160
#define SCREEN_HEIGHT 144

// PPU modes aka (LCD status)
typedef enum {
    PPU_MODE_HBLANK = 0,  // Horizontal blank (end of scanline)
    PPU_MODE_VBLANK = 1,  // Vertical blank (end of frame)
    PPU_MODE_OAM    = 2,  // Searching OAM for sprites
    PPU_MODE_DRAW   = 3   // Drawing pixels to screen
} PPUMode;

// PPU 
typedef struct {
    u8 frame_buffer[SCREEN_HEIGHT][SCREEN_WIDTH];  // Final rendered pixels
    u16 scan_line_counter;  // Cycles between scanlines
    u8 current_scanline;    // Between 0 andd 153
    PPUMode mode;          
    
    bool lcd_enabled;    
} PPU;

// Functions
void ppu_init(PPU* ppu);
void ppu_step(PPU* ppu, u8 cycles);  // Called after each CPU instruction
void ppu_render_scanline(PPU* ppu);  // Draws one scanline
u8 apply_palette(u8 color);
u8 get_pixel_tile(u8 tile_pos, u8 pixel_x, u8 pixel_y);
#endif