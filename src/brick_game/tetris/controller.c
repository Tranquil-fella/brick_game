
#include "controller.h"

#include "game_data.h"
#include "highscore_keeper.h"
#include "movement_queue.h"
#include "tetromino_mover.h"

#define MOVEMENT_POS 3
#define IS_MOVE_CMD(x) ((int)(x) >= MOVEMENT_POS)
#define MOVEMENT_NUM(x) (((int)x) - MOVEMENT_POS)
#define NO_MOVEMENT -1

typedef struct {
  MoveCommand_t movement;
  Command_t command;
} Actions_t;

void initGame(void);
void pauseGame(void);
void terminateGame(void);
void switch_pause_state(void);
MoveCommand_t getMoveCommand(const UserAction_t action, const bool hold);
void getAction(const int action, const int hold);
Actions_t* getControllerActions();
Command_t getCommand(const UserAction_t action);
void execAction();
void processMovement(MoveCommand_t move);

GameInfo_t updateCurrentState() {
  static GameInfo_t state = {0};
  if (mtx_trylock(getMutex()) == thrd_success) {
    state = *getGameInfo();
    mtx_unlock(getMutex());
  }
  return state;
}

void userInput(const UserAction_t action, const bool hold) {
  if (action >= Start && action <= Action) {
    Controller_t* controller = getController();
    controller->getAction(action, hold);
    controller->exec();
  }
}

Controller_t* getController() {
  static Controller_t controller = {getAction, execAction};
  return &controller;
}

Actions_t* getControllerActions() {
  static Actions_t actions = {.movement.move = NO_MOVEMENT, .command = NULL};
  return &actions;
}

void getAction(const int action, const int hold) {
  if (IS_MOVE_CMD(action)) {
    getControllerActions()->movement = getMoveCommand(action, hold);
  } else {
    getControllerActions()->command = getCommand(action);
  }
}

MoveCommand_t getMoveCommand(const UserAction_t action, const bool hold) {
  return (MoveCommand_t){.move = MOVEMENT_NUM((int)action), .hold = hold};
}

Command_t getCommand(const UserAction_t action) {
  static const Command_t commands[4][3] = {
      {initGame, NULL, NULL},                 //  StartState
      {pauseGame, pauseGame, terminateGame},  //  PauseState
      {pauseGame, pauseGame, terminateGame},  //  RunState
      {NULL, NULL, terminateGame}             //  EndState
  };
  return commands[(int)getGameState()][action];
}

void execAction() {
  Actions_t* actions = getControllerActions();
  if (actions->command) {
    actions->command();
    actions->command = NULL;
  } else if (actions->movement.move != NO_MOVEMENT) {
    processMovement(actions->movement);
    actions->movement.move = NO_MOVEMENT;
  }
}

void processMovement(MoveCommand_t move_cmd) {
  if (isGameState(RunState)) {
    if (mtx_lock(getMutex()) == thrd_success) {
      pushQueue(move_cmd);
      mtx_unlock(getMutex());
    }
  }
}

void initGame() {
  srand(time(NULL));
  initQueue();
  if (initGameData() == EXIT_SUCCESS) {
    setGameState(RunState);
    if (initTetrominoMover() == EXIT_FAILURE) {
      cleanUpData();
      setGameState(StartState);
    }
  }
}

void pauseGame() {
  if (mtx_lock(getMutex()) == thrd_success) {
    switch_pause_state();
    mtx_unlock(getMutex());
  }
}

void switch_pause_state() {
  if (isGameState(RunState)) {
    setGameState(PauseState);
    getGameInfo()->pause = true;
  } else if (isGameState(PauseState)) {
    if (cnd_broadcast(getPauseCondition()) == thrd_success) {
      setGameState(RunState);
      getGameInfo()->pause = false;
    }
  }
}

void terminateGame() {
  if (mtx_lock(getMutex()) == thrd_success) {
    if (isGameState(PauseState)) switch_pause_state();
    setGameState(EndState);
    mtx_unlock(getMutex());
    waitTetrominoMoverEnd();
    cleanUpData();
  }
}
