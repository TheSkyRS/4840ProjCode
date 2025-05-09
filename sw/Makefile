# Makefile for Forest Fire and Ice Game (Software)

# === 编译器设置 ===
CC = arm-linux-gnueabihf-gcc

# === 目录结构 ===
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
PACKAGE_DIR = package

# === 编译参数 ===
CFLAGS = -Wall -Wextra -g -I$(INC_DIR) -Ivga_demo -O2
LDFLAGS = -lm

# === 主程序源文件（排除 joypad_test.c）===
SRCS = $(wildcard $(SRC_DIR)/*.c)
SRCS := $(filter-out $(SRC_DIR)/joypad_test.c, $(SRCS))
OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(SRCS))
TARGET = $(BUILD_DIR)/game_sw

# === Joypad 相关 ===
JOYPAD_SRCS = $(SRC_DIR)/joypad_input.c
JOYPAD_TEST_SRCS = $(SRC_DIR)/joypad_test.c
JOYPAD_HEADERS = $(INC_DIR)/joypad_input.h $(INC_DIR)/input_handler.h
JOYPAD_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(JOYPAD_SRCS))
JOYPAD_TEST_OBJS = $(patsubst $(SRC_DIR)/%.c, $(BUILD_DIR)/%.o, $(JOYPAD_TEST_SRCS))
JOYPAD_TEST_TARGET = $(BUILD_DIR)/joypad_test
JOYPAD_PACKAGE = $(PACKAGE_DIR)/joypad_module.tar.gz

# === 默认目标：构建主程序 ===
all: $(TARGET)

# === 主程序链接 ===
$(TARGET): $(OBJS)
	@mkdir -p $(@D)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Linking complete: $@"

# === 通用编译规则 ===
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled: $< -> $@"

# === 构建 Joypad 测试程序 ===
joypad_test: $(JOYPAD_OBJS) $(JOYPAD_TEST_OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(JOYPAD_OBJS) $(JOYPAD_TEST_OBJS) -o $(JOYPAD_TEST_TARGET) $(LDFLAGS)
	@echo "Built joypad test program: $(JOYPAD_TEST_TARGET)"

# === 打包 Joypad 模块（可选）===
joypad_package: joypad_test
	@mkdir -p $(PACKAGE_DIR)/{include,src,bin}
	@cp $(JOYPAD_HEADERS) $(PACKAGE_DIR)/include/
	@cp $(JOYPAD_SRCS) $(JOYPAD_TEST_SRCS) $(PACKAGE_DIR)/src/
	@cp $(JOYPAD_TEST_TARGET) $(PACKAGE_DIR)/bin/
	@tar -czf $(JOYPAD_PACKAGE) -C $(PACKAGE_DIR) .
	@echo "Packaged joypad module: $(JOYPAD_PACKAGE)"

# === 清理 ===
clean:
	@rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory."

clean_package:
	@rm -rf $(PACKAGE_DIR)
	@echo "Cleaned package directory."

clean_all: clean clean_package
	@echo "Cleaned all build and package files."

.PHONY: all clean clean_package clean_all joypad_test joypad_package
