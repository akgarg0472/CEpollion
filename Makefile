CC = gcc
CFLAGS = -Wall -Wextra -std=c17 -D_GNU_SOURCE
TARGET = out/cepollion
SRCS = $(wildcard server/*.c)
OBJS = $(SRCS:server/%.c=out/%.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

out/%.o: server/%.c
	@mkdir -p out
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -rf out
