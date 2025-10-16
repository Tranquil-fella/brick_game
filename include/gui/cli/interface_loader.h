/**
 * @file interface_loader.h
 * @brief Dynamic loading and unloading of game interface modules.
 *
 * Provides functions to load/unload game interfaces (shared libraries) and bind
 * their exported functions to an `Interface_t` structure.
 */

#ifndef INTERFACE_LOADER
#define INTERFACE_LOADER

#include "frontend.h"

/**
 * @brief Loads a game interface (shared library) and binds its functions.
 *
 * @param game_name Name of the game (e.g., "Pacman"). Case-insensitive.
 * @param interface Pointer to an `Interface_t` struct to populate with loaded
 * functions.
 * @return int `EXIT_SUCCESS` on success, `EXIT_FAILURE` on error (e.g., library
 * not found).
 *
 * @note The library path is constructed as
 * `data/libs/lib<game_name_lowercase>.so`.
 * @warning Always check the return value. On failure, `interface` may be
 * partially initialized.
 *
 * @example
 *   Interface_t interface;
 *   if (loadGameInterface("Pacman", &interface) == EXIT_SUCCESS) {
 *     // Use interface.userInput() and interface.updateCurrentState()
 *   }
 */
int loadGameInterface(const char *game_name, Interface_t *interface);

/**
 * @brief Unloads a game interface and resets the `Interface_t` struct.
 *
 * @param interface Pointer to the `Interface_t` struct to clean up.
 *
 * @note Safe to call even if `interface` is partially loaded or NULL.
 * @post `interface->handle`, `interface->userInput`, and
 * `interface->updateCurrentState` are set to NULL.
 */
void unloadGameInterface(Interface_t *interface);

#endif