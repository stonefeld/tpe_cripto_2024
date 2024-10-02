CC=gcc
CFLAGS=-Wall -Wextra -Werror -std=c99 -pedantic

SRC=$(wildcard src/*.c)
OBJ=$(SRC:src/%.c=bin/%.o)
OUT=bin
BIN=$(OUT)/stegobmp

all: dirs $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $^

bin/%.o: src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

dirs:
	mkdir -p $(OUT)

clean:
	rm -rf $(OUT)
