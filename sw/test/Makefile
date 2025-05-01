CC = gcc
CFLAGS = -Wall -Wextra -I../include
LDFLAGS = -lm

TEST_SRCS = test_collision.c ../src/collision_detection.c
TEST_OBJS = $(TEST_SRCS:.c=.o)
TEST_TARGET = test_collision

.PHONY: all clean test

all: $(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS)
	$(CC) $(TEST_OBJS) -o $(TEST_TARGET) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(TEST_OBJS) $(TEST_TARGET) 