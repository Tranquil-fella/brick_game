/**
 * @file backend.h
 * @brief Main interface for the Brick Game game backend, defining core data
 * structures and functions
 * @details This header serves as the primary interface between the game’s
 * frontend (e.g., UI or input system) and the backend logic. It defines
 * essential constants for the game field dimensions, an enumeration for user
 * actions, the central game state structure (GameInfo_t), and key functions for
 * handling input and state updates. This file is intended to be included by
 * both the game logic implementation and any external systems (like a graphical
 * interface) that need to interact with the game’s state. It provides a clean,
 * minimal API for accessing and manipulating the game’s runtime data without
 * exposing internal implementation details.
 */

#ifndef BACKEND_H
#define BACKEND_H

#include <stdbool.h>  // For boolean type used in function parameters and game state
#include <stdlib.h>  // For memory management functions (e.g., malloc/free) used internally

/**
 * @def FIELD_WIDTH
 * @brief Width of the main game field in cells
 * @details Defines the horizontal size of the Brick Game playing field as 10
 * cells, which is the standard width for a classic Brick Game game. This
 * constant is used to allocate and manage the 2D array representing the game
 * field in the GameInfo_t structure.
 */
#define FIELD_WIDTH 10

/**
 * @def FIELD_LENGTH
 * @brief Height of the main game field in cells
 * @details Sets the vertical size of the Brick Game playing field to 20 cells,
 * adhering to traditional Brick Game dimensions. This determines how many rows
 * are available for figure stacking before the game potentially ends due to
 * overflow.
 */
#define FIELD_LENGTH 20

/**
 * @def NEXTF_WIDTH
 * @brief Width of the "next figure" preview field in cells
 * @details Specifies a 4-cell width for the preview area that displays the
 * upcoming figure. This smaller field ensures the next shape fits within a
 * compact square grid, typically shown to the side of the main field in the UI.
 */
#define NEXTF_WIDTH 4

/**
 * @def NEXTF_LENGTH
 * @brief Height of the "next figure" preview field in cells
 * @details Defines a 4-cell height for the preview area, matching its width to
 * form a square. This size accommodates all figure shapes (which fit within a
 * 4x4 grid) for display to the player before they enter the main field.
 */
#define NEXTF_LENGTH 4

/**
 * @enum UserAction_t
 * @brief Enumeration of all possible user inputs recognized by the game
 * @details This enum lists the distinct actions a player can perform, serving
 * as the primary input vocabulary for the game. Each value corresponds to a
 * specific command or movement, which is processed by the backend to update the
 * game state. The values are designed to be passed to the userInput() function
 * to trigger appropriate responses, such as moving a figure or pausing the
 * game.
 */
typedef enum {
  Start,  ///< Initiates a new game, transitioning from an idle state to active
          ///< play. Typically triggered by a "start" button or key press.
  Pause,  ///< Toggles the game’s pause state, suspending or resuming gameplay.
          ///< Used to temporarily halt figure movement and game progression.
  Terminate,  ///< Ends the current game, stopping all activity and potentially
              ///< saving scores. Often mapped to an "exit" or "quit" command.
  Left,       ///< Moves the current figure one cell to the left, if possible.
         ///< Part of the standard movement controls for positioning pieces.
  Right,  ///< Moves the current figure one cell to the right, if possible.
          ///< Complements the Left action for horizontal positioning.
  Up,     ///< Rotates the current figure clockwise, if space allows.
          ///< Commonly used for adjusting piece orientation during descent.
  Down,   ///< Accelerates the current figure’s downward movement (soft drop).
          ///< Speeds up gameplay by dropping the piece faster than the
          ///< auto-shift.
  Action  ///< Performs a special action, often a hard drop (instant placement).
          ///< Typically locks the figure in place immediately upon execution.
} UserAction_t;

/**
 * @struct GameInfo_t
 * @brief Central structure holding all runtime game state information
 * @details This structure is the heart of the game’s backend, storing all
 * critical data needed to represent the current state of a Brick Game game. It
 * includes the main playing field, the next figure preview, and various game
 * metrics like score and speed. This structure is shared across the system and
 * updated by the backend logic, with its contents accessible to the frontend
 * via updateCurrentState().
 */
typedef struct {
  int **field;  ///< 2D array representing the main game field (FIELD_WIDTH x
                ///< FIELD_LENGTH). Each cell stores an integer indicating its
                ///< state (e.g., empty, occupied).
  int **next;   ///< 2D array for the next figure preview (NEXTF_WIDTH x
                ///< NEXTF_LENGTH). Displays the upcoming shape to be spawned
                ///< into the main field.
  int score;    ///< Current player score, incremented by clearing lines.
                ///< Reflects the player’s progress and success in the game.
  int high_score;  ///< Highest score achieved in this session or historically.
                   ///< Updated when the current score exceeds it upon game end.
  int level;  ///< Current game level, typically tied to difficulty or speed.
              ///< Increases as the player progresses (e.g., every 600 points).
  int speed;  ///< Current drop speed of figures, affecting gameplay pace.
              ///< Adjusted based on level or score milestones.
  int pause;  ///< Pause flag (0 for running, 1 for paused).
              ///< Indicates whether the game is currently suspended.
} GameInfo_t;

/**
 * @brief Processes user input and updates the game state accordingly
 * @param action The UserAction_t value representing the user’s command or
 * movement
 * @param hold Boolean indicating if the input is held (true) or a single press
 * (false)
 * @details This function is the main entry point for handling player
 * interactions. It takes a user action (e.g., Left, Pause) and a hold status,
 * then delegates the processing to the appropriate game logic. For movement
 * actions (Left, Right, Up, Down, Action), the hold parameter may influence
 * behavior (e.g., repeated movement while held). For commands (Start, Pause,
 * Terminate), it triggers state transitions. This function is thread-safe when
 * implemented with proper synchronization (e.g., mutexes) in the backend.
 * @note Expected to be called by the frontend whenever user input is detected,
 * such as a key press or button click.
 */
void userInput(const UserAction_t action, bool hold);

/**
 * @brief Retrieves a snapshot of the current game state for external use
 * @return GameInfo_t structure containing the latest game data
 * @details Provides a copy of the current GameInfo_t structure, allowing the
 * frontend (e.g., UI) to access and display the game’s state without directly
 * modifying the shared backend data. This function ensures thread safety by
 * locking the game state during the copy operation (when implemented). The
 * returned data includes the field layout, next figure, score, and other
 * metrics, making it suitable for rendering or decision-making outside the
 * backend.
 * @note The returned structure is a snapshot; subsequent backend updates won’t
 * affect it until this function is called again.
 */
GameInfo_t updateCurrentState();

#endif