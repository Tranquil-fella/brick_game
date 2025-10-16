/**
 * @file menus.h
 * @brief Menu navigation and game screen management.
 *
 * Provides functions to initialize, navigate, and handle menu interactions,
 * as well as switch between game screens (e.g., main menu, game screen, exit
 * menu).
 */

#ifndef MENUS_H
#define MENUS_H

#include "frontend.h"

/**
 * @brief Initializes game menus (main menu, game selection, exit menu).
 *
 * @param menus Pointer to a `GameMenus_t` structure to populate with menu data.
 * @return int `OK` on success, or an error code if initialization fails.
 *
 * @note Must be called before using any menu navigation functions.
 */
int initGameMenus(GameMenus_t *menus);

/**
 * @brief Frees resources associated with game menus.
 *
 * @param menus Pointer to the `GameMenus_t` structure to clean up.
 *
 * @note Safe to call even if `menus` is partially initialized.
 */
void destroyGameMenus(const GameMenus_t *menus);

/**
 * @brief Handles menu navigation based on user input.
 *
 * @param action User input action (e.g., `Up`, `Down`, `Start`).
 * @param data Pointer to the `GameData_t` structure containing game state and
 * menus.
 *
 * @note Supports vertical navigation (up/down) and selection (`Start`/`Pause`).
 * @warning Thread-safe access to `data->game_menus` must be ensured externally.
 */
void navigateMenu(const UserAction_t action, GameData_t *data);

/**
 * @brief Handles in-game input (e.g., move, rotate, pause).
 *
 * @param action User input action (e.g., `Left`, `Right`, `Terminate`).
 * @param hold If `true`, the action is a continuous hold (e.g., moving left
 * repeatedly).
 * @param data Pointer to the `GameData_t` structure containing game state.
 *
 * @note Delegates input to the loaded game interface
 * (`data->interface.userInput`).
 */
void navigateGameScreen(const UserAction_t action, const bool hold,
                        GameData_t *data);

/**
 * @brief Switches to a new screen and updates the display.
 *
 * @param data Pointer to the `GameData_t` structure.
 * @param scr Target screen to switch to (e.g., `MainMenu`, `GameScreen`).
 *
 * @note Automatically handles menu posting/unposting and game state
 * transitions.
 * @example
 *   postMenu(&data, ExitMenu); // Switches to exit confirmation menu.
 */
void postMenu(GameData_t *data, const GameScreen_t scr);

#endif