
#include "movement_queue.h"

#include <string.h>

#define QUEUE_SIZE 5

/**
 * @brief Internal queue structure (circular buffer).
 */
typedef struct {
  MoveCommand_t movements[QUEUE_SIZE];  ///< Array storing queued commands.
  int first;                            ///< Index of the oldest command.
  int last;  ///< Index where the next command will be inserted.
} MovementQueue_t;

MovementQueue_t* getQueue() {
  static MovementQueue_t queue;
  return &queue;
}

void initQueue() {
  MovementQueue_t* queue = getQueue();
  memset(queue, 0, sizeof(MovementQueue_t));
}

MoveCommand_t popQueue() {
  MovementQueue_t* queue = getQueue();
  const MoveCommand_t action = queue->movements[queue->first];
  queue->first = (queue->first + 1) % QUEUE_SIZE;
  return action;
}

void pushQueue(const MoveCommand_t action) {
  MovementQueue_t* queue = getQueue();
  queue->movements[queue->last] = action;
  queue->last = (queue->last + 1) % QUEUE_SIZE;
  if (queue->first == queue->last)
    queue->first = (queue->first + 1) % QUEUE_SIZE;
}

bool isEmptyQueue() {
  const MovementQueue_t* queue = getQueue();
  return queue->last == queue->first;
}
