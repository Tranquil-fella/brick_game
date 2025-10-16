#include "frontend.h"

#include "../../brick_game/colors.h"
#include "game_field.h"
#include "input.h"
#include "menus.h"

#define BACKGROUND_COLOR COLOR_BLACK
#define DEFAULT_GAMEDATA                  \
  (GameData_t){.controls.prog_on = true,  \
               .current_scr = MainMenu,   \
               .controls.game_on = false, \
               .mem = 0}
#define TIMEOUT 160
#define MTX_ON 1
#define CND_ON 2
#define WIN_ON 4
#define MENU_ON 8
#define INITIALIZED 0b00001111
#define FAILED 0b10000000

int initProgramm(GameData_t* data);
void cleanup(GameData_t* data);
void initNcurses();
void initColors();
int initWindows(GameWindows_t* windows);
void destroyWindows(GameWindows_t* windows);

int main() {
  GameData_t data;
  if (initProgramm(&data) != OK)
    error(&data.controls);
  else {
    postMenu(&data, MainMenu);
    processInput(&data);
  }
  cleanup(&data);
  return data.mem == FAILED ? EXIT_FAILURE : EXIT_SUCCESS;
}

int initProgramm(GameData_t* data) {
  int result = EXIT_FAILURE;
  *data = DEFAULT_GAMEDATA;
  initNcurses();
  if (!mtx_init(&data->controls.mutex, mtx_plain)) data->mem |= MTX_ON;
  if (!cnd_init(&data->controls.cnd)) data->mem |= CND_ON;
  if (!initWindows(&data->windows)) data->mem |= WIN_ON;
  if (!initGameMenus(&data->game_menus)) data->mem |= MENU_ON;
  if (data->mem == INITIALIZED)
    result = OK;
  else
    data->mem = FAILED;
  return result;
}

int initDisplay(GameData_t* data) {
  int exit_code = EXIT_FAILURE;
  if (thrd_create(&data->controls.game_thrd, printGameScreen, data) ==
      thrd_success)
    exit_code = OK;
  return exit_code;
}

int initWindows(GameWindows_t* windows) {
  int result = OK;
  windows->field_win =
      newwin(FIELD_WIN_HEIGHT, FIELD_WIN_WIDTH, 0, SCREEN_LEFT_OFFSET);
  windows->stats_win = newwin(FIELD_WIN_HEIGHT, STATS_WIN_WIDTH, 0,
                              FIELD_WIN_WIDTH + SCREEN_LEFT_OFFSET);
  windows->context_win =
      newwin(CONTEXT_WIN_HEIGHT, FIELD_WIN_WIDTH + STATS_WIN_WIDTH,
             FIELD_WIN_HEIGHT, SCREEN_LEFT_OFFSET);
  if (!windows->field_win || !windows->stats_win || !windows->context_win)
    result = EXIT_FAILURE;
  return result;
}

void initNcurses() {
  initscr();
  noecho();
  initColors();
  curs_set(0);
  keypad(stdscr, TRUE);
  timeout(TIMEOUT);
}

void initColors() {
  start_color();
  init_pair(Static, BACKGROUND_COLOR, COLOR_WHITE);
  init_pair(Damaged, BACKGROUND_COLOR, COLOR_WHITE);
  init_pair(Red, BACKGROUND_COLOR, COLOR_RED);
  init_pair(Magenta, BACKGROUND_COLOR, COLOR_MAGENTA);
  init_pair(Green, BACKGROUND_COLOR, COLOR_GREEN);
  init_pair(Cyan, BACKGROUND_COLOR, COLOR_BLUE);
  init_pair(Yellow, BACKGROUND_COLOR, COLOR_YELLOW);
  init_pair(Orange, BACKGROUND_COLOR, COLOR_YELLOW);
  init_pair(Blue, BACKGROUND_COLOR, COLOR_BLUE);
}

void error(Controls_t* ctrls) {
  clear();
  printw("An error ocurred, the programm will close...");
  refresh();
  timeout(-1);
  getch();
  ctrls->prog_on = false;
}

void cleanup(GameData_t* data) {
  if (data->mem & MTX_ON) mtx_destroy(&data->controls.mutex);
  if (data->mem & CND_ON) cnd_destroy(&data->controls.cnd);
  if (data->mem & MENU_ON) destroyGameMenus(&data->game_menus);
  if (data->mem & WIN_ON) destroyWindows(&data->windows);
  endwin();
}

void destroyWindows(GameWindows_t* windows) {
  if (windows->field_win) delwin(windows->field_win);
  if (windows->stats_win) delwin(windows->stats_win);
  if (windows->context_win) delwin(windows->context_win);
}