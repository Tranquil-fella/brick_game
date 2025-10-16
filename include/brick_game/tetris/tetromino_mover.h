/**
 * @file tetromino_mover.h
 * @brief Interface for managing tetromino movement and game timing in a
 * multi-threaded Tetris system
 * @details This header file defines the core components responsible for
 * controlling tetromino movement within the Tetris game. It provides a
 * multi-threaded architecture where one thread handles the main game loop
 * (processing user inputs and game logic) and another schedules automatic
 * downward movement of tetrominos. The file includes timing constants for
 * controlling game pace, macros for sleep operations, and function declarations
 * for initializing and managing these threads. It relies on the C11 threads
 * library (`threads.h`) for concurrency, the `backend.h` file for game state
 * access (via GameInfo_t), and `tetromino.h` for tetromino-specific data and
 * operations. This module is critical for ensuring smooth, timed progression of
 * the game while maintaining responsiveness to user actions.
 */

#ifndef MOVER_H
#define MOVER_H

#include <threads.h>  // C11 threading library for creating and managing threads and sleep operations

#include "backend.h"  // Provides GameInfo_t and user input interface for game state management
#include "tetromino.h"  // Defines tetromino structures and operations used in movement logic

/**
 * @def MAIN_SLEEP_TIME
 * @brief Duration in nanoseconds to pause between iterations of the main game
 * loop
 * @details Set to 5,000,000 nanoseconds (5 milliseconds), this constant
 * controls the polling frequency of the main game loop. It determines how often
 * the game checks for user input, updates the tetromino position, and processes
 * game logic. A shorter duration increases responsiveness but may tax system
 * resources, while a longer duration conserves resources at the cost of
 * perceived lag. This value strikes a balance for smooth gameplay in a typical
 * Tetris implementation.
 */
#define MAIN_SLEEP_TIME (5000000)

/**
 * @def ANIMATION_SLEEP_TIME
 * @brief Duration in nanoseconds for the row destruction animation delay
 * @details Defined as 20,000,000 nanoseconds (20 milliseconds), this constant
 * sets the pause duration during the visual feedback of clearing filled rows.
 * It’s used to create a brief delay that enhances the player experience by
 * making line clears more noticeable before the field updates. The value is
 * multiplied in the implementation to extend the total animation time (e.g., 8
 * times this value for a full sequence), ensuring the effect is visible without
 * overly slowing gameplay.
 */
#define ANIMATION_SLEEP_TIME (20000000)

/**
 * @def AUTOSHIFT_SLEEP_TIME
 * @brief Base duration in nanoseconds for automatic tetromino downward movement
 * @details Set to 600,000,000 nanoseconds (600 milliseconds), this constant
 * establishes the initial time interval between automatic drops of the
 * tetromino as it moves down the field. It represents the slowest speed (level
 * 1) and is adjusted downward as the game speed increases (via
 * GET_SLEEP_DURATION). This duration ensures new players have ample time to
 * react while allowing for faster gameplay at higher levels.
 */
#define AUTOSHIFT_SLEEP_TIME (600000000)

/**
 * @def SLEEP_OFFSET
 * @brief Reduction in sleep time per speed level in nanoseconds
 * @details Defined as 50,000,000 nanoseconds (50 milliseconds), this constant
 * is subtracted from AUTOSHIFT_SLEEP_TIME for each increase in game speed. It
 * controls how much faster the tetromino drops as the player progresses, making
 * the game more challenging. For example, at speed level 2, the drop interval
 * would be 550ms, then 500ms at level 3, and so on, up to a practical minimum
 * determined by MAX_SPEED.
 */
#define SLEEP_OFFSET (50000000)

/**
 * @def GET_SLEEP_DURATION(i)
 * @brief Macro to calculate the auto-shift sleep duration based on current
 * speed level
 * @param i The current speed level (typically from GameInfo_t.speed)
 * @details Computes the sleep time for automatic tetromino drops by subtracting
 * SLEEP_OFFSET multiplied by the speed level from AUTOSHIFT_SLEEP_TIME. For
 * instance, at speed 1, it’s 600ms - 50ms = 550ms; at speed 5, it’s 600ms -
 * 250ms = 350ms. This formula ensures a smooth, predictable increase in
 * difficulty as the game progresses, with the result used by the
 * autoShiftScheduler thread to time its operations.
 */
#define GET_SLEEP_DURATION(i) (AUTOSHIFT_SLEEP_TIME - SLEEP_OFFSET * (i))

/**
 * @def SLEEP(x)
 * @brief Convenience macro for pausing execution for a specified number of
 * nanoseconds
 * @param x Duration in nanoseconds to sleep
 * @details Uses the C11 thrd_sleep() function with a timespec structure to
 * pause the calling thread for the given duration. The macro simplifies timing
 * control throughout the mover system, abstracting the struct initialization
 * (only nanoseconds are set, seconds remain 0). It’s used with MAIN_SLEEP_TIME,
 * ANIMATION_SLEEP_TIME, and GET_SLEEP_DURATION to regulate game pacing and
 * animations. Note that actual sleep time may vary slightly due to system
 * scheduling.
 */
#define SLEEP(x) thrd_sleep(&(struct timespec){.tv_nsec = (x)}, NULL)

/**
 * @brief Initializes the tetromino movement system by launching required
 * threads
 * @return EXIT_SUCCESS if both main and scheduler threads are created
 * successfully, EXIT_FAILURE if either thread creation fails
 * @details Sets up the multi-threaded movement system by creating two threads:
 * one for the main game loop (mainGameLoop) and one for the auto-shift
 * scheduler (autoShiftScheduler). Both threads operate on the shared GameInfo_t
 * structure from backend.h to manage game state. This function is typically
 * called after game data initialization to start the core gameplay mechanics.
 * If either thread fails to start, it returns an error to allow the caller to
 * handle the failure (e.g., by cleaning up resources).
 */
int initTetrominoMover();

/**
 * @brief Executes the primary game loop in a dedicated thread
 * @param arg Pointer to the GameInfo_t structure containing the current game
 * state
 * @return EXIT_SUCCESS upon normal completion (e.g., when the game ends)
 * @details Runs as the main thread’s entry point, continuously processing game
 * logic such as tetromino movement, row clearing, and state updates. It loops
 * while the game is in RunState or PauseState, sleeping between iterations (via
 * MAIN_SLEEP_TIME) to control frame rate. The arg parameter provides access to
 * shared game data, which it modifies in a thread-safe manner using mutexes.
 * This function drives the core gameplay experience, responding to user inputs
 * and updating the field.
 */
int mainGameLoop(void* arg);

/**
 * @brief Manages automatic downward movement of tetrominos in a separate thread
 * @param arg Pointer to the GameInfo_t structure containing the current game
 * state
 * @return EXIT_SUCCESS upon normal completion (e.g., when the game ends)
 * @details Operates as the scheduler thread, periodically queuing downward
 * movement commands for the current tetromino based on the game’s speed (via
 * GET_SLEEP_DURATION). It runs concurrently with the main game loop, ensuring
 * tetrominos drop automatically without requiring constant user input. The
 * thread sleeps between actions, adjusts its timing based on the speed level,
 * and pauses during PauseState. This separation of concerns allows precise
 * control over drop timing independent of other game logic.
 */
int autoShiftScheduler(void* arg);

/**
 * @brief Blocks execution until all tetromino mover threads have completed
 * @note Blocks the calling thread until both the main game loop and scheduler
 * threads finish execution, ensuring proper cleanup and resource release
 * @details Joins the main and scheduler threads (created by initTetrominoMover)
 * to synchronize their termination with the rest of the program. This function
 * is typically called when the game ends (e.g., after setting EndState) to
 * ensure all threaded operations complete before final cleanup. It prevents
 * resource leaks and ensures a clean exit by waiting for both threads to
 * return, regardless of how they terminate (normally or via game end).
 */
void waitTetrominoMoverEnd();

#endif