# Makefile for Forest Fire and Ice Game (Software)

# Compiler
CC = arm-linux-gnueabihf-gcc

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
PACKAGE_DIR = package

# Compiler flags
CFLAGS = -Wall -Wextra -g -I$(INC_DIR) -O2
LDFLAGS = -lm # Link math library if needed

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c)

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# Executable name
TARGET = $(BUILD_DIR)/game_sw

# Joypad module specific files
JOYPAD_SRCS = $(SRC_DIR)/joypad_input.c
JOYPAD_TEST_SRCS = $(SRC_DIR)/joypad_test.c
JOYPAD_HEADERS = $(INC_DIR)/joypad_input.h $(INC_DIR)/input_handler.h
JOYPAD_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(JOYPAD_SRCS))
JOYPAD_TEST_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(JOYPAD_TEST_SRCS))
JOYPAD_TEST_TARGET = $(BUILD_DIR)/joypad_test

# Joypad package name
JOYPAD_PACKAGE = $(PACKAGE_DIR)/joypad_module.tar.gz

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

# Build just the joypad test program
joypad_test: $(JOYPAD_OBJS) $(JOYPAD_TEST_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(JOYPAD_OBJS) $(JOYPAD_TEST_OBJS) -o $(JOYPAD_TEST_TARGET) $(LDFLAGS)
	@echo "Built joypad test program: $(JOYPAD_TEST_TARGET)"

# Package the joypad module
joypad_package: joypad_test
	@mkdir -p $(PACKAGE_DIR)
	@mkdir -p $(PACKAGE_DIR)/include
	@mkdir -p $(PACKAGE_DIR)/src
	@mkdir -p $(PACKAGE_DIR)/bin
	@cp $(JOYPAD_HEADERS) $(PACKAGE_DIR)/include/
	@cp $(JOYPAD_SRCS) $(JOYPAD_TEST_SRCS) $(PACKAGE_DIR)/src/
	@cp $(JOYPAD_TEST_TARGET) $(PACKAGE_DIR)/bin/
	@tar -czf $(JOYPAD_PACKAGE) -C $(PACKAGE_DIR) .
	@echo "Packaged joypad module: $(JOYPAD_PACKAGE)"

# Clean build directory
clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory."

# Clean package directory
clean_package:
	@rm -rf $(PACKAGE_DIR)
	@echo "Cleaned package directory."

# Clean all
clean_all: clean clean_package
	@echo "Cleaned all directories."

.PHONY: all clean clean_package clean_all joypad_test joypad_package

