/**
 * @file highscore_keeper.h
 * @brief High score persistence system for Tetris
 * @details
 * - Manages reading/writing the highest achieved score to a file
 * - Uses a simple plaintext format for score storage
 * - File is stored at "./data/saves/tetris.score"
 */

#ifndef HIGHSCORE_KEEPER_H
#define HIGHSCORE_KEEPER_H

#define SAVE_FILE "./data/saves/tetris.score"  ///< Path to score storage file

/**
 * @brief Retrieves the current high score from persistent storage
 * @return The highest score recorded (0 if no score exists or file is
 * inaccessible)
 * @note Automatically returns 0 if the score file doesn't exist yet
 */
int getHighscore(void);

/**
 * @brief Updates and persists a new high score
 * @param score The new score to save
 * @note Will overwrite any previous score
 * @warning Does nothing if file cannot be opened for writing
 */
void setHighscore(int score);

#endif