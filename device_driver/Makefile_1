# Makefile for FPGA Sprite Engine Driver and User Applications

# Cross-compiler for ARM
CC = arm-linux-gnueabihf-gcc

# Directories
BUILD_DIR = build
KERNEL_DIR ?= /lib/modules/$(shell uname -r)/build

# Compiler flags
CFLAGS = -Wall -Wextra -g -I. -O2
LDFLAGS = -lm # Link math library if needed

# Kernel module source
MODULE_NAME = fpga_sprite
MODULE_OBJ = $(MODULE_NAME).o

# User application test source
TEST_SRC = fpga_sprite_test.c
TEST_TARGET = $(BUILD_DIR)/fpga_sprite_test

# Default target
all: kernel_module user_app

# Kernel module targets
obj-m := $(MODULE_OBJ)

# Build kernel module
kernel_module:
	@echo "Building kernel module: $(MODULE_NAME)"
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules

# Build user space test application
$(TEST_TARGET): $(TEST_SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)
	@echo "Built user test application: $@"

user_app: $(TEST_TARGET)

# Clean build files
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) clean
	@echo "Clean complete."

# Install the kernel module
install:
	@echo "Installing kernel module..."
	$(MAKE) -C $(KERNEL_DIR) M=$(PWD) modules_install
	depmod -a
	@echo "Installation complete."

# Load the kernel module
load:
	@echo "Loading kernel module..."
	insmod $(MODULE_NAME).ko

# Unload the kernel module
unload:
	@echo "Unloading kernel module..."
	rmmod $(MODULE_NAME)

.PHONY: all kernel_module user_app clean install load unload

