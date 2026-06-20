#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu.h"
#include "memory.h"
#include "ppu.h"
#include "ppu.h"
#include "ui/display.h"
#include "ui/input.h"
#include "boot.h"
#include "sram.h"
#include "ui/audio.h"
#include "apu.h"
#include <SDL3/SDL.h>



bool load_rom(const char* path){ // Passing in path of file

    FILE* file = fopen(path, "rb");
    if (!file){
        printf("Failed to open ROM: %s\n", path);
        return false;
    }

    // FILE SIZE

    fseek(file, 0, SEEK_END); // Move pointer to file end
    long size = ftell(file); // Final Position = file size
    rewind(file); // Go back to start

    u8* rom_buffer = malloc(size); // Need to read file data before copying to gb memory
    if (!rom_buffer){
        printf("Failed to allocate memory for ROM\n");
        fclose(file);
        return false;
    }

    size_t read_bytes = fread(rom_buffer, 1, size, file); // Usage: fread( <destination>, <size of element>, <number of elements>, <source>)
    fclose(file);

    // READ CHECK

    if (read_bytes != size) {
        printf("Failed to read entire ROM\n");
        free(rom_buffer);
        return false;
    }

    memory_load_rom(rom_buffer, size); // Copies data from buffer to gb memory
    free(rom_buffer);

    printf("ROM loaded: %s (%ld bytes)\n", path, size);
    return true;
}

int main(int argc, char* argv[]){
    CPU cpu;
    PPU ppu;
    APU apu;
    Display display;
    Audio audio;
    
    memory_init();
    boot_init();
    cpu_init(&cpu);
    ppu_init(&ppu);
    apu_init(&apu);
    memory_attach_apu(&apu);
    input_init();

    if (argc < 2){ 
        printf("Usage: emulator.exe <rom.gb>\n");
        return 1;
    }

    if (!load_rom(argv[1])){
        return 1;
    }

    sram_init(argv[1]);


    // Initialize SDL display
    if (!display_init(&display, 4)) {  // 4x scale = 640×576 window
        return 1;
    }

    // Initialize SD: Audio
    if (!audio_init(&audio)) {  // 4x scale = 640×576 window
        return 1;
    }

    printf("\nRUNNING ROM (press ESC to quit)...\n");

    u8 previous_scanline = 0;
    u64 frame_count = 0;
    bool running = true;
    float speed = 1.0f;  // Not used, just for function signature
    const double TARGET_FRAME_TIME_MS = 1000.0 / 59.7275;  // exact GB refresh rate
    Uint64 perf_freq = SDL_GetPerformanceFrequency();
    Uint64 frame_start = SDL_GetPerformanceCounter();

    while (running){
        // Handle input (returns false if user wants to quit)
        running = display_handle_input(&display);
        
        // Execute CPU instruction
        cpu_step(&cpu, &ppu, &apu);

        // Render frame when VBlank starts
        u8 current_scanline = memory_read(0xFF44);
        if (current_scanline == 0 && previous_scanline >= 144){
            frame_count++;
            
            display_render(&display, &ppu);
            audio_pump(&audio, &apu);

            // Frame pacing: sleep until the next frame is due
            Uint64 now = SDL_GetPerformanceCounter();
            double elapsed_ms = (double)(now - frame_start) * 1000.0 / perf_freq;
            double remaining_ms = TARGET_FRAME_TIME_MS - elapsed_ms;
            if (remaining_ms > 1.0) {
                SDL_Delay((Uint32)(remaining_ms - 1.0));  // coarse sleep
            }
            // Spin for sub-millisecond precision after the sleep
            do {
                now = SDL_GetPerformanceCounter();
                elapsed_ms = (double)(now - frame_start) * 1000.0 / perf_freq;
            } while (elapsed_ms < TARGET_FRAME_TIME_MS);

            frame_start = SDL_GetPerformanceCounter();
            
            if (frame_count % 60 == 0) {
                printf("[Frame %llu] | PC: 0x%04X | A: 0x%02X | LCDC: 0x%02X\n", frame_count, cpu.pc, cpu.A, memory_read(0xFF40));
            }
        }

        previous_scanline = current_scanline;
    }

    printf("\n\n=== Execution Complete ===\n");
    printf("Total cycles: %llu\n", cpu.cycles);
    printf("Total frames: %llu\n", frame_count);
    printf("Halted: %s\n", cpu.isHalted ? "Yes" : "No");
    printf("Final PC: 0x%04X\n", cpu.pc);
    
    // Cleanup
    sram_cleanup();
    audio_cleanup(&audio);
    display_cleanup(&display);
    clear_memory();
    return 0;
}
