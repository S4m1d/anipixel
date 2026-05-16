CC      = gcc
TARGET  = anipixel

CFLAGS  = -Wall -Wextra -O2 -Isrc $(shell pkg-config --cflags libpng)
LDFLAGS = $(shell pkg-config --libs libpng libcjson)

DEBUG_CFLAGS  = -Wall -Wextra -g -fsanitize=address -Isrc $(shell pkg-config --cflags libpng)
DEBUG_LDFLAGS = $(shell pkg-config --libs libpng libcjson) -fsanitize=address

SRC = $(wildcard src/*.c)
OBJ       = $(patsubst src/%.c, build/release/%.o, $(SRC))
DEBUG_OBJ = $(patsubst src/%.c, build/debug/%.o, $(SRC))

build/release/$(TARGET): $(OBJ)
	@mkdir -p build/release
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

build/debug/$(TARGET): $(DEBUG_OBJ)
	@mkdir -p build/debug
	$(CC) $(DEBUG_CFLAGS) -o $@ $^ $(DEBUG_LDFLAGS)

build/release/%.o: src/%.c
	@mkdir -p build/release
	$(CC) $(CFLAGS) -c $< -o $@

build/debug/%.o: src/%.c
	@mkdir -p build/debug
	$(CC) $(DEBUG_CFLAGS) -c $< -o $@

all: build/release/$(TARGET)
debug: build/debug/$(TARGET)

clean:
	rm -rf build

.PHONY: all debug clean
