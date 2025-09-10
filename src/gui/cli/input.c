#include "input.h"

#include "menus.h"
#define DEFAULT_INPUT {ERR, ERR, false}

typedef struct {
  UserAction_t current;
  UserAction_t previous;
  bool hold;
} UserInput_t;

UserInput_t* getInput(int user_input);
static UserAction_t getAction(int user_input);
static void execAction(const UserInput_t* input, GameData_t* data);

void processInput(GameData_t* data) {
  while (data->controls.prog_on) {
    untouchwin(stdscr);
    const UserInput_t* input = getInput(getch());
    execAction(input, data);
  }
}

UserInput_t* getInput(int user_input) {
  static UserInput_t input = DEFAULT_INPUT;
  input.current = getAction(user_input);
  input.hold = (input.current == input.previous);
  input.previous = input.current;
  return &input;
}

UserAction_t getAction(int user_input) {
  UserAction_t action = ERR;
  switch (user_input) {
    case KEY_UP:
    case 'w':
      action = Up;
      break;
    case KEY_DOWN:
    case 's':
      action = Down;
      break;
    case KEY_LEFT:
    case 'a':
      action = Left;
      break;
    case KEY_RIGHT:
    case 'd':
      action = Right;
      break;
    case 'p':
    case 'z':
      action = Pause;
      break;
    case ENTER_KEY:
      action = Start;
      break;
    case SPACEBAR:
      action = Action;
      break;
    case ESCAPE:
    case 'q':
      action = Terminate;
      break;
    default:
      break;
  }
  return action;
}

void execAction(const UserInput_t* input, GameData_t* data) {
  if ((int)input->current != ERR) {
    if (data->current_scr == GameScreen) {
      navigateGameScreen(input->current, input->hold, data);
    } else {
      navigateMenu(input->current, data);
    }
  }
}