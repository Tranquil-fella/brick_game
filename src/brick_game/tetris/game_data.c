#include "game_data.h"

#include <string.h>

#include "highscore_keeper.h"
#include "tetromino.h"

typedef struct {
  GameInfo_t info;
  GameState_t state;
  mtx_t mutex;
  cnd_t pause_cond;
  Threads_t threads;
} GameRuntimeData_t;

int** initField();
int** initNextShape();

GameRuntimeData_t* getGameData() {
  static GameRuntimeData_t info;
  return &info;
}

GameInfo_t* getGameInfo() { return &getGameData()->info; }

Threads_t* getThreads() { return &getGameData()->threads; }

mtx_t* getMutex() { return &getGameData()->mutex; }

cnd_t* getPauseCondition() { return &getGameData()->pause_cond; }

GameState_t getGameState() { return getGameData()->state; }

bool isGameState(const GameState_t state) {
  return getGameData()->state == state;
}

void setGameState(GameState_t state) { getGameData()->state = state; }

int initGameData() {
  int exit_code = EXIT_SUCCESS;
  GameRuntimeData_t* data = getGameData();
  data->info.field = initField();
  data->info.high_score = getHighscore();
  data->info.next = initNextShape();
  data->info.speed = 1;
  data->info.level = 1;
  if (mtx_init(&data->mutex, mtx_plain) != thrd_success)
    exit_code = EXIT_FAILURE;
  else if (cnd_init(&data->pause_cond) != thrd_success) {
    mtx_destroy(&data->mutex);
    exit_code = EXIT_FAILURE;
  }
  return exit_code;
}

int** initField() {
  static int field[FIELD_LENGTH * FIELD_WIDTH] = {0};
  static int* field_ptrs[FIELD_LENGTH];
  for (int i = 0; i < FIELD_LENGTH; ++i) {
    field_ptrs[i] = field + (i * FIELD_WIDTH);
  }
  return field_ptrs;
}

int** initNextShape() {
  static int next_arr[NEXTF_LENGTH * NEXTF_WIDTH + 1];
  static int* next[NEXTF_LENGTH];
  for (int i = 0; i < NEXTF_LENGTH; ++i) {
    next[i] = next_arr + (i * NEXTF_WIDTH);
  }
  setRandomShape(next);
  return next;
}

void cleanUpData() {
  GameRuntimeData_t* data = getGameData();
  cnd_destroy(&data->pause_cond);
  mtx_destroy(&data->mutex);
  memset(data->info.field[0], 0, FIELD_LENGTH * FIELD_WIDTH * sizeof(int));
  memset(data->info.next[0], 0, (NEXTF_LENGTH * NEXTF_WIDTH + 1) * sizeof(int));
  memset(data, 0, sizeof(GameRuntimeData_t));
}