# Makefile for Forest Fire and Ice Game (Software)

# Compiler
CC = arm-linux-gnueabihf-gcc

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build

# Compiler flags
CFLAGS = -Wall -Wextra -g -I$(INC_DIR) -O2
LDFLAGS = -lm # Link math library if needed

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Executable name
TARGET = $(BUILD_DIR)/game_sw

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Linking complete: $@"

# Compile source files into object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled: $< -> $@"

# Clean build directory
clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory."

.PHONY: all clean

