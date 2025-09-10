
#include "tetromino.h"

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

bool canMove(const Tetromino_t* new, int** field);
bool canRotate(Tetromino_t* new, int** field);
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

Movement_t getMovement(const MoveCommand_t cmd) {
  static const Movement_t movements[2][5] = {
      {moveLeft, moveRight, NULL, moveDown, rotate},
      {moveLeft, moveRight, NULL, smashDown, rotate}  //  hold
  };
  return movements[(int)cmd.hold][cmd.move];
}

int moveDown(GameInfo_t* info, Tetromino_t* current) {
  int result = EXIT_SUCCESS;
  Tetromino_t new = *current;
  ++new.centerCoords.y;
  removeTetromino(current, info->field);
  if (canMove(&new, info->field)) {
    putTetromino(&new, info->field);
    *current = new;
  } else {
    result = EXIT_FAILURE;
    putTetromino(current, info->field);
    settleTetromino(current, info->field);
    *current = getNextTetromino(info->next);
  }
  return result;
}

int smashDown(GameInfo_t* info, Tetromino_t* tetromino) {
  while (moveDown(info, tetromino) == EXIT_SUCCESS) {
    SLEEP(ANIMATION_SLEEP_TIME);
  }
  SLEEP(ANIMATION_SLEEP_TIME * 5);
  flushQueue();
  return EXIT_SUCCESS;
}

int moveLeft(GameInfo_t* info, Tetromino_t* current) {
  int result = EXIT_SUCCESS;
  Tetromino_t new = *current;
  --new.centerCoords.x;
  removeTetromino(current, info->field);
  if (canMove(&new, info->field))
    *current = new;
  else
    result = EXIT_FAILURE;
  putTetromino(current, info->field);
  return result;
}

int moveRight(GameInfo_t* info, Tetromino_t* current) {
  int result = EXIT_SUCCESS;
  Tetromino_t new = *current;
  ++new.centerCoords.x;
  removeTetromino(current, info->field);
  if (canMove(&new, info->field))
    *current = new;
  else
    result = EXIT_FAILURE;
  putTetromino(current, info->field);
  return result;
}

int rotate(GameInfo_t* info, Tetromino_t* current) {
  int result = EXIT_SUCCESS;
  Tetromino_t new = *current;
  if (current->shape != I_shape)
    new.rotation = (new.rotation + 1) % 4;
  else
    new.rotation = (new.rotation + 1) % 2;
  removeTetromino(current, info->field);

  if (canRotate(&new, info->field))
    *current = new;
  else
    result = EXIT_FAILURE;

  putTetromino(current, info->field);
  return result;
}

Tetromino_t getNextTetromino(int** next) {
  Tetromino_t new = {.shape = NEXT_SHAPE_NUM(next),
                     .rotation = Angle0,
                     .centerCoords = {.x = START_X, .y = START_Y}};
  setRandomShape(next);
  return new;
}

void setRandomShape(int** next) {
  Tetromino_t new = {.centerCoords = {1, 1},
                     .shape = NEXT_SHAPE_NUM(next),
                     .rotation = Angle0};
  if (new.shape) removeTetromino(&new, next);
  NEXT_SHAPE_NUM(next) = RANDOM_SHAPE;
  new.shape = NEXT_SHAPE_NUM(next);
  putTetromino(&new, next);
}

void removeTetromino(const Tetromino_t* current, int** field) {
  setFieldCellState(field, Empty, current);
}

void putTetromino(const Tetromino_t* new, int** field) {
  for (int i = 0; i < TOTAL_PIECES; i++) {
    setFieldCellState(field, new->shape, new);
  }
}

void settleTetromino(const Tetromino_t* current, int** field) {
  setFieldCellState(field, Settled, current);
}

void setFieldCellState(int** field, const FieldCellState_t state,
                       const Tetromino_t* tetromino) {
  for (int i = 0; i < TOTAL_PIECES; i++) {
    Coordinates_t coords = getTetrPieceCoords(tetromino, i);
    if (coords.y >= 0) field[coords.y][coords.x] = state;
  }
}

Coordinates_t getTetrPieceCoords(const Tetromino_t* tetromino,
                                 const int piece) {
  static int tetr_pieces_coords[TOTAL_TETROMINOS][TOTAL_PIECES][2] = {
      I_SHAPE, O_SHAPE, T_SHAPE, S_SHAPE, Z_SHAPE, J_SHAPE, L_SHAPE};
  Coordinates_t coords = {
      .y = tetr_pieces_coords[SHAPE_NUMBER(tetromino->shape)][piece][0],
      .x = tetr_pieces_coords[SHAPE_NUMBER(tetromino->shape)][piece][1]};
  coords = applyRotation(tetromino, coords);
  coords.x += tetromino->centerCoords.x;
  coords.y += tetromino->centerCoords.y;
  return coords;
}

Coordinates_t applyRotation(const Tetromino_t* tetromino,
                            Coordinates_t piece_coords) {
  if (tetromino->shape != O_shape) {
    if (tetromino->rotation == Angle90) {
      int temp = piece_coords.x;
      piece_coords.x = -piece_coords.y;
      piece_coords.y = temp;
    } else if (tetromino->rotation == Angle180) {
      piece_coords.x = -piece_coords.x;
      piece_coords.y = -piece_coords.y;
    } else if (tetromino->rotation == Angle270) {
      int temp = piece_coords.x;
      piece_coords.x = piece_coords.y;
      piece_coords.y = -temp;
    }
  }
  return piece_coords;
}

bool canMove(const Tetromino_t* new, int** field) {
  bool result = true;
  for (int i = 0; i < 4; i++) {
    Coordinates_t currPiece = getTetrPieceCoords(new, i);
    if (currPiece.x < 0 || currPiece.x >= FIELD_WIDTH ||
        currPiece.y >= FIELD_LENGTH) {
      result = false;
    } else if (currPiece.y >= 0 && field[currPiece.y][currPiece.x] != Empty) {
      result = false;
    }
  }
  return result;
}

bool canRotate(Tetromino_t* new, int** field) {
  int result = true;
  Coordinates_t saved = new->centerCoords;
  if (!canMove(new, field)) {
    ++new->centerCoords.x;
    if (new->shape == I_shape&& new->rotation == Angle90) ++new->centerCoords.x;
    if (!canMove(new, field)) {
      new->centerCoords = saved;
      --new->centerCoords.x;
      if (new->shape == I_shape&& new->rotation == Angle0)
        --new->centerCoords.x;
      if (!canMove(new, field)) result = false;
    }
  }
  return result;
}