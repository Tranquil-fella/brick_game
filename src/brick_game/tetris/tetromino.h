/**
 * @file tetromino.h
 * @brief Tetromino game piece management and movement system
 * @details
 * - Handles all tetromino shapes, rotations, and movement logic
 * - Provides collision detection and field interaction
 * - Manages the "next piece" preview system
 */

#ifndef TETROMINO_H
#define TETROMINO_H

#include "backend.h"

/**
 * @struct MoveCommand_t
 * @brief Represents a player movement command
 */
typedef struct {
  int move;   ///< Movement direction/action
  bool hold;  ///< Whether the hold action is active
} MoveCommand_t;

/**
 * @struct Coordinates_t
 * @brief 2D position in the game field
 */
typedef struct {
  int x;  ///< Horizontal position (columns)
  int y;  ///< Vertical position (rows)
} Coordinates_t;

/**
 * @struct Tetromino_t
 * @brief Active tetromino piece information
 */
typedef struct {
  Coordinates_t centerCoords;  ///< Rotation center coordinates
  int shape;                   ///< Shape type (I_shape, O_shape, etc.)
  int rotation;                ///< Current rotation (0°, 90°, 180°, 270°)
} Tetromino_t;

/**
 * @enum FieldCellState_t
 * @brief Possible states of a game field cell
 */
typedef enum {
  Empty,          ///< Empty cell
  Settled,        ///< Settled (locked) tetromino piece
  Volatile,       ///< Currently moving tetromino piece
  I_shape,        ///< I-shaped tetromino
  O_shape,        ///< O-shaped tetromino
  T_shape,        ///< T-shaped tetromino
  S_shape,        ///< S-shaped tetromino
  Z_shape,        ///< Z-shaped tetromino
  J_shape,        ///< J-shaped tetromino
  L_shape,        ///< L-shaped tetromino
  CellStateCount  ///< Total count of cell states
} FieldCellState_t;

/**
 * @typedef Movement_t
 * @brief Function pointer for tetromino movement operations
 */
typedef int (*Movement_t)(GameInfo_t*, Tetromino_t*);

/**
 * @brief Gets movement function for a command
 * @param cmd Movement command to process
 * @return Corresponding movement function pointer
 */
Movement_t getMovement(const MoveCommand_t cmd);

/**
 * @brief Gets the next tetromino from preview
 * @param next Pointer to next shape buffer
 * @return New tetromino with default position
 */
Tetromino_t getNextTetromino(int** next);

/**
 * @brief Generates a new random shape in next buffer
 * @param next Pointer to next shape buffer
 */
void setRandomShape(int** next);

/**
 * @brief Removes tetromino from field (sets to Empty)
 * @param current Tetromino to remove
 * @param field Game field to modify
 */
void removeTetromino(const Tetromino_t* current, int** field);

/**
 * @brief Locks tetromino in place (sets to Settled)
 * @param current Tetromino to settle
 * @param field Game field to modify
 */
void settleTetromino(const Tetromino_t* current, int** field);

/**
 * @brief Places tetromino on field (sets to Volatile)
 * @param new Tetromino to place
 * @param field Game field to modify
 */
void putTetromino(const Tetromino_t* new, int** field);

#endif