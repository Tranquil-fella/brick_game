# Makefile for snake library and automated test building
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -Werror -I. -I.. -fPIC -fconcepts-diagnostics-depth=2 -DSAVE_PATH=\""$(SAVE_PATH)\""
GTEST_FLAGS = -lgtest -lgtest_main -lpthread
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage

# Paths (set by root Makefile)
LIBS_DIR ?= ./
BIN_DIR ?= ./bin
GAME_LIB = $(LIBS_DIR)libsnake.so

SAVE_PATH ?= ./saves

# Source files
SRCS = $(wildcard *.cc)
OBJS = $(SRCS:.cc=.o)

# Auto-discover test files and create test binaries with _test suffix
TEST_SRCS = $(wildcard tests/*_test.cc)
TEST_BASE_NAMES = $(patsubst tests/%_test.cc,%,$(TEST_SRCS))
TEST_BINS = $(addprefix $(BIN_DIR)/,$(addsuffix _test,$(TEST_BASE_NAMES)))

# Default target - build library
all: $(GAME_LIB)

# Build shared library
$(GAME_LIB): $(OBJS)
	$(CXX) -shared -o $@ $^ $(CXXFLAGS)

# Compile source files with coverage flags for test builds
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pattern rule for building test binaries with _test suffix (with coverage)
$(BIN_DIR)/%_test: tests/%_test.cc $(GAME_LIB) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(COVERAGE_FLAGS) -o $@ $< -L$(LIBS_DIR) -lsnake $(GTEST_FLAGS)

# Create bin directory if it doesn't exist
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Individual test run targets
run_%: $(BIN_DIR)/%_test
	LD_LIBRARY_PATH=$(LIBS_DIR) ./$<

# Run all tests with coverage
test: CXXFLAGS += $(COVERAGE_FLAGS)
test: $(TEST_BINS)
	@for test_bin in $(TEST_BINS); do \
		echo "Running $$test_bin..."; \
		LD_LIBRARY_PATH=$(LIBS_DIR) ./$$test_bin; \
		echo ""; \
	done

# Run tests with valgrind for memory checking
test_valgrind: $(TEST_BINS)
	@for test_bin in $(TEST_BINS); do \
		echo "Running $$test_bin with valgrind..."; \
		LD_LIBRARY_PATH=$(LIBS_DIR) valgrind --leak-check=full ./$$test_bin; \
		echo ""; \
	done

# Generate coverage report
gcov_report: test
	@mkdir -p ../../$(GCOV_DIR)/snake
	@mv *.gc* tests/*.gc* ../../$(GCOV_DIR)/snake/ 2>/dev/null || true
	@echo "Snake coverage data moved to ../../$(GCOV_DIR)/snake/"
	@echo "Use 'make gcov_report' from root to generate HTML report"

# Clean up
clean:
	rm -f *.o $(GAME_LIB) $(TEST_BINS) *.gc* tests/*.gc*
	rm -rf $(BIN_DIR)

# Debug build
debug: CXXFLAGS += -g -O0
debug: clean all

# Release build  
release: CXXFLAGS += -O3 -DNDEBUG
release: clean all

.PHONY: all test test_valgrind clean debug release run_% gcov_report