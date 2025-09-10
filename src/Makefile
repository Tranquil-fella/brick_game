#compilator and linker flags
CC = gcc
CFLAGS = -std=c11 -Wall -Werror -Wextra
NCURSES_FLAGS = -lncurses -lmenu 
MEM_FLAGS = -g #-fsanitize=address -fsanitize=leak -fsanitize=undefined -fsanitize=unreachable
COVERAGE_FLAGS = -fprofile-arcs -ftest-coverage
CHECK_FLAGS = -lcheck -lsubunit -lm
LINK_FLAGS = -ltetris  -L./$(LIBS_DIR) -Wl,-rpath,'$$ORIGIN'/$(LIBS_DIR)

#source files
BACKEND_SRC = $(wildcard $(BACKEND_DIR)*.c)
BACKEND_OBJ = $(BACKEND_SRC:.c=.o)
FRONTEND_SRC = $(wildcard $(FRONTEND_DIR)*.c)
TESTS_SRC = $(wildcard $(TESTS_DIR)*.c)

#paths and names of objects
BACKEND_DIR = brick_game/tetris/
TESTS_DIR = $(BACKEND_DIR)tests/
DATA= data/
LIBS_DIR = $(DATA)libs/
BIN = $(DATA)bin/
SAVES= $(DATA)saves
FRONTEND_DIR = gui/cli/
GAME_NAME = BrickGame_v1.0
GAME= $(BIN)$(GAME_NAME)
GAME_LIB = $(LIBS_DIR)libtetris.so
TEST = test.out
ICON = brick_game.png
INSTALL = install.sh
TESTS_DIR = $(BACKEND_DIR)tests/
INSTALL_PATH = /home/$$USER/Games/
INSTALL_DIR = $(INSTALL_PATH)$(GAME_NAME)/
SAVES_DIR = ./saves
DIST = $(GAME_NAME).tar.gz
DOXY = Doxyfile
DOC_DIR = docs/
GCOV_DIR = gcov_misc
REPORT_DIR = report
INFO = coverage.info
GCDA_GCNO = $(BACKEND_DIR)*.gc*  *.gc*


all: dvi install gcov_report 

play: $(GAME)
	@./$(GAME)

$(GAME): $(GAME_LIB)
	$(CC) $(FRONTEND_SRC) -o $@ $(CFLAGS) $(MEM_FLAGS) $(NCURSES_FLAGS)

$(GAME_LIB): $(BACKEND_OBJ) | folders
	$(CC) -shared $^ -o $@ $(CFLAGS) $(MEM_FLAGS)

folders: 
	@mkdir -p $(LIBS_DIR) $(BIN) $(SAVES)

%.o: %.c
	@$(CC) -c -fPIC $< -o $@ $(CFLAGS) $(MEM_FLAGS)

install: $(GAME)
	@chmod +x $(INSTALL)
	@./$(INSTALL) $(GAME_NAME) $(INSTALL_DIR) $(DATA) $(ICON)
	@echo game installed at $(INSTALL_DIR) and available at desktop

uninstall:
	@rm -f $$HOME/Desktop/$(GAME_NAME).desktop $$HOME/.local/share/konsole/$(GAME_NAME).profile
	@rm -rf $(INSTALL_DIR)
	@echo game deleted

clean:
	rm -f $(BACKEND_OBJ) $(GAME_LIB) $(GAME) $(TEST) $(DIST) $(TRASH) $(INFO) 
	rm -rf $(DATA) $(REPORT_DIR) $(GCOV_DIR) $(DOC_DIR)

dvi:
	@mkdir -p $(DOC_DIR)
	-@doxygen $(DOXY) >/dev/null 2>&1
	@open $(DOC_DIR)html/index.html

dist:
	@mkdir -p $(GAME_NAME)
	@cp -r $(BACKEND_DIR) $(FRONTEND_DIR) $(INSTALL) $(ICON) $(DOXY) Makefile $(GAME_NAME)/
	@tar -czvf $(DIST) ./$(GAME_NAME)/
	@rm -rf ./$(GAME_NAME)/

test: CFLAGS += $(COVERAGE_FLAGS)
test: $(TEST)
	-@./$(TEST)
	@mkdir -p $(GCOV_DIR)
	-@mv $(GCDA_GCNO) $(GCOV_DIR) 


$(TEST): $(GAME_LIB)
	$(CC) $(TESTS_SRC) -o $(TEST) $(CFLAGS) $(CHECK_FLAGS) $(LINK_FLAGS)


$(REPORT_DIR)/index.html: test
	-@lcov -q -t $(GAME_NAME) -o $(INFO) -c -d $(GCOV_DIR) >/dev/null 2>&1 
	-@lcov --remove $(INFO) '$(BACKEND_DIR)tests/*' -o $(INFO) >/dev/null 2>&1
	-@genhtml -q $(INFO) -o $(REPORT_DIR)  >/dev/null
	
gcov_report : $(REPORT_DIR)/index.html
	@open $(REPORT_DIR)/index.html >/dev/null 2>&1

valgrind: $(GAME)
	valgrind --track-origins=yes --leak-check=full --show-reachable=no --suppressions=./ncurses.supp ./$(GAME)

cppcheck:
	cppcheck --check-level=exhaustive --enable=all --suppress=missingIncludeSystem  .

clang:
	find . -name '*.c' -o -name '*.h' | xargs clang-format -n --style=Google

.PHONY: all install uninstall clean dvi dist test gcov_report valgrind clang