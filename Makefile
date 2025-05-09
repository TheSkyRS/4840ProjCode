# Makefile for Joypad FPGA Testing

# 编译器设置 (可根据FPGA平台修改)
CC = gcc
# 如果使用交叉编译，取消下面的注释并调整为合适的交叉编译器
# CC = arm-linux-gnueabihf-gcc

# 编译标志
CFLAGS = -Wall -Wextra -O2
# 调试标志，需要时启用
# CFLAGS += -g

# 源文件目录和头文件目录
SRC_DIR = sw/src
INC_DIR = sw/include

# 输出目录
BUILD_DIR = build

# 源文件
SRCS = $(SRC_DIR)/joypad_input.c \
       $(SRC_DIR)/joypad_test.c

# 目标文件
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

# 可执行文件名
TARGET = $(BUILD_DIR)/joypad_test

# 默认目标
all: directories $(TARGET)

# 创建构建目录
directories:
	mkdir -p $(BUILD_DIR)

# 编译源文件
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -I$(INC_DIR) -c $< -o $@

# 链接目标文件
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $@

# 清理生成的文件
clean:
	rm -rf $(BUILD_DIR)

# FPGA烧录 (根据具体FPGA平台修改)
fpga-upload: $(TARGET)
	@echo "准备将程序烧录到FPGA..."
	@echo "请根据您的FPGA平台修改此部分"
	# 示例: scp $(TARGET) root@fpga:/usr/bin/

# 运行程序
run: $(TARGET)
	./$(TARGET)

# 帮助信息
help:
	@echo "Joypad FPGA 测试 Makefile"
	@echo "可用命令:"
	@echo "  make all        - 编译程序"
	@echo "  make clean      - 清理编译文件"
	@echo "  make fpga-upload - 上传到FPGA (需要配置)"
	@echo "  make run        - 直接运行程序"
	@echo "  make help       - 显示此帮助信息"

.PHONY: all clean fpga-upload run help directories 