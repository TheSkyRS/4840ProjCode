# Makefile for Forest Fire and Ice Game (Software)

# === 编译器设置 ===
CC = arm-linux-gnueabihf-gcc

# === 目录结构 ===
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
PACKAGE_DIR = package

# === 编译参数 ===
CFLAGS = -Wall -Wextra -g -I$(INC_DIR) -O2
LDFLAGS = -lm

# 源文件（排除内核模块 + 测试文件）
SRCS = $(wildcard $(SRC_DIR)/*.c)
SRCS := $(filter-out $(SRC_DIR)/vga_top.c $(SRC_DIR)/joypad_test.c, $(SRCS))
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))
TARGET = $(BUILD_DIR)/game_sw


# === Joypad 相关 ===
JOYPAD_SRCS = $(SRC_DIR)/joypad_input.c
JOYPAD_TEST_SRCS = $(SRC_DIR)/joypad_test.c
JOYPAD_HEADERS = $(INC_DIR)/joypad_input.h $(INC_DIR)/input_handler.h
JOYPAD_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(JOYPAD_SRCS))
JOYPAD_TEST_OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(JOYPAD_TEST_SRCS))
JOYPAD_TEST_TARGET = $(BUILD_DIR)/joypad_test
JOYPAD_PACKAGE = $(PACKAGE_DIR)/joypad_module.tar.gz

# === 默认构建用户程序 ===
all: $(TARGET)

# === 链接主程序 ===
$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Linking complete: $@"

# === 编译每个 .c 文件为 .o ===
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled: $< -> $@"

# === 构建 joypad 测试程序 ===
joypad_test: $(JOYPAD_OBJS) $(JOYPAD_TEST_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(JOYPAD_OBJS) $(JOYPAD_TEST_OBJS) -o $(JOYPAD_TEST_TARGET) $(LDFLAGS)
	@echo "Built joypad test program: $(JOYPAD_TEST_TARGET)"

# === 打包 joypad 模块 ===
joypad_package: joypad_test
	@mkdir -p $(PACKAGE_DIR)/{include,src,bin}
	@cp $(JOYPAD_HEADERS) $(PACKAGE_DIR)/include/
	@cp $(JOYPAD_SRCS) $(JOYPAD_TEST_SRCS) $(PACKAGE_DIR)/src/
	@cp $(JOYPAD_TEST_TARGET) $(PACKAGE_DIR)/bin/
	@tar -czf $(JOYPAD_PACKAGE) -C $(PACKAGE_DIR) .
	@echo "Packaged joypad module: $(JOYPAD_PACKAGE)"

# === 构建 vga_top.ko 内核模块 ===
VGA_TOP_KO := $(SRC_DIR)/vga_top.ko
KERNEL_BUILD := /lib/modules/$(shell uname -r)/build

vga_top.ko:
	$(MAKE) -C $(KERNEL_BUILD) M=$(abspath $(SRC_DIR)) modules
	@echo "Built kernel module: $(VGA_TOP_KO)"

vga_top_clean:
	$(MAKE) -C $(KERNEL_BUILD) M=$(abspath $(SRC_DIR)) clean
	@rm -f $(VGA_TOP_KO)
	@echo "Cleaned vga_top kernel module."

# === 清理指令 ===
clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory."

clean_package:
	@rm -rf $(PACKAGE_DIR)
	@echo "Cleaned package directory."

clean_all: clean clean_package
	@echo "Cleaned all directories."

.PHONY: all clean clean_package clean_all joypad_test joypad_package vga_top.ko vga_top_clean
