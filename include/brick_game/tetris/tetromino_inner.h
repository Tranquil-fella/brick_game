#ifndef TETR_INNER_H
#define TETR_INNER_H

#include "movement_queue.h"
#include "tetromino_mover.h"

#define FIELD_STATES 3
#define TOTAL_PIECES 4
#define TOTAL_TETROMINOS 7
#define SHAPE_NUMBER(X) ((X) - FIELD_STATES)
#define START_X (FIELD_WIDTH / 2)
#define START_Y -1
#define NEXT_SHAPE_NUM(next) *(next[0] + NEXTF_LENGTH * NEXTF_WIDTH)

#define RANDOM_SHAPE (rand() % (CellStateCount - FIELD_STATES)) + FIELD_STATES
#define I_SHAPE {{0, -1}, {0, 0}, {0, 1}, {0, 2}}
#define O_SHAPE {{0, 0}, {0, 1}, {1, 0}, {1, 1}}
#define T_SHAPE {{-1, 0}, {0, 0}, {1, 0}, {0, 1}}
#define S_SHAPE {{-1, 1}, {0, 1}, {0, 0}, {1, 0}}
#define Z_SHAPE {{-1, 0}, {0, 0}, {0, 1}, {1, 1}}
#define J_SHAPE {{-1, -1}, {-1, 0}, {0, 0}, {1, 0}}
#define L_SHAPE {{-1, 1}, {-1, 0}, {0, 0}, {1, 0}}

typedef enum { Angle0, Angle90, Angle180, Angle270 } Angle_t;

bool canMove(const Tetromino_t*, int** field);
bool canRotate(Tetromino_t*, int** field);
Coordinates_t applyRotation(const Tetromino_t* tetromino,
                            Coordinates_t piece_coords);
Coordinates_t getTetrPieceCoords(const Tetromino_t* tetromino,
                                 const int pieceNumber);
void setFieldCellState(int** field, const FieldCellState_t state,
                       const Tetromino_t* tetromino);
int moveDown(GameInfo_t* info, Tetromino_t* current);
int moveLeft(GameInfo_t* info, Tetromino_t* current);
int moveRight(GameInfo_t* info, Tetromino_t* current);
int smashDown(GameInfo_t* info, Tetromino_t* current);
int rotate(GameInfo_t* info, Tetromino_t* current);

#endif