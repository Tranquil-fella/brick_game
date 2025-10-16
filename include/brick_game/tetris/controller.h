/**
 * @file controller.h
 * @brief Defines the game controller interface for handling user input and game
 * commands
 * @details This header file provides the core interface for managing user
 * interactions within the Tetris game. It defines a controller structure that
 * encapsulates two key function pointers: one for interpreting user actions
 * (e.g., key presses) and another for executing the resulting commands or
 * movements. The controller acts as a bridge between raw user input (like
 * pressing "left" or "pause") and the game logic that processes those inputs,
 * ensuring that actions are handled appropriately based on the current game
 * state. It’s designed to be lightweight and reusable, with a single global
 * instance accessible via getController().
 */

#ifndef CONTROLLER_H
#define CONTROLLER_H

/**
 * @typedef Command_t
 * @brief Function pointer type for commands that take no arguments and return
 * nothing
 * @details Represents a generic command function that can be executed by the
 * controller. These commands typically correspond to high-level game actions
 * such as starting the game, pausing it, or terminating it. The void return
 * type and lack of parameters make it flexible for various use cases, allowing
 * the controller to trigger state changes or other operations without needing
 * specific data passed directly to the command.
 */
typedef void (*Command_t)(void);

/**
 * @struct Controller_t
 * @brief Structure defining the controller's interface for handling user
 * actions
 * @details This structure contains two function pointers that form the backbone
 * of the game's input handling system. The 'getAction' function interprets user
 * input (e.g., a key press and whether it’s held) and determines what should
 * happen, while the 'exec' function carries out the resulting action. Together,
 * they provide a two-step process: interpret the input, then execute the
 * command or movement. This separation allows for flexibility in how actions
 * are processed and executed.
 */
typedef struct {
  /**
   * @brief Function pointer to interpret user input into actionable commands or
   * movements
   * @param action Integer representing the user’s input (e.g., mapped from key
   * codes)
   * @param hold Integer indicating if the key is held (typically 1 for held, 0
   * for not)
   * @details This function takes raw input data and translates it into either a
   * movement command (e.g., move tetromino left) or a game command (e.g.,
   * pause). It stores the result in a shared actions structure for later
   * execution by the 'exec' function. The use of integers allows for easy
   * mapping from key codes or other input systems.
   */
  void (*getAction)(const int, const int);

  /**
   * @brief Function pointer to execute the interpreted action
   * @details Executes whatever command or movement was determined by
   * 'getAction'. This could involve starting the game, pausing it, moving a
   * tetromino, or terminating the game. It’s responsible for ensuring the
   * action is carried out in a thread-safe manner when necessary (e.g., by
   * locking mutexes).
   */
  void (*exec)(void);
} Controller_t;

/**
 * @brief Retrieves the singleton instance of the game controller
 * @return Pointer to the static Controller_t structure containing function
 * pointers
 * @details Provides access to the game’s controller, which is implemented as a
 * static singleton in the corresponding .c file. This ensures there’s only one
 *          controller instance throughout the game, simplifying management of
 * user input. The returned structure is pre-initialized with the appropriate
 *          function pointers (getAction and execAction), ready to process user
 * input immediately. This function is typically called whenever user input
 * needs to be handled, such as in the main event loop or input processing
 * routine.
 * @note The controller is not thread-safe by itself; any multi-threaded use
 * requires proper synchronization (e.g., via the mutex from game_data.h).
 */
Controller_t* getController();

#endif