#include "tetromino_mover.h"

#include "tetromino_mover_inner.h"

int initTetrominoMover() {
  int exit_code = EXIT_SUCCESS;
  Threads_t* threads = getThreads();
  if (thrd_create(&threads->main, mainGameLoop, getGameInfo()) ==
      thrd_success) {
    if (thrd_create(&threads->scheduler, autoShiftScheduler, getGameInfo()) !=
        thrd_success)
      exit_code = EXIT_FAILURE;
  } else
    exit_code = EXIT_FAILURE;
  return exit_code;
}

void waitTetrominoMoverEnd() {
  const Threads_t* threads = getThreads();
  thrd_join(threads->main, NULL);
  thrd_join(threads->scheduler, NULL);
}

static inline void adjustSpeed(GameInfo_t* info) {
  info->speed =
      info->speed >= MAX_SPEED ? MAX_SPEED : info->score / LEVEL_THRESHOLD + 1;
  info->level = info->speed;
}

static inline void handlePause() {
  while (isGameState(PauseState)) {
    cnd_wait(getPauseCondition(), getMutex());
  }
}

static inline bool isGameOver(const GameInfo_t* info) {
  bool gameover = false;
  for (int i = 0; i < FIELD_WIDTH && !gameover; ++i) {
    if (info->field[0][i] == Settled) gameover = true;
  }
  return gameover;
}
static inline int countReward(int combo) {
  int reward = 0;
  while (combo) reward += 100 * combo--;
  return reward;
}

int mainGameLoop(void* arg) {
  GameInfo_t* info = (GameInfo_t*)arg;
  Tetromino_t tetromino = getNextTetromino(info->next);

  while (isGameState(RunState) || isGameState(PauseState)) {
    if (mtx_lock(getMutex()) == thrd_success) {
      tickGameLogic(info, &tetromino);
      handlePause();
      mtx_unlock(getMutex());
    }
    SLEEP(MAIN_SLEEP_TIME);
  }
  return EXIT_SUCCESS;
}

void tickGameLogic(GameInfo_t* info, Tetromino_t* tetromino) {
  info->score += destroyMarkedRows(info, tetromino);
  adjustSpeed(info);
  if (!isEmptyQueue()) {
    moveTetromino(info, tetromino);
    markFilledRows(info->field);
    if (isGameOver(info)) endGame(info);
  }
}

void moveTetromino(GameInfo_t* info, Tetromino_t* tetromino) {
  Movement_t movement = getMovement(popQueue());
  if (movement) movement(info, tetromino);
}

int autoShiftScheduler(void* arg) {
  const GameInfo_t* info = (GameInfo_t*)arg;
  const MoveCommand_t down = MOVE_DOWN;
  while (isGameState(RunState) || isGameState(PauseState)) {
    SLEEP(GET_SLEEP_DURATION(info->speed));
    if (mtx_lock(getMutex()) == thrd_success) {
      pushQueue(down);
      handlePause();
      mtx_unlock(getMutex());
    }
  }
  return EXIT_SUCCESS;
}

void markFilledRows(int** field) {
  bool found_filled_rows = false;
  for (int i = 0; i < FIELD_LENGTH; ++i) {
    bool filled = true;
    for (int j = 0; j < FIELD_WIDTH && filled; ++j) {
      if (field[i][j] != Settled) filled = false;
    }
    if (filled) {
      markRow(field, i);
      found_filled_rows = true;
    }
  }
  if (found_filled_rows) SLEEP(ANIMATION_SLEEP_TIME * 8);
}

void markRow(int** field, int row) {
  static int marked_row[] = {
      Volatile, Volatile, Volatile, Volatile, Volatile,
      Volatile, Volatile, Volatile, Volatile, Volatile,
  };
  memcpy(field[row], marked_row, sizeof(marked_row));
}

int destroyMarkedRows(GameInfo_t* info, const Tetromino_t* current) {
  int temp_combo = 0, combo = 0;
  for (int i = 0; i <= FIELD_LENGTH; ++i) {
    if (i != FIELD_LENGTH && info->field[i][0] == Volatile) {
      ++temp_combo;
    } else if (temp_combo) {
      combo += temp_combo;
      removeTetromino(current, info->field);
      destroyRows(info->field, i, temp_combo);
      putTetromino(current, info->field);
      temp_combo = 0;
    }
  }
  return countReward(combo);
}

void destroyRows(int** field, const int row, const int combo) {
  memmove(field[0] + (FIELD_WIDTH)*combo, field[0],
          FIELD_WIDTH * (row - combo) * sizeof(int));
  memset(field[0], Empty, FIELD_WIDTH * sizeof(int) * combo);
}

void endGame(GameInfo_t* info) {
  setGameState(EndState);
  info->level = 0;
  info->speed = 0;
  if (info->score > info->high_score) setHighscore(info->score);
}