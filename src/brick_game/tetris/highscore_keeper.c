#include "highscore_keeper.h"

#include <stdio.h>
#include <stdlib.h>

int getHighscore(void) {
  int highscore = 0;
  FILE *file = fopen(SAVE_FILE, "r");
  if (file) {
    fscanf(file, "%d", &highscore);
    fclose(file);
  }
  return highscore;
}

void setHighscore(int new_score) {
  FILE *file = fopen(SAVE_FILE, "w");
  if (file) {
    fprintf(file, "%d", new_score);
    fclose(file);
  }
}