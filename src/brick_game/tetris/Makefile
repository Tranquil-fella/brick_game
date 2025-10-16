# Tetris module Makefile - builds only the library
CC = gcc
CFLAGS = -std=c11 -Wall -Werror -Wextra -fPIC 
MEM_FLAGS = -g
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
CHECK_FLAGS = -lcheck -lsubunit -lm

# Paths (set by root Makefile)
LIBS_DIR ?= ./
GAME_LIB = $(LIBS_DIR)libtetris.so

# Source files
SRC = $(wildcard *.c)
OBJ = $(SRC:.c=.o)
TESTS_SRC = $(wildcard tests/*.c)
TEST_OBJ = $(TESTS_SRC:.c=.o)
TEST = test_tetris.out

.PHONY: all clean test gcov_report valgrind

all: $(GAME_LIB)

$(GAME_LIB): $(OBJ)
	$(CC) -shared $^ -o $@ $(CFLAGS) $(MEM_FLAGS)

%.o: %.c
	$(CC) -c -fPIC $< -o $@ $(CFLAGS) $(MEM_FLAGS)

test: CFLAGS += $(COVERAGE_FLAGS)
test: $(TEST)
	./$(TEST)

$(TEST): $(TEST_OBJ)
	$(CC) $^ -o $@ $(CFLAGS) $(CHECK_FLAGS) $(MEM_FLAGS)

tests/%.o: tests/%.c
	$(CC) -c $< -o $@ $(CFLAGS) $(MEM_FLAGS)

gcov_report: test
	@mkdir -p ../../$(GCOV_DIR)/tetris
	@mv *.gc* tests/*.gc* ../../$(GCOV_DIR)/tetris/ 2>/dev/null || true
	@echo "Tetris coverage data moved to $(GCOV_DIR)/tetris/"

valgrind: $(TEST)
	valgrind --track-origins=yes --leak-check=full ./$(TEST)

clean:
	rm -f *.o tests/*.o $(GAME_LIB) $(TEST) *.gc* tests/*.gc*