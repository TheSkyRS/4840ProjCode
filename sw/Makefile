# 顶层目录路径
INCLUDEDIR = include
SRCDIR = src
TESTDIR = test

# 所有源文件
SRCS = $(wildcard $(SRCDIR)/*.c)
TEST_SRCS = $(wildcard $(TESTDIR)/*.c)

# 头文件路径
INCLUDES = -I$(INCLUDEDIR)

# 目标文件列表（.c → .o）
OBJS = $(SRCS:.c=.o)
TEST_OBJS = $(filter-out $(SRCDIR)/main.o, $(OBJS)) $(TEST_SRCS:.c=.o)

# 最终可执行文件名
TARGET = game
TEST_TARGET = test_joypad

# 编译器与选项
CC = gcc
CFLAGS = -Wall -O2 $(INCLUDES)
LDLIBS = -lm  # 添加数学库链接

.PHONY: all clean test

all: $(TARGET)

test: $(TEST_TARGET)

$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) -o $@ $^ $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(TESTDIR)/*.o $(TARGET) $(TEST_TARGET)
