#ifndef BACKEND_H
#define BACKEND_H

#ifndef __cplusplus
#include <stdbool.h>
#include <stdlib.h>
#endif

#define FIELD_WIDTH 10
#define FIELD_LENGTH 20
#define NEXTF_WIDTH 4
#define NEXTF_LENGTH 4

typedef enum {
  Start,
  Pause,
  Terminate,
  Left,
  Right,
  Up,
  Down,
  Action
} UserAction_t;

typedef struct {
  int **field;
  int **next;
  int score;
  int high_score;
  int level;
  int speed;
  int pause;
} GameInfo_t;

#ifdef __cplusplus
extern "C" {
#endif

void userInput(const UserAction_t action, bool hold);
GameInfo_t updateCurrentState();

#ifdef __cplusplus
}
#endif

#endif