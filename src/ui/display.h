#ifndef DISPLAY_H
#define DISPLAY_H
#include <SDL3/SDL.h>
#include "ppu.h"

typedef struct{
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    int scale;
} Display;

bool display_init(Display* display, int scale);
void display_render(Display* display, PPU* ppu);
bool display_handle_input(Display* display);  // Returns false if user wants to quit
void display_cleanup(Display* display);

#endif