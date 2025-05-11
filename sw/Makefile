# 顶层目录路径
INCLUDEDIR = include
SRCDIR = src

# 所有源文件
SRCS = $(wildcard $(SRCDIR)/*.c)

# 头文件路径
INCLUDES = -I$(INCLUDEDIR)

# 目标文件列表（.c → .o）
OBJS = $(SRCS:.c=.o)

# 最终可执行文件名
TARGET = game

# 编译器与选项
CC = gcc
CFLAGS = -Wall -O2 $(INCLUDES)
LDLIBS = -lm  # 添加数学库链接

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^

# 每个 .c → .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(TARGET)
