CC=gcc
CFLAGS=-Wall -Wextra -std=c99 -pedantic -Iinclude -g
LDFLAGS=-fsanitize=address -lssl -lcrypto

SRC=$(wildcard src/*.c)
OBJ=$(SRC:src/%.c=bin/%.o)
OUT=bin
BIN=$(OUT)/stegobmp

all: dirs $(BIN)

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $^ $(LDFLAGS)

bin/%.o: src/%.c
	$(CC) -o $@ -c $< $(CFLAGS)

dirs:
	mkdir -p $(OUT)

clean:
	rm -rf $(OUT)
