#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include "sram.h"
#include "memory.h"

static char* sram_save_name = NULL;
static bool sram_is_dirty = false;

void sram_init(const char* rom_path) {
    if (!rom_path){
        return;
    }

    #ifdef _WIN32 // Creates saves folder if it doens;t exist
    mkdir("saves");
    #else
        mkdir("saves", 0755);
    #endif

    // Extract just the filename from the path
    const char* filename = strrchr(rom_path, '/');
    if (!filename) {
        filename = strrchr(rom_path, '\\');
    }

    if (filename) {
        filename++;
    } 
    
    else {
        filename = rom_path;
    }
    
    const char* dot = strrchr(rom_path, '.');
    size_t base_len = dot ? (size_t)(dot - filename) : strlen(filename);
    
    sram_save_name = malloc(7 + base_len + 5);
    if (!sram_save_name){
        return;
    }
    
    strcpy(sram_save_name,"saves/");
    strncat(sram_save_name, filename, base_len);
    strcat(sram_save_name, ".sav");
    
    printf("Save file: %s\n", sram_save_name);
    
    sram_load();
}

void sram_load(void) {
    if (!sram_save_name){
        return;
    }

    FILE* file = fopen(sram_save_name, "rb");
    if (!file) {
        printf("No existing save\n");
        return;
    }
    
    u8 sram_data[0x8000];
    size_t read = fread(sram_data, 1, 0x8000, file);
    fclose(file);
    
    if (read > 0) {
        memory_load_sram(sram_data, read);
        printf("Successfully Loaded Save (%zu bytes)\n", read);
    }
}

void sram_save(void) {
    if (!sram_save_name){
        return;
    }

    if (!sram_is_dirty){
        return; // Only save when sram dirty
    }
    
    u8 sram_data[0x8000];
    size_t sram_size = memory_get_sram(sram_data);
    
    if (sram_size == 0){
        return;
    }
    
    FILE* file = fopen(sram_save_name, "wb");
    if (!file) {
        printf("Failed to write to save\n");
        return;
    }
    
    fwrite(sram_data, 1, sram_size, file);
    fclose(file);
    
    printf("Succesfully saved game (%zu bytes)\n", sram_size);
    sram_is_dirty = false;
}

void sram_cleanup(void) {
    // Save on exit if dirty
    if (sram_is_dirty) {
        printf("\n Saving game progress...\n");
        sram_save();
    }
    
    if (sram_save_name) {
        free(sram_save_name);
        sram_save_name = NULL;
    }
}

void sram_mark_dirty(void) {
    if (!sram_is_dirty) {
        sram_is_dirty = true;
        printf("Game has not been saved\n");
    }
}
