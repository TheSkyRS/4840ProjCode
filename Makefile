# Makefile for Joypad FPGA Testing

# Compiler settings (can be modified according to FPGA platform)
CC = gcc
# If using cross-compilation, uncomment below and adjust to the appropriate cross-compiler
# CC = arm-linux-gnueabihf-gcc

# Compilation flags
CFLAGS = -Wall -Wextra -O2
# Debug flags, enable when needed
# CFLAGS += -g

# Source file and header file directories
SRC_DIR = sw/src
INC_DIR = sw/include

# Output directory
BUILD_DIR = build

# Source files
SRCS = $(SRC_DIR)/joypad_input.c \
       $(SRC_DIR)/joypad_test.c

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Executable file name
TARGET = $(BUILD_DIR)/joypad_test

# Default target
all: directories $(TARGET)

# Create build directory
directories:
	mkdir -p $(BUILD_DIR)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# Link object files
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# Clean generated files
clean:
	rm -rf $(BUILD_DIR)

# FPGA upload (modify according to specific FPGA platform)
fpga-upload: $(TARGET)
	@echo "Preparing to upload program to FPGA..."	@echo "Please modify this section according to your FPGA platform"
	# Example: scp $(TARGET) root@fpga:/usr/bin/

# Run program
run: $(TARGET)
	./$(TARGET)

# Help information
help:
	@echo "Joypad FPGA Test Makefile"
	@echo "Available commands:"
	@echo "  make all        - Compile program"
	@echo "  make clean      - Clean compiled files"
	@echo "  make fpga-upload - Upload to FPGA (needs configuration)"
	@echo "  make run        - Run program directly"
	@echo "  make help       - Display this help information"

.PHONY: all clean fpga-upload run help directories 