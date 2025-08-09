CC = clang
CFLAGS = -Wall -Wextra -O2 -std=c11
LDFLAGS =
TARGET = tg

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)