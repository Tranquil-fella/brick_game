/**
 * @file games_finder.h
 * @brief Header for game library discovery utility.
 *
 * Provides functionality to scan a directory for shared libraries (.so files)
 * and retrieve cleaned-up game names.
 */

#ifndef GAME_FINDER_H
#define GAME_FINDER_H

#define MAX_LIBS 10  ///< Maximum number of supported game libraries.

/**
 * @brief Stores a list of available game names.
 *
 * Holds an array of game names (extracted from shared library filenames)
 * and the current number of discovered games.
 */
typedef struct {
  const char *games[MAX_LIBS];  ///< Array of game name strings.
  int size;                     ///< Number of valid entries in `games`.
} Games_t;

/**
 * @brief Scans for shared libraries and returns discovered games.
 *
 * Searches for shared libraries in the "lib" directory relative to the
 * executable location.
 *
 * @return const Games_t* Pointer to a static `Games_t` structure containing
 *         up to `MAX_LIBS` game names. NULL if no games are found.
 *
 * @note The returned pointer is statically allocated; do not free() it.
 * @example
 *   const Games_t *games = getAvailableGames();
 *   for (int i = 0; i < games->size; i++) {
 *     printf("Game %d: %s\n", i, games->games[i]);
 *   }
 */
const Games_t *getAvailableGames();

#endif