/**
 * @file input.h
 * @brief Input processing and action mapping for game controls.
 *
 * Translates keyboard inputs into game actions and delegates them to the
 * appropriate handlers (menu navigation or in-game controls).
 */

#ifndef INPUT_H
#define INPUT_H

#include "frontend.h"

#define ESCAPE 27     ///< ASCII code for ESC key (used for termination).
#define ENTER_KEY 10  ///< ASCII code for Enter key (used for selections).
#define SPACEBAR 32   ///< ASCII code for Spacebar (used for pause).

/**
 * @brief Processes user input and triggers corresponding game actions.
 *
 * Continuously polls for input and delegates actions to either:
 * - `navigateMenu()` (for menus)
 * - `navigateGameScreen()` (for in-game controls)
 *
 * @param data Pointer to the `GameData_t` structure containing game state.
 *
 * @note Runs in a loop while `data->controls.prog_on` is true.
 * @warning Uses blocking `getch()`; ensure this is called in a dedicated
 * thread.
 *
 */
void processInput(GameData_t* data);

#endif