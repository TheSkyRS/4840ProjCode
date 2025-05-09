# 简化版Makefile - 只编译joypad相关文件

# 编译器
CC = gcc

# 编译选项
CFLAGS = -Wall -O2

# 头文件目录
INCLUDES = -I./sw/include

# 源文件
SOURCES = sw/src/joypad_input.c sw/src/joypad_test.c

# 目标文件
TARGET = joypad_test

# 默认目标 - 编译程序
all: $(TARGET)

# 链接目标文件生成可执行文件
$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(INCLUDES) $(SOURCES) -o $(TARGET)

# 清理生成的文件
clean:
	rm -f $(TARGET)

# 运行程序
run: $(TARGET)
	./$(TARGET)

# 显示帮助信息
help:
	@echo "简化版Makefile - 只编译joypad相关文件"
	@echo "命令:"
	@echo "  make       - 编译程序"
	@echo "  make clean - 清理编译文件"
	@echo "  make run   - 运行程序" 
	@echo "  make help  - 显示此帮助信息"

.PHONY: all clean run help 