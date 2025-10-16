# Root Makefile for the Brick Game project
DATA = ../build
MISC = ../misk
LIBS_DIR = $(DATA)/libs
BIN = $(DATA)/bin
SAVES = $(DATA)/saves
INSTALL_PATH = /home/$$USER/Games
ICON = $(MISC)/brick_game.png
INSTALL = ../scripts/install.sh
DOXY = $(MISC)/Doxyfile
DOC_DIR = $(DATA)/docs
GCOV_DIR = $(DATA)/gcov_misc
REPORT_DIR = $(DATA)/coverage_report
INFO = coverage.info
V_SUPPRESS = $(MISC)/valgrind.supp


.PHONY: all snake tetris cli desktop test test_snake test_tetris clean install uninstall dvi dist gcov_report valgrind cppcheck clang folders

# Default target - build everything available
all: folders snake tetris cli desktop
	echo $(SAVE_PATH)


# Create necessary directories
folders:
	@mkdir -p -m 755 $(DATA) $(LIBS_DIR) $(BIN) $(SAVES)
	@echo "Created directories: $(DATA), $(LIBS_DIR), $(BIN), $(SAVES)"

# Build specific modules
snake:
	$(MAKE) -C brick_game/snake LIBS_DIR=../../$(LIBS_DIR) 

tetris:
	$(MAKE) -C brick_game/tetris LIBS_DIR=../../$(LIBS_DIR)
cli:
	$(MAKE) -C gui/cli BIN=../../$(BIN) LIBS_DIR=../../$(LIBS_DIR)

# Desktop GUI build using CMake
desktop:
	@echo "Building desktop GUI with CMake..."
	@mkdir -p gui/desktop/build
	@cd gui/desktop/build && \
	 cmake .. && \
	 $(MAKE) -j$(nproc)
	@cp gui/desktop/build/desktop $(BIN)/BrickGameDesktop 2>/dev/null || \
	 echo "Desktop executable built but not copied to bin (may need manual installation)"

# Test targets
test: test_snake test_tetris

test_snake:
	$(MAKE) -C brick_game/snake test

test_tetris:
	$(MAKE) -C brick_game/tetris test

# Global targets
install: all
	@chmod +x $(INSTALL)
	@./$(INSTALL) $(DATA) $(ICON) $(INSTALL_PATH)
	@echo "Games installed at $(INSTALL_PATH)"

uninstall:
	@rm -f $$HOME/Desktop/BrickGame*.desktop $$HOME/.local/share/konsole/BrickGame*.profile
	@rm -rf $(INSTALL_PATH)BrickGame*
	@echo "Games uninstalled"

dvi:
	@mkdir -p $(DOC_DIR)
	-@doxygen $(DOXY) >/dev/null 2>&1
	@open $(DOC_DIR)html/index.html 2>/dev/null || echo "Documentation generated at $(DOC_DIR)"

dist:
	@mkdir -p BrickGame
	@cp -r brick_game gui Makefile $(INSTALL) $(ICON) $(DOXY) BrickGame/
	@tar -czvf BrickGame.tar.gz ./BrickGame/
	@rm -rf ./BrickGame/

clean:
	$(MAKE) -C brick_game/snake clean
	$(MAKE) -C brick_game/tetris clean
	$(MAKE) -C gui/cli clean
	@# Clean CMake build
	rm -rf gui/desktop/build
	@# Clean root directories
	rm -rf $(DATA) $(DOC_DIR) $(REPORT_DIR) $(GCOV_DIR) BrickGame.tar.gz

# Desktop-specific clean
clean_desktop:
	rm -rf gui/desktop/build

gcov_report:
	$(MAKE) -C brick_game/snake gcov_report
	$(MAKE) -C brick_game/tetris gcov_report

valgrind_snake:
	$(MAKE) -C brick_game/snake valgrind

valgrind_tetris:
	$(MAKE) -C brick_game/tetris valgrind

cppcheck:
	cppcheck --check-level=exhaustive --enable=all --suppress=missingIncludeSystem brick_game/ gui/

clang:
	find . -name '*.c' -o -name '*.h' -o -name '*.cc' -o -name '*.hpp' | xargs clang-format -n --style=Google

# Desktop-specific targets
desktop_rebuild: clean_desktop desktop

desktop_debug:
	@mkdir -p gui/desktop/build
	@cd gui/desktop/build && \
	 cmake -DCMAKE_BUILD_TYPE=Debug .. && \
	 $(MAKE) -j$(nproc)

desktop_release:
	@mkdir -p gui/desktop/build
	@cd gui/desktop/build && \
	 cmake -DCMAKE_BUILD_TYPE=Release .. && \
	 $(MAKE) -j$(nproc)