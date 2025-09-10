/**
 * @file game_data.h
 * @brief Central game state management system
 * @details
 * - Manages all runtime game state including field, next shape, and game status
 * - Provides thread-safe access to shared game resources
 * - Handles game state transitions (Start/Pause/Run/End)
 */

#ifndef GAME_DATA_H
#define GAME_DATA_H

#include <threads.h>

#include "backend.h"

/**
 * @enum GameState_t
 * @brief Represents possible game states
 */
typedef enum {
  StartState,  ///< Initial state before game begins
  PauseState,  ///< Game is paused
  RunState,    ///< Game is actively running
  EndState     ///< Game has ended (win/lose)
} GameState_t;

/**
 * @struct Threads_t
 * @brief Container for game threads
 */
typedef struct {
  thrd_t main;       ///< Main game thread
  thrd_t scheduler;  ///< Scheduler/controller thread
} Threads_t;

/**
 * @brief Initializes all game data structures
 * @return EXIT_SUCCESS on success, EXIT_FAILURE if mutex/cond initialization
 * fails
 * @warning Must be called before any other game_data functions
 */
int initGameData();

/**
 * @brief Gets the main game information structure
 * @return Pointer to GameInfo_t containing field, score, speed, etc.
 * @note Thread-safe when used with proper mutex locking
 */
GameInfo_t* getGameInfo();

/**
 * @brief Gets the global game mutex
 * @return Pointer to mutex for thread synchronization
 */
mtx_t* getMutex();

/**
 * @brief Gets the pause condition variable
 * @return Pointer to condition variable used for pause/resume
 */
cnd_t* getPauseCondition();

/**
 * @brief Gets the game thread handles
 * @return Pointer to Threads_t structure containing game threads
 */
Threads_t* getThreads();

/**
 * @brief Gets current game state
 * @return Current GameState_t value
 */
GameState_t getGameState();

/**
 * @brief Checks if game is in specific state
 * @param state State to check against
 * @return true if game is in specified state
 */
bool isGameState(const GameState_t state);

/**
 * @brief Updates game state
 * @param state New state to set
 * @note Automatically signals pause_cond when changing states
 */
void setGameState(const GameState_t state);

/**
 * @brief Cleans up all game resources
 * @note Destroys mutex/cond and zeros all game data
 */
void cleanUpData();

#endif