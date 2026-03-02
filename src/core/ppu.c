#include <string.h>
#include "ppu.h"
#include "memory.h"

// Window line counter (increments only on scanlines where the window is actually drawn)
static u8 s_window_line = 0;

// ---------- Tile helpers ----------

u8 get_pixel_tile(u8 tile_pos, u8 pixel_x, u8 pixel_y){
    // OBJ tile data is always from $8000 on DMG
    u16 tile_location = 0x8000 + ((u16)tile_pos * 16);

    u16 row_location = tile_location + ((u16)pixel_y * 2);
    u8 lower_byte = memory_read(row_location);
    u8 upper_byte = memory_read(row_location + 1);

    u8 bit_pos = 7 - pixel_x;
    u8 lower_bit = (lower_byte >> bit_pos) & 1;
    u8 upper_bit = (upper_byte >> bit_pos) & 1;

    return (upper_bit << 1) | lower_bit;
}

// Keeping your functions
u8 apply_palette(u8 color) {
    u8 palette = memory_read(0xFF47);  // BGP
    return ppu_map_palette(palette, color);
}

// kept for compatibility
u8 apply_pixel_palette(u8 color, bool palette){
    return apply_sprite_palette(color, palette);
}

u8 apply_sprite_palette(u8 color, bool palette1){
    u8 palette = memory_read(palette1 ? 0xFF49 : 0xFF48); // OBP1/OBP0
    return ppu_map_palette(palette, color);
}

// ---------- PPU core ----------

void ppu_init(PPU* ppu){
    // Initialises the PPU
    memset(ppu, 0, sizeof(PPU));
    ppu->lcd_enabled = true;
    ppu->mode = PPU_MODE_OAM;
    ppu->current_scanline = 0;
    ppu->scan_line_counter = 0;

    ppu->dot = 0;

    // initialise LY
    memory_write(0xFF44, 0);

    // Prime latches (safe defaults)
    ppu->latched_lcdc = memory_read(0xFF40);
    ppu->latched_scx  = memory_read(0xFF43);
    ppu->latched_scy  = memory_read(0xFF42);
    ppu->latched_wx   = memory_read(0xFF4B);
    ppu->latched_wy   = memory_read(0xFF4A);
    ppu->latched_bgp  = memory_read(0xFF47);
    ppu->latched_obp0 = memory_read(0xFF48);
    ppu->latched_obp1 = memory_read(0xFF49);

    s_window_line = 0;

    // Set STAT mode to OAM on boot and update LYC flag
    ppu_set_mode(PPU_MODE_OAM);
    ppu_check_lyc_interrupt();
}

void ppu_render_scanline(PPU* ppu){
    // Use latched regs (captured at start of mode 3)
    u8 LCDC_reg = ppu->latched_lcdc;

    // Raw BG/WIN colour id (0-3) for OBJ priority
    u8 bg_raw[SCREEN_WIDTH];
    memset(bg_raw, 0, sizeof(bg_raw));

    // ---------- Background / Window ----------
    if (!(LCDC_reg & 0x01)){
        // BG disabled: output colour 0 from BGP across the line (sprites may still draw)
        u8 shade0 = ppu_map_palette(ppu->latched_bgp, 0);
        for (int x = 0; x < SCREEN_WIDTH; x++){
            ppu->frame_buffer[ppu->current_scanline][x] = shade0;
            bg_raw[x] = 0;
        }
        goto SPRITES;
    }

    u8 scroll_y = ppu->latched_scy;
    u8 scroll_x = ppu->latched_scx;

    u16 bg_map = (LCDC_reg & 0x08) ? 0x9C00 : 0x9800;
    bool unsigned_tile_mode = (LCDC_reg & 0x10) != 0;

    u8 y_in_bg = (u8)(ppu->current_scanline + scroll_y);
    u8 tile_row = y_in_bg / 8;
    u8 tile_y   = y_in_bg % 8;

    for (int x = 0; x < SCREEN_WIDTH; x++){
        u8 x_in_bg = (u8)(scroll_x + (u8)x);
        u8 tile_col = x_in_bg / 8;
        u8 tile_x   = x_in_bg % 8;

        u16 tile_map_location = bg_map + ((u16)tile_row * 32) + tile_col;
        u8 tile_index = memory_read(tile_map_location);

        u16 tile_location = background_tile_address(tile_index, unsigned_tile_mode);
        u8 colour_id = ppu_get_pixel_tile_address(tile_location, tile_x, tile_y);
        u8 shade = ppu_map_palette(ppu->latched_bgp, colour_id);

        ppu->frame_buffer[ppu->current_scanline][x] = shade;
        bg_raw[x] = colour_id;
    }

    // Window overlay
    {
        bool window_enabled = (LCDC_reg & 0x20) != 0;
        if (window_enabled){
            u8 wy = ppu->latched_wy;
            u8 wx = ppu->latched_wx;

            // If WX is off-screen (>= 167), the window is hidden.
            if (ppu->current_scanline >= wy && wx <= 166){
                int win_x0 = (int)wx - 7;
                u16 win_map = (LCDC_reg & 0x40) ? 0x9C00 : 0x9800;

                u8 win_y = s_window_line;
                u8 win_tile_row = win_y / 8;
                u8 win_tile_y   = win_y % 8;

                for (int x = 0; x < SCREEN_WIDTH; x++){
                    if (x < win_x0){
                        continue;
                    }

                    int win_x = x - win_x0;
                    u8 tile_col = (u8)(win_x / 8);
                    u8 tile_x   = (u8)(win_x % 8);

                    u16 tile_map_location = win_map + ((u16)win_tile_row * 32) + tile_col;
                    u8 tile_index = memory_read(tile_map_location);

                    u16 tile_location = background_tile_address(tile_index, unsigned_tile_mode);
                    u8 colour_id = ppu_get_pixel_tile_address(tile_location, tile_x, win_tile_y);
                    u8 shade = ppu_map_palette(ppu->latched_bgp, colour_id);

                    ppu->frame_buffer[ppu->current_scanline][x] = shade;
                    bg_raw[x] = colour_id;
                }

                // Only advance if window actually drew
                s_window_line++;
            }
        }
    }

    // ---------- Sprites ----------
SPRITES:
    u8 LCDC_Reg_Sprite = ppu->latched_lcdc;

    if (!(LCDC_Reg_Sprite & 0x02)){
        return;
    }

    u8 sprite_height = (LCDC_Reg_Sprite & 0x04) ? 16 : 8;
    u8 scanline = ppu->current_scanline;

    Sprite line_sprites[10];
    int sprites_on_line = 0;

    // First 10 in OAM order
    for (int i = 0; i < 40; i++){
        u16 oam_addr = 0xFE00 + (u16)(i * 4);

        u8 sy_raw = memory_read(oam_addr + 0);
        u8 sx_raw = memory_read(oam_addr + 1);
        u8 tile   = memory_read(oam_addr + 2);
        u8 attr   = memory_read(oam_addr + 3);

        int sy = (int)sy_raw - 16;
        int sx = (int)sx_raw - 8;

        if (scanline < sy || scanline >= sy + sprite_height){
            continue;
        } 

        if (sprites_on_line >= 10){
            break;
        }

        line_sprites[sprites_on_line].oam_index = (u8)i;
        line_sprites[sprites_on_line].x = sx;
        line_sprites[sprites_on_line].y = sy;
        line_sprites[sprites_on_line].tile_index = tile;
        line_sprites[sprites_on_line].attributes = attr;
        sprites_on_line++;
    }

    // Sort by x val and then oam val (both ascending)
    ppu_sort_sprites(line_sprites, sprites_on_line);

    // Draw objects so highest priority on top
    for (int s = sprites_on_line - 1; s >= 0; s--){
        int sx = line_sprites[s].x;
        int sy = line_sprites[s].y;
        u8 tile_index = line_sprites[s].tile_index;
        u8 attr = line_sprites[s].attributes;

        u8 line_y = (u8)(scanline - sy);

        bool y_flip = (attr & 0x40) != 0;
        bool x_flip = (attr & 0x20) != 0;
        bool behind_bg = (attr & 0x80) != 0;
        bool use_palette1 = (attr & 0x10) != 0;

        if (y_flip) line_y = sprite_height - 1 - line_y;

        if (sprite_height == 16) tile_index &= 0xFE;

        u8 tile_to_use = tile_index;
        u8 tile_pixel_y = line_y;

        if (sprite_height == 16 && line_y >= 8){
            tile_to_use = tile_index + 1;
            tile_pixel_y = (u8)(line_y - 8);
        }

        for (int px = 0; px < 8; px++){
            int x = sx + px;
            if (x < 0 || x >= SCREEN_WIDTH){
                continue;
            }

            u8 pixel_x = x_flip ? (u8)(7 - px) : (u8)px;
            u8 colour_id = get_pixel_tile(tile_to_use, pixel_x, tile_pixel_y);

            if (colour_id == 0){
                continue;
            }

            if (behind_bg && bg_raw[x] != 0){
                continue;
            }

            u8 obj_pal = use_palette1 ? ppu->latched_obp1 : ppu->latched_obp0;
            u8 shade = ppu_map_palette(obj_pal, colour_id);
            ppu->frame_buffer[scanline][x] = shade;
        }
    }
}

void ppu_step(PPU* ppu, u8 cycle_count){

    if (!ppu->lcd_enabled){
        return;
    }

    u8 LCDC = memory_read(0xFF40);
    bool LCD_enabled = (LCDC & 0x80) != 0;

    if (!LCD_enabled){
        ppu->dot = 0;
        ppu->current_scanline = 0;
        s_window_line = 0;
        memory_write(0xFF44, 0);
        ppu->mode = PPU_MODE_OAM;
        ppu_set_mode(PPU_MODE_OAM);
        // you can also clear coincidence flag here if you want
        return;
}

    ppu->dot += cycle_count;

    while (ppu->dot >= 456){// Handles wrapping scanlines
        ppu->dot -= 456;
        ppu->current_scanline++;

        if (ppu->current_scanline == 144){
            // VBlank 
            ppu->mode = PPU_MODE_VBLANK;
            ppu_set_mode(PPU_MODE_VBLANK);
            ppu_request_mode_interrupt(PPU_MODE_VBLANK);
            request_interrupt(0x01); // interrupt
        }
        else if (ppu->current_scanline >= 154){
            
            ppu->current_scanline = 0;
            s_window_line = 0;

            ppu->mode = PPU_MODE_OAM;
            ppu_set_mode(PPU_MODE_OAM);
            ppu_request_mode_interrupt(PPU_MODE_OAM);
        }

        // update LY
        memory_write(0xFF44, ppu->current_scanline);
        ppu_check_lyc_interrupt();
    }

    // mode transitions within visible scanlines
    if (ppu->current_scanline < 144){

        if (ppu->dot < 80){
            if (ppu->mode != PPU_MODE_OAM){
                ppu->mode = PPU_MODE_OAM;
                ppu_set_mode(PPU_MODE_OAM);
                ppu_request_mode_interrupt(PPU_MODE_OAM);
                ppu_check_lyc_interrupt();
            }
        }
        else if (ppu->dot < 252){
            if (ppu->mode != PPU_MODE_DRAW){
                ppu->mode = PPU_MODE_DRAW;
                ppu_set_mode(PPU_MODE_DRAW);

                // latch regs at start of mode 3
                ppu->latched_lcdc = memory_read(0xFF40);
                ppu->latched_scx  = memory_read(0xFF43);
                ppu->latched_scy  = memory_read(0xFF42);
                ppu->latched_wx   = memory_read(0xFF4B);
                ppu->latched_wy   = memory_read(0xFF4A);
                ppu->latched_bgp  = memory_read(0xFF47);
                ppu->latched_obp0 = memory_read(0xFF48);
                ppu->latched_obp1 = memory_read(0xFF49);

                ppu_check_lyc_interrupt();
            }
        }
        else{
            if (ppu->mode != PPU_MODE_HBLANK){
                ppu->mode = PPU_MODE_HBLANK;
                ppu_set_mode(PPU_MODE_HBLANK);
                ppu_request_mode_interrupt(PPU_MODE_HBLANK);
                ppu_check_lyc_interrupt();

                ppu_render_scanline(ppu);
            }
        }
    }
}