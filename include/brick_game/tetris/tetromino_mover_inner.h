#ifndef TETR_MVR_INNER_H
#define TETR_MVR_INNER_H

#include <string.h>

#include "game_data.h"
#include "highscore_keeper.h"
#include "movement_queue.h"

#define MOVE_DOWN {3, false}
#define BASE_REWARD 100
#define MAX_SPEED 10
#define LEVEL_THRESHOLD 600

void tickGameLogic(GameInfo_t* info, Tetromino_t* tetromino);
void moveTetromino(GameInfo_t* info, Tetromino_t* tetromino);
void markFilledRows(int** field);
void markRow(int** field, int row);
int destroyMarkedRows(GameInfo_t* info, const Tetromino_t* current);
void destroyRows(int** field, const int row, const int combo);
void endGame(GameInfo_t* info);
#endif