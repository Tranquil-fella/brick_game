/**
 * @file game_field.h
 * @brief Game screen rendering and display management.
 *
 * Provides functions to render the game field, next piece, statistics, and game
 * context.
 */

#ifndef GAME_FIELD_H
#define GAME_FIELD_H

/**
 * @brief Renders the entire game screen (field, stats, next piece, etc.).
 *
 * @param arg Pointer to a `GameData_t` structure containing game state and
 * window handles.
 * @return int `OK` on success, or an error code if rendering fails.
 *
 * @note This function runs in a loop while `data->controls.game_on` is true.
 * @warning Requires thread-safe access to `GameData_t` (uses mutex locks).
 */
int printGameScreen(void* arg);

#endif