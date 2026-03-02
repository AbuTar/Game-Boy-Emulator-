# Compiler and flags
CC = gcc

SDL3_PREFIX := C:/SDL3/x86_64-w64-mingw32
SDL3_INC    := -I$(SDL3_PREFIX)/include
SDL3_LIB    := -L$(SDL3_PREFIX)/lib
SDL3_LINK   := -lSDL3

CFLAGS  = -Wall -Wextra -std=c11 -I./src -I./src/core -I./src/ui $(SDL3_INC) -MMD -MP
LDFLAGS = $(SDL3_LIB) $(SDL3_LINK)

# Directories
SRC_DIR = src
CORE_DIR = $(SRC_DIR)/core
UI_DIR = $(SRC_DIR)/ui
BIN_DIR = bin
BUILD_DIR = build

# Target executable
TARGET = $(BIN_DIR)/emulator.exe

# Source files
CORE_SOURCES = $(CORE_DIR)/cpu.c \
               $(CORE_DIR)/cpu_instructions.c \
               $(CORE_DIR)/cpu_helpers.c \
               $(CORE_DIR)/memory.c \
			   $(CORE_DIR)/ppu_helpers.c \
               $(CORE_DIR)/ppu.c

UI_SOURCES = $(UI_DIR)/display.c \
             $(UI_DIR)/input.c

MAIN_SOURCE = $(SRC_DIR)/main.c

SOURCES = $(CORE_SOURCES) $(UI_SOURCES) $(MAIN_SOURCE)

# Object files (same names as .c files but with .o extension)
# Put object files under build/ mirroring src/ structure
OBJECTS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SOURCES))
DEPS    = $(OBJECTS:.o=.d)

# Default target
all: $(TARGET)

# Link all object files into executable
$(TARGET): $(OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile .c files to .o files
# Compile into build/ and create folders as needed
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Auto-include dependency files
-include $(DEPS)

# Clean build artifacts
clean:
	rm -rf $(BUILD_DIR) $(TARGET)
	@echo "Clean complete"

# Rebuild everything
rebuild: clean all

# Run the emulator (requires ROM path)
run: $(TARGET)
	./$(TARGET) $(ROM)

.PHONY: all clean rebuild run