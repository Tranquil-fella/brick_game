/**
 * @file frontend.h
 * @brief Main frontend header for the Brick Game application.
 *
 * Defines all data structures, enumerations, and functions required for:
 * - NCurses-based UI rendering
 * - Game state management
 * - Menu system
 * - Input handling
 */

#ifndef FRONTEND_H
#define FRONTEND_H

#include <menu.h>
#include <ncurses.h>
#include <stdlib.h>
#include <threads.h>

#include "../../brick_game/backend.h"

/**
 * @name Window Dimension Constants
 * @{
 */
#define FIELD_WIN_HEIGHT \
  (FIELD_LENGTH + 2)  ///< Game field window height (rows)
#define FIELD_WIN_WIDTH \
  (FIELD_WIDTH * 2 + 2)         ///< Game field window width (columns)
#define STATS_WIN_WIDTH (12)    ///< Stats window width
#define CONTEXT_WIN_HEIGHT (5)  ///< Context window height
#define SCREEN_WIDTH \
  (FIELD_WIN_WIDTH + STATS_WIN_WIDTH)  ///< Total screen width
#define SCREEN_HEIGHT \
  (FIELD_WIN_HEIGHT + CONTEXT_WIN_HEIGHT)  ///< Total screen height
#define SCREEN_LEFT_OFFSET 0               ///< Left margin for window placement
/** @} */

/**
 * @brief Enumeration of possible game screens
 */
typedef enum {
  MainMenu,        ///< Main menu screen
  ExitMenu,        ///< Exit confirmation screen
  GameSelectMenu,  ///< Game selection screen
  TotalMenus,      ///< Count of menu screens
  GameScreen       ///< Active gameplay screen
} GameScreen_t;

/**
 * @brief Structure holding NCurses window handles
 */
typedef struct {
  WINDOW* field_win;    ///< Window for main game field
  WINDOW* stats_win;    ///< Window for statistics and next piece
  WINDOW* context_win;  ///< Window for contextual information
} GameWindows_t;

/**
 * @brief Structure representing a menu
 */
typedef struct {
  const char* title;  ///< Menu title
  WINDOW* window;     ///< Parent window
  WINDOW* subwindow;  ///< Subwindow for menu items
  MENU* menu;         ///< NCurses menu handle
  ITEM** items;       ///< Array of menu items
  int items_size;     ///< Number of menu items
} Menu_t;

/**
 * @brief Collection of all game menus
 */
typedef struct {
  Menu_t menus[TotalMenus];  ///< Array of all available menus
} GameMenus_t;

/**
 * @brief Function pointer type for input handling
 */
typedef void (*inputFunc_t)(const UserAction_t action, bool hold);

/**
 * @brief Function pointer type for game state updates
 */
typedef GameInfo_t (*updateFunc_t)(void);

/**
 * @brief Game interface structure
 */
typedef struct {
  inputFunc_t userInput;            ///< Function to handle user input
  updateFunc_t updateCurrentState;  ///< Function to update game state
  void* handle;                     ///< Handle to loaded game library
} Interface_t;

/**
 * @brief Game control structure
 */
typedef struct {
  bool prog_on;      ///< Whether program is running
  bool game_on;      ///< Whether game is active
  mtx_t mutex;       ///< Mutex for thread synchronization
  cnd_t cnd;         ///< Condition variable for signaling
  thrd_t game_thrd;  ///< Game thread handle
} Controls_t;

/**
 * @brief Main game data structure
 */
typedef struct {
  Controls_t controls;       ///< Control flags and synchronization
  GameScreen_t current_scr;  ///< Current active screen
  GameWindows_t windows;     ///< NCurses windows
  GameMenus_t game_menus;    ///< Game menus
  Interface_t interface;     ///< Game interface functions
  int mem;                   ///< Memory flags for cleanup
} GameData_t;

/**
 * @brief Field cell color types
 */
typedef enum {
  Empty,    ///< Empty cell
  Static,   ///< Static block
  Damaged,  ///< Damaged block
  Color_1,  ///< Color 1 (Red)
  Color_2,  ///< Color 2 (Magenta)
  Color_3,  ///< Color 3 (Green)
  Color_4,  ///< Color 4 (Magenta)
  Color_5,  ///< Color 5 (Yellow)
  Color_6,  ///< Color 6 (Yellow)
  Color_7   ///< Color 7 (Blue)
} FieldCellColors_t;

/**
 * @brief Initializes the game display
 * @param controls Pointer to game data structure
 * @return OK on success, error code otherwise
 */
int initDisplay(GameData_t* controls);

/**
 * @brief Handles fatal errors
 * @param ctrls Pointer to controls structure
 *
 * @note Displays error message and prepares for program termination
 */
void error(Controls_t* ctrls);

#endif