#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cpu.h"
#include "memory.h"


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



int main(int argc, char* argv[]){ // Usage : emulator.exe test.gb - takes 2 arguments
    CPU cpu;
    memory_init();
    cpu_init(&cpu);

    if (argc < 2){ 
        printf("Usage: emulator.exe <rom.gb>\n");
        return 1;
    }

    if (!load_rom(argv[1])){ // argv[1] is the ROM file path
        return 1;
    }

    printf("RUNNING ROM:.......\n");

    while (!cpu.isHalted && cpu.cycles < 1000000000){
        cpu_step(&cpu);

        u8 output = memory_read(0xFF01);
        if (output != 0){
            printf("%c", output);
            fflush(stdout);
            memory_write(0xFF01, 0);        
        }
    }

    // Serial Check 
    for (int i = 0; i < 100; i++) {  // Check a few more times
        u8 output = memory_read(0xFF01);
        if (output != 0) {
            printf("%c", output);
            fflush(stdout);
            memory_write(0xFF01, 0);
        }
    }

    printf("\n\n=== Execution Complete ===\n");
    printf("Total cycles: %llu\n", cpu.cycles);
    printf("Halted: %s\n", cpu.isHalted ? "Yes" : "No (timeout)");
    printf("Final PC: 0x%04X\n", cpu.pc);

    return 0;
}
