#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu.h"
#include "memory.h"
#include "ppu.h"
#include "ppu.h"
#include "display.h"


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
    Display display;  // Add display
    
    memory_init();
    cpu_init(&cpu);
    ppu_init(&ppu);

    if (argc < 2){ 
        printf("Usage: emulator.exe <rom.gb>\n");
        return 1;
    }

    if (!load_rom(argv[1])){
        return 1;
    }

    // Initialize SDL display
    if (!display_init(&display, 4)) {  // 4x scale = 640×576 window
        return 1;
    }

    printf("\nRUNNING ROM (press ESC to quit)...\n");

    u8 previous_scanline = 0;
    u64 frame_count = 0;
    bool running = true;

    while (running && cpu.cycles < 1000000000){
        // Handle input (returns false if user wants to quit)
        running = display_handle_input(&display);
        
        // Execute CPU instruction
        cpu_step(&cpu, &ppu);

        // Serial output (Blargg tests)
        u8 output = memory_read(0xFF01);
        if (output != 0){
            printf("%c", output);
            fflush(stdout);
            memory_write(0xFF01, 0);        
        }

        // Render frame when VBlank starts
        u8 current_scanline = memory_read(0xFF44);
        if (current_scanline == 0 && previous_scanline >= 144){
            frame_count++;
            
            // Render to SDL window
            display_render(&display, &ppu);
            
            // Optional: Print debug info occasionally
            if (frame_count % 60 == 0) {
                printf("[Frame %llu] Rendered\n", frame_count);
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
    display_cleanup(&display);
    clear_memory();
    return 0;
}


// int main(int argc, char* argv[]){ // Usage : emulator.exe test.gb - takes 2 arguments
//     CPU cpu;
//     PPU ppu;
//     memory_init();
//     cpu_init(&cpu);
//     ppu_init(&ppu);

//     if (argc < 2){ 
//         printf("Usage: emulator.exe <rom.gb>\n");
//         return 1;
//     }

//     if (!load_rom(argv[1])){ // argv[1] is the ROM file path
//         return 1;
//     }

//     printf("\n=== DETAILED GRAPHICS DEBUG ===\n");
//     printf("LCDC (0xFF40): 0x%02X\n", memory_read(0xFF40));
//     printf("  Bit 7 (LCD Enable):  %d\n", (memory_read(0xFF40) >> 7) & 1);
//     printf("  Bit 0 (BG Enable):   %d\n", memory_read(0xFF40) & 1);
//     printf("SCY  (0xFF42): 0x%02X\n", memory_read(0xFF42));
//     printf("SCX  (0xFF43): 0x%02X\n", memory_read(0xFF43));
//     printf("BGP  (0xFF47): 0x%02X\n", memory_read(0xFF47));

//     printf("\nVRAM Tile 0 (0x8000-0x800F):\n");
//     for (int i = 0; i < 16; i++) {
//         printf("%02X ", memory_read(0x8000 + i));
//     }

//     printf("\n\nVRAM Tile 1 (0x8010-0x801F):\n");
//     for (int i = 0; i < 16; i++) {
//         printf("%02X ", memory_read(0x8010 + i));
//     }

//     printf("\n\nBackground Map (0x9800-0x981F - first 32 bytes):\n");
//     for (int i = 0; i < 32; i++) {
//         printf("%02X ", memory_read(0x9800 + i));
//         if ((i + 1) % 16 == 0) printf("\n");
//     }

//     printf("\n");

//     printf("RUNNING ROM:.......\n");

//     u8 previous_scanline = 0;
//     u64 frame_count = 0; 

//     while (!cpu.isHalted && cpu.cycles < 1000000000){
//         cpu_step(&cpu, &ppu);

//         u8 output = memory_read(0xFF01);
//         if (output != 0){
//             printf("%c", output);
//             fflush(stdout);
//             memory_write(0xFF01, 0);        
//         }

//         u8 current_scanline = memory_read(0xFF44);
//         if (current_scanline == 0 && previous_scanline >= 144){
//             frame_count ++;

//             if (frame_count % 60 == 0) {
//                 printf("[Frame %llu] Line 0 pixels: ", frame_count);

//                 for (int i = 0; i < 10; i++){
//                     printf("%d ", ppu.frame_buffer[0][i]);
//                 }

//                 printf("\n");

//                 printf("VRAM at frame: ");
//                 for (int i = 0; i < 16; i++) {
//                     printf("%02X ", memory_read(0x8000 + i));
//                 }
//                 printf("\n");
//             }
//         }

//         previous_scanline = current_scanline;
//     }

//     printf("\n\n=== Execution Complete ===\n");
//     printf("Total cycles: %llu\n", cpu.cycles);
//     printf("Total frames: %llu\n", frame_count); 
//     printf("Halted: %s\n", cpu.isHalted ? "Yes" : "No (timeout)");
//     printf("Final PC: 0x%04X\n", cpu.pc);
    
//     clear_memory();
//     return 0;

    
// }
