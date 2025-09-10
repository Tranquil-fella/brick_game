
#include "menus.h"

#include <string.h>

#include "games_finder.h"

#define SET_MENU_OPTS(name, options) const char *const name[] = {options, NULL}
#define MAIN_MENU_OPTS "Select a Game", "   Quit   "
#define EXIT_MENU_OPTS "   YES   ", "   NO   "
#define MAIN_MENU_TITLE "BRICK GAME"
#define EXIT_MENU_TITLE "Would you like to exit?"
#define GAME_SELECT_MENU_TITLE "Choose your game:"
#define SET_TITLE(dest, title)     \
  static const char *name = title; \
  dest = name

#define SIZE_OF(a) (sizeof(a) / sizeof(a[0]))

int initMainMenu(GameMenus_t *menus);
int initExitMenu(GameMenus_t *menus);
int initGameSelectMenu(GameMenus_t *menus);
int initItems(Menu_t *menu, const char *const choices[]);
int initEmptyItem(Menu_t *menu);
int initMenu(Menu_t *menu);
void destroyItems(const Menu_t *menu);
void destroyMenu(const Menu_t *menu);

void print_centered(WINDOW *win, const char *str);
int initMenuWindows(GameMenus_t *menus);

int initGameMenus(GameMenus_t *menus) {
  int result = EXIT_FAILURE;
  if (initMenuWindows(menus) == OK) {
    if (initMainMenu(menus) || initExitMenu(menus) ||
        initGameSelectMenu(menus)) {
      destroyGameMenus(menus);
    } else
      result = OK;
  }
  return result;
}

int initMenuWindows(GameMenus_t *menus) {
  int result = OK;
  for (int i = 0; i < (int)TotalMenus && result == OK; ++i) {
    menus->menus[i].window =
        subwin(stdscr, SCREEN_HEIGHT, SCREEN_WIDTH, 0, SCREEN_LEFT_OFFSET);
    if (!menus->menus[i].window) {
      while (--i >= 0) {
        delwin(menus->menus[i].window);
      }
      result = EXIT_FAILURE;
    }
  }
  for (int i = 0; i < (int)TotalMenus && result == OK; ++i) {
    menus->menus[i].subwindow =
        derwin(menus->menus[i].window, SCREEN_HEIGHT / 2, SCREEN_WIDTH / 2,
               SCREEN_HEIGHT / 3, SCREEN_WIDTH / 3 + 1);
    if (!menus->menus[i].subwindow) {
      while (--i >= 0) {
        delwin(menus->menus[i].subwindow);
      }
      for (int j = 0; j < (int)TotalMenus; ++j) {
        delwin(menus->menus[j].window);
      }
      result = EXIT_FAILURE;
    }
  }
  return result;
}

int initMainMenu(GameMenus_t *menus) {
  int result = EXIT_FAILURE;
  SET_MENU_OPTS(choices, MAIN_MENU_OPTS);
  static ITEM *items[SIZE_OF(choices)];
  SET_TITLE(menus->menus[MainMenu].title, MAIN_MENU_TITLE);
  menus->menus[MainMenu].items_size = SIZE_OF(choices) - 1;
  menus->menus[MainMenu].items = items;

  if (initItems(&menus->menus[MainMenu], choices) == OK) {
    if (initMenu(&menus->menus[MainMenu]) == OK) {
      result = OK;
    } else {
      destroyItems(&menus->menus[MainMenu]);
    }
  }

  return result;
}

int initExitMenu(GameMenus_t *menus) {
  int result = EXIT_FAILURE;
  SET_MENU_OPTS(choices, EXIT_MENU_OPTS);
  static ITEM *items[SIZE_OF(choices)];
  SET_TITLE(menus->menus[ExitMenu].title, EXIT_MENU_TITLE);
  menus->menus[ExitMenu].items_size = SIZE_OF(choices) - 1;
  menus->menus[ExitMenu].items = items;
  if (initItems(&menus->menus[ExitMenu], choices) == OK) {
    if (initMenu(&menus->menus[ExitMenu]) == OK) {
      result = OK;
    } else {
      destroyItems(&menus->menus[ExitMenu]);
    }
  }
  return result;
}

int initGameSelectMenu(GameMenus_t *menus) {
  int result = EXIT_FAILURE;
  const Games_t *games = getAvailableGames();
  static ITEM *items[MAX_LIBS];
  menus->menus[GameSelectMenu].items = items;
  SET_TITLE(menus->menus[GameSelectMenu].title, GAME_SELECT_MENU_TITLE);
  if (games->size == 0) {
    if (initEmptyItem(&menus->menus[GameSelectMenu]) == OK) {
      if (initMenu(&menus->menus[GameSelectMenu]) == OK) {
        result = OK;
      } else {
        free_item(menus->menus[GameSelectMenu].items[0]);
      }
    }
  } else {
    menus->menus[GameSelectMenu].items_size = games->size;
    if (initItems(&menus->menus[GameSelectMenu], games->games) == OK) {
      if (initMenu(&menus->menus[GameSelectMenu]) == OK) {
        result = OK;
      } else {
        destroyItems(&menus->menus[GameSelectMenu]);
      }
    }
  }
  return result;
}

int initItems(Menu_t *menu, const char *const choices[]) {
  int result = OK;
  for (int i = 0; i < menu->items_size && !result; ++i) {
    menu->items[i] = new_item(choices[i], "");
    if (menu->items[i] == NULL) {
      while (--i >= 0) {
        free_item(menu->items[i]);
      }
      result = EXIT_FAILURE;
    }
    menu->items[menu->items_size] = NULL;
  }
  return result;
}

int initEmptyItem(Menu_t *menu) {
  int result = OK;
  ITEM *empty_item = new_item("<empty>", "");
  if (empty_item) {
    set_item_userptr(empty_item, NULL);
    item_opts_off(empty_item, O_SELECTABLE);
    menu->items[0] = empty_item;
    menu->items_size = 1;
  } else
    result = EXIT_FAILURE;
  return result;
}

int initMenu(Menu_t *menu) {
  int result = OK;
  if ((menu->menu = new_menu(menu->items))) {
    set_menu_format(menu->menu, menu->items_size, 1);
    set_menu_mark(menu->menu, "");
    if (set_menu_win(menu->menu, menu->window) != E_OK ||
        set_menu_sub(menu->menu, menu->subwindow) != E_OK) {
      result = EXIT_FAILURE;
    }
  } else
    result = EXIT_FAILURE;
  return result;
}

void destroyGameMenus(const GameMenus_t *menus) {
  for (int i = 0; i < (int)TotalMenus; ++i) {
    destroyMenu(&menus->menus[i]);
    delwin(menus->menus[i].subwindow);
    delwin(menus->menus[i].window);
  }
}

void destroyMenu(const Menu_t *menu) {
  if (menu->menu) {
    free_menu(menu->menu);
    destroyItems(menu);
  }
}

void destroyItems(const Menu_t *menu) {
  for (int i = 0; i < menu->items_size; ++i) {
    free_item(menu->items[i]);
  }
}

void postMenu(GameData_t *data, const GameScreen_t new) {
  Menu_t *menu = &data->game_menus.menus[new];
  mtx_lock(&data->controls.mutex);
  clear();
  box(menu->window, 0, 0);
  print_centered(menu->window, menu->title);
  post_menu(menu->menu);
  refresh();
  mtx_unlock(&data->controls.mutex);
}

void print_centered(WINDOW *win, const char *str) {
  int cols = getmaxx(win);
  int x = (cols - strlen(str)) / 2;
  mvwprintw(win, 4, x, "%s", str);
}
