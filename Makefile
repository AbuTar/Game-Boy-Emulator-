# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -I./src
LDFLAGS = -lSDL3

# Directories
SRC_DIR = src
CORE_DIR = $(SRC_DIR)/core
UI_DIR = $(SRC_DIR)/ui
BIN_DIR = bin

# Target executable
TARGET = $(BIN_DIR)/emulator

# Source files
CORE_SOURCES = $(CORE_DIR)/cpu.c \
               $(CORE_DIR)/cpu_instructions.c \
               $(CORE_DIR)/cpu_helpers.c \
               $(CORE_DIR)/memory.c \
               $(CORE_DIR)/ppu.c

UI_SOURCES = $(UI_DIR)/display.c

MAIN_SOURCE = $(SRC_DIR)/main.c

# Object files (same names as .c files but with .o extension)
CORE_OBJECTS = $(CORE_SOURCES:.c=.o)
UI_OBJECTS = $(UI_SOURCES:.c=.o)
MAIN_OBJECT = $(MAIN_SOURCE:.c=.o)

ALL_OBJECTS = $(CORE_OBJECTS) $(UI_OBJECTS) $(MAIN_OBJECT)

# Default target
all: $(TARGET)

# Link all object files into executable
$(TARGET): $(ALL_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(ALL_OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Compile .c files to .o files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	rm -f $(ALL_OBJECTS) $(TARGET)
	@echo "Clean complete"

# Rebuild everything
rebuild: clean all

# Run the emulator (requires ROM path)
run: $(TARGET)
	./$(TARGET) $(ROM)

.PHONY: all clean rebuild run