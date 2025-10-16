# Makefile for BrickGame project
BUILD_DIR = build
DEFAULT_INSTALL_PREFIX = $(HOME)/.local/share/BrickGame
CMAKE = cmake
CMAKE_FLAGS = -DBUILD_TYPE=Release
TOOLSET = "Ninja" # switch to "GNU Makefile" if needed

# Default target
all: $(BUILD_DIR)
	$(CMAKE) --build $(BUILD_DIR)

# Configure and build
$(BUILD_DIR):
	$(CMAKE) -G $(TOOLSET) -B $(BUILD_DIR) $(CMAKE_FLAGS) .

# Install using CMake's --prefix (recommended way)
install: $(BUILD_DIR)
	$(CMAKE) -G $(TOOLSET) -B $(BUILD_DIR) $(CMAKE_FLAGS) .
	$(CMAKE) --build $(BUILD_DIR)
	$(CMAKE) --install $(BUILD_DIR) --prefix $(INSTALL_PREFIX)
	@sh ./scripts/install.sh $(INSTALL_PREFIX)
	@echo "Installed to $(INSTALL_PREFIX)"

# Uninstall (remove installed files)
uninstall:
	@rm -rf $(INSTALL_PREFIX)
	@sh scripts/uninstall.sh
	@echo "Uninstalled from $(INSTALL_PREFIX)"

# Clean build directory
clean:
	rm -rf $(BUILD_DIR) docs *gz

# Generate documentation (DVI)
dvi:
	@echo "Generating LaTeX documentation..."
	@mkdir -p docs
	@pdflatex -output-directory=docs scripts/brickgame.tex >/dev/null 

# Create distribution tarball
dist: clean
	@echo "Creating distribution package..."
	@mkdir -p BrickGame-2.0
	@cp -r src include materials misc scripts LICENSE README.md BrickGame-2.0/
	@tar -czf BrickGame-2.0.tar.gz BrickGame-2.0/
	@rm -rf BrickGame-2.0
	@echo "Distribution package: BrickGame-2.0.tar.gz"

# Run tests
test: $(BUILD_DIR)
	$(CMAKE) -G $(TOOLSET) -B $(BUILD_DIR) -DBUILD_TESTS=ON .
	$(CMAKE) --build $(BUILD_DIR) --target tests_all
	$(CMAKE) --build $(BUILD_DIR) --target test

# Run tests with coverage
coverage: $(BUILD_DIR)
	$(CMAKE) -G $(TOOLSET) -B $(BUILD_DIR) -DBUILD_TYPE=Coverage .
	$(CMAKE) --build $(BUILD_DIR) --target tests_all
	$(CMAKE) --build $(BUILD_DIR) --target coverage
	@echo "Coverage report: $(BUILD_DIR)/coverage/coverage_report/index.html"

# Build debug version
debug: $(BUILD_DIR)
	$(CMAKE) -G $(TOOLSET) -B $(BUILD_DIR) -DBUILD_TYPE=Debug -DENABLE_SANITIZER=ON .
	$(CMAKE) --build $(BUILD_DIR)

# Build release version  
release: $(BUILD_DIR)
	$(CMAKE) -G $(TOOLSET) -B $(BUILD_DIR) -DBUILD_TYPE=Release .
	$(CMAKE) --build $(BUILD_DIR)

# Configure with custom install prefix
configure: $(BUILD_DIR)
	@echo "Configured with INSTALL_PREFIX=$(INSTALL_PREFIX)"

format:
	@echo "[clang-format] applying..."
	@find . -name "*.hpp" -o -name "*.h" -o -name "*.c" -o -name "*.cc"  -o -name "*.cpp" -o -name "*.tpp" | xargs clang-format -i -style=Google

format-check:
	@echo "[clang-format] checking..."
	@find .  -name "*.hpp" -o -name "*.h" -o -name "*.c" -o -name "*.cc"  -o -name "*.cpp" -o -name "*.tpp"  | xargs clang-format -n -style=Google

cppcheck:
	@echo "[cppcheck] analyzing..."
	@cppcheck --enable=warning,style,performance,portability \
	         --suppress=missingIncludeSystem --std=c++20 \
			 --std=c11 --check-level=exhaustive \
	         ./src ./include

# Show help
help:
	@echo "Available targets:"
	@echo "  all           - Build project (default)"
	@echo "  install       - Install using: make install INSTALL_PREFIX=/path"
	@echo "                  Default: INSTALL_PREFIX=$(DEFAULT_INSTALL_PREFIX)"
	@echo "  stage-install - Install with DESTDIR for packaging"
	@echo "  system-install - Install to /usr/local"
	@echo "  uninstall     - Remove installed files"
	@echo "  clean         - Remove build directory"
	@echo "  dvi           - Generate documentation"
	@echo "  dist          - Create distribution tarball"
	@echo "  test          - Build and run tests"
	@echo "  debug         - Build debug version"
	@echo "  release       - Build release version"
	@echo "  help          - Show this help"
	@echo ""
	@echo "Usage examples:"
	@echo "  make install INSTALL_PREFIX=/usr/local"
	@echo "  make install INSTALL_PREFIX=\$$HOME/.local"
	@echo "  make stage-install DESTDIR=/tmp/stage INSTALL_PREFIX=/usr"

# Set default INSTALL_PREFIX if not provided
ifndef INSTALL_PREFIX
    INSTALL_PREFIX = $(DEFAULT_INSTALL_PREFIX)
endif

.PHONY: all install uninstall clean dvi dist test debug release help system-install stage-install configure