#include <SDL3/SDL.h>
#include <stdio.h>
#include "display.h"
#include "input.h"

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
    
    // Update texture with new frame then render to screen
    SDL_UpdateTexture(display->texture, NULL, pixels, SCREEN_WIDTH * sizeof(u32));
    SDL_RenderClear(display->renderer);
    SDL_RenderTexture(display->renderer, display->texture, NULL, NULL);
    SDL_RenderPresent(display->renderer);
}

bool display_handle_input(Display* display) {
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_EVENT_QUIT) {
            return false;
        }
        
        if (event.type == SDL_EVENT_KEY_DOWN) {
            switch (event.key.key) {

                // ESCAPE, START, SELECT
                case SDLK_ESCAPE:
                    return false;
                    
                case SDLK_RETURN:
                    input_button_press(BUTTON_START);
                    printf("START pressed\n");
                    break;
                
                case SDLK_RSHIFT:
                    input_button_press(BUTTON_SELECT);
                    printf("SELECT pressed\n");
                    break;


                // D-PAD
                case SDLK_LEFT:
                    input_button_press(BUTTON_LEFT);
                    printf("LEFT pressed\n");
                    break;
                
                case SDLK_RIGHT:
                    input_button_press(BUTTON_RIGHT);
                    printf("RIGHT pressed\n");
                    break;

                case SDLK_UP:
                    input_button_press(BUTTON_UP);
                    printf("UP pressed\n");
                    break;

                case SDLK_DOWN:
                    input_button_press(BUTTON_DOWN);
                    printf("DOWN pressed\n");
                    break;


                // A, B BUTTONS
                case SDLK_Z:
                    input_button_press(BUTTON_A);
                    printf("A pressed\n");
                    break;

                case SDLK_X:
                    input_button_press(BUTTON_B);
                    printf("B pressed\n");
                    break;
            }
        }

        if (event.type == SDL_EVENT_KEY_UP) {
            switch (event.key.key) {
                
                // START, SELECT
                case SDLK_RETURN:
                    input_button_release(BUTTON_START);
                    printf("START pressed\n");
                    break;
                    
                case SDLK_RSHIFT:
                    input_button_release(BUTTON_SELECT);
                    printf("SELECT pressed\n");
                    break;


                // D-PAD
                case SDLK_LEFT:
                    input_button_release(BUTTON_LEFT);
                    printf("LEFT pressed\n");
                    break;
                
                case SDLK_RIGHT:
                    input_button_release(BUTTON_RIGHT);
                    printf("RIGHT pressed\n");
                    break;

                case SDLK_UP:
                    input_button_release(BUTTON_UP);
                    printf("UP pressed\n");
                    break;

                case SDLK_DOWN:
                    input_button_release(BUTTON_DOWN);
                    printf("DOWN pressed\n");
                    break;


                // A, B BUTTONS
                case SDLK_Z:
                    input_button_release(BUTTON_A);
                    printf("A pressed\n");
                    break;

                case SDLK_X:
                    input_button_release(BUTTON_B);
                    printf("B pressed\n");
                    break;
            }
        }
    }
    
    // Update the Joypad Register
    input_update_state();
    return true;
}

void display_cleanup(Display* display) {
    if (display->texture) SDL_DestroyTexture(display->texture);
    if (display->renderer) SDL_DestroyRenderer(display->renderer);
    if (display->window) SDL_DestroyWindow(display->window);
    SDL_Quit();
    printf("SDL3 cleaned up\n");
}