#include "display.h"
#include <SDL3/SDL.h>
#include <stdio.h>

bool display_init(Display* display, int scale) {
    display->scale = scale;
    
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return false;
    }
    
    display->window = SDL_CreateWindow(
        "Game Boy Emulator",
        SCREEN_WIDTH * scale,
        SCREEN_HEIGHT * scale,
        0
    );
    
    if (!display->window) {
        printf("Failed to create window: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }
    
    display->renderer = SDL_CreateRenderer(display->window, NULL);
    if (!display->renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(display->window);
        SDL_Quit();
        return false;
    }
    
    display->texture = SDL_CreateTexture(
        display->renderer,
        SDL_PIXELFORMAT_XRGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_WIDTH,
        SCREEN_HEIGHT
    );
    
    if (!display->texture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(display->renderer);
        SDL_DestroyWindow(display->window);
        SDL_Quit();
        return false;
    }
    
    printf("SDL3 initialized: %dx%d window (scale %dx)\n", 
           SCREEN_WIDTH * scale, SCREEN_HEIGHT * scale, scale);
    return true;
}

void display_render(Display* display, PPU* ppu) {
    // Game Boy colors (4 shades of gray/green)
    u32 colors[4] = {
        0xFFE0F8D0,  // 0 = Lightest (white-green)
        0xFF88C070,  // 1 = Light gray-green
        0xFF346856,  // 2 = Dark gray-green
        0xFF081820   // 3 = Darkest (black-green)
    };
    
    // Convert PPU framebuffer (2-bit colors) to RGB pixels
    u32 pixels[SCREEN_HEIGHT][SCREEN_WIDTH];
    
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            u8 color_index = ppu->frame_buffer[y][x] & 0x03;  // Ensure 0-3
            pixels[y][x] = colors[color_index];
        }
    }
    
    // Update texture with new frame
    SDL_UpdateTexture(display->texture, NULL, pixels, SCREEN_WIDTH * sizeof(u32));
    
    // Render to screen
    SDL_RenderClear(display->renderer);
    SDL_RenderTexture(display->renderer, display->texture, NULL, NULL);
    SDL_RenderPresent(display->renderer);
}

bool display_handle_input(Display* display) {
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return false;  // User closed window
        }
        
        if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {
                case SDLK_ESCAPE:
                    return false;  // ESC to quit
                    
                case SDLK_RETURN:  // Enter = START
                    // TODO: Set joypad START bit
                    printf("START pressed\n");
                    break;
                    
                case SDLK_RSHIFT:  // Right Shift = SELECT
                    // TODO: Set joypad SELECT bit
                    printf("SELECT pressed\n");
                    break;
                    
                case SDLK_UP:
                case SDLK_DOWN:
                case SDLK_LEFT:
                case SDLK_RIGHT:
                case SDLK_Z:  // A button
                case SDLK_X:  // B button
                    // TODO: Implement joypad input
                    break;
            }
        }
    }
    
    return true;  // Keep running
}

void display_cleanup(Display* display) {
    if (display->texture) SDL_DestroyTexture(display->texture);
    if (display->renderer) SDL_DestroyRenderer(display->renderer);
    if (display->window) SDL_DestroyWindow(display->window);
    SDL_Quit();
    printf("SDL3 cleaned up\n");
}