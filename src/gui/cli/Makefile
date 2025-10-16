# CLI GUI Makefile - builds the frontend executable
CC = gcc
CFLAGS = -std=c11 -Wall -Werror -Wextra
MEM_FLAGS = -g
NCURSES_FLAGS = -lncurses -lmenu

# Paths (set by root Makefile)
BIN ?= ./
LIBS_DIR ?= ./
GAME_NAME = BrickGameCLI
GAME = $(BIN)$(GAME_NAME)

# Source files
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)

.PHONY: all clean

all: $(GAME)

$(GAME): $(OBJ)
	$(CC) $^ -o $@ $(CFLAGS) $(MEM_FLAGS) $(NCURSES_FLAGS)

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS) $(MEM_FLAGS)

clean:
	rm -f *.o $(GAME)