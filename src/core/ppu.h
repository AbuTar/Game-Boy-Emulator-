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
    u16 dot;          // 0..455

    // latched per-scanline rendering regs
    u8 latched_lcdc;
    u8 latched_scx;
    u8 latched_scy;
    u8 latched_wx;
    u8 latched_wy;
    u8 latched_bgp;
    u8 latched_obp0;
    u8 latched_obp1;
} PPU;

// Sprites
typedef struct {
    u8 oam_index;
    int x;
    int y;
    u8 tile_index;
    u8 attributes;
} Sprite;

// Functions
void ppu_init(PPU* ppu);
void ppu_step(PPU* ppu, u8 cycles);  // Called after each CPU instruction
void ppu_render_scanline(PPU* ppu);  // Draws one scanline

u8 apply_palette(u8 colour);
u8 get_pixel_tile(u8 tile_pos, u8 pixel_x, u8 pixel_y);
u8 apply_pixel_palette(u8 colour, bool palette);
u8 apply_sprite_palette(u8 colour, bool palette);


// Helper functions

u8  ppu_map_palette(u8 palette, u8 color);
u16 background_tile_address(u8 tile_index, bool unsigned_mode);
u8  ppu_get_pixel_tile_address(u16 tile_location, u8 pixel_x, u8 pixel_y);

void ppu_set_mode(u8 mode);
void ppu_check_lyc_interrupt(void);
void ppu_request_mode_interrupt(u8 mode);

void ppu_sort_sprites(Sprite* sprites, int count);
#endif