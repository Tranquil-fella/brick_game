#include <string.h>

#include "interface_loader.h"
#include "menus.h"

typedef void (*MenuAction_t)(const UserAction_t, GameData_t *);

typedef void (*OptionsHandler_t)(GameData_t *);

void handleEscape(GameData_t *data);
void switchScreen(GameData_t *data, const GameScreen_t new);
void handleMenuOpts(GameData_t *data);
void handleMainMenuOpts(GameData_t *data);
void handleExitMenuOpts(GameData_t *data);
void handleGameSelectOpts(GameData_t *data);
static void startGame(GameData_t *data);
static void endGame(GameData_t *data);
const char *getCurrentItemName(MENU *menu);

void navigateMenu(const UserAction_t action, GameData_t *data) {
  switch (action) {
    case Down:
      menu_driver(data->game_menus.menus[data->current_scr].menu,
                  REQ_DOWN_ITEM);
      refresh();
      break;
    case Up:
      menu_driver(data->game_menus.menus[data->current_scr].menu, REQ_UP_ITEM);
      refresh();
      break;
    case Terminate:
      handleEscape(data);
      break;
    case Pause:
    case Start:
    case Action:
      handleMenuOpts(data);
      break;
    default:
  }
}
void handleMenuOpts(GameData_t *data) {
  static OptionsHandler_t handlers[TotalMenus] = {
      handleMainMenuOpts, handleExitMenuOpts, handleGameSelectOpts};
  handlers[data->current_scr](data);
}

void handleEscape(GameData_t *data) {
  switch (data->current_scr) {
    case MainMenu:
      switchScreen(data, ExitMenu);
      break;
    case ExitMenu:
      switchScreen(data, data->controls.game_on ? GameScreen : MainMenu);
      break;
    case GameSelectMenu:
      switchScreen(data, MainMenu);
    default:
      break;
  }
}

void handleMainMenuOpts(GameData_t *data) {
  int choice = item_index(current_item(data->game_menus.menus[MainMenu].menu));
  if (choice == 0) {
    switchScreen(data, GameSelectMenu);
  } else if (choice == 1) {
    switchScreen(data, ExitMenu);
  }
}

void handleExitMenuOpts(GameData_t *data) {
  int choice = item_index(current_item(data->game_menus.menus[ExitMenu].menu));
  if (choice == 0) {
    if (data->controls.game_on) {
      endGame(data);
    } else {
      data->controls.prog_on = false;
    }
  } else if (choice == 1) {
    switchScreen(data, data->controls.game_on ? GameScreen : MainMenu);
  }
}

void handleGameSelectOpts(GameData_t *data) {
  const char *game_name =
      getCurrentItemName(data->game_menus.menus[GameSelectMenu].menu);
  if (game_name) {
    if (loadGameInterface(game_name, &data->interface) == EXIT_FAILURE)
      error(&data->controls);
    else {
      startGame(data);
    }
  } else
    switchScreen(data, MainMenu);
}

void navigateGameScreen(const UserAction_t action, const bool hold,
                        GameData_t *data) {
  switch (action) {
    case Terminate:
      switchScreen(data, ExitMenu);
      break;
    default:
      data->interface.userInput(action, hold);
      break;
  }
}

void switchScreen(GameData_t *data, const GameScreen_t new) {
  if (data->current_scr != GameScreen) {
    unpost_menu(data->game_menus.menus[data->current_scr].menu);
  } else {
    if (!data->interface.updateCurrentState().pause)
      data->interface.userInput(Pause, false);
  }

  data->current_scr = new;

  if (new != GameScreen) {
    postMenu(data, new);
  } else {
    if (!!data->interface.updateCurrentState().pause)
      data->interface.userInput(Pause, false);
    cnd_signal(&data->controls.cnd);
  }
}

const char *getCurrentItemName(MENU *menu) {
  ITEM *cur_item = current_item(menu);
  const char *name = item_name(cur_item);
  return (!name || !strcmp(name, "<empty>")) ? NULL : name;
}

void startGame(GameData_t *data) {
  data->controls.game_on = true;
  switchScreen(data, GameScreen);
  data->interface.userInput(Start, false);
  if (initDisplay(data) != OK) {
    error(&data->controls);
  }
}

void endGame(GameData_t *data) {
  data->controls.game_on = false;
  data->interface.userInput(Terminate, false);
  cnd_signal(&data->controls.cnd);
  thrd_join(data->controls.game_thrd, NULL);
  unloadGameInterface(&data->interface);
  switchScreen(data, MainMenu);
}
