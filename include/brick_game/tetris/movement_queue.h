/**
 * @file movement_queue.h
 * @brief Circular queue for storing tetromino movement commands.
 * @details
 * - Implements a FIFO queue with a fixed size (`QUEUE_SIZE = 5`).
 * - Automatically overwrites the oldest command when full (circular buffer
 * behavior).
 * - Designed for single-threaded use (no built-in locking).
 */

#ifndef MV_QUEUE_H
#define MV_QUEUE_H

#include "backend.h"
#include "tetromino.h"

/**
 * @brief Initializes or resets the movement queue.
 * @note Also used by `flushQueue` macro to clear the queue.
 */
void initQueue();

/**
 * @brief Removes and returns the oldest command from the queue.
 * @return MoveCommand_t The dequeued movement command.
 * @warning Undefined behavior if the queue is empty (check with
 * `isEmptyQueue()`).
 */
MoveCommand_t popQueue();

/**
 * @brief Adds a new movement command to the queue.
 * @param action The command to enqueue (of type `MoveCommand_t`).
 * @note Overwrites the oldest command if the queue is full.
 */
void pushQueue(const MoveCommand_t action);

/**
 * @brief Checks if the queue is empty.
 * @return `true` if empty, `false` otherwise.
 */
bool isEmptyQueue();

/**
 * @macro flushQueue
 * @brief Clears all commands in the queue (alias for `initQueue`).
 */
#define flushQueue initQueue

#endif