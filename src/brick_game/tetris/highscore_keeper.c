#include "highscore_keeper.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// Helper function to create directory if it doesn't exist
static int create_directory(const char* path) {
  struct stat st = {0};
  if (stat(path, &st) == -1) {
    return mkdir(path, 0755) == 0;
  }
  return 1;  // Directory already exists
}

// Helper function to create nested directories
static int create_directories(const char* path) {
  char temp[1024];
  char* p = NULL;
  size_t len;

  snprintf(temp, sizeof(temp), "%s", path);
  len = strlen(temp);

  if (temp[len - 1] == '/') {
    temp[len - 1] = 0;
  }

  for (p = temp + 1; *p; p++) {
    if (*p == '/') {
      *p = 0;
      if (!create_directory(temp)) {
        return 0;
      }
      *p = '/';
    }
  }
  return create_directory(temp);
}

// Get the proper save directory path
static char* get_save_directory(void) {
  static char path[1000];
  const char* env_var;

  env_var = getenv("HOME");
  if (env_var && env_var[0] != '\0') {
    snprintf(path, sizeof(path), "%s/.local/share/BrickGame/saves", env_var);
    return path;
  }

  // Fallback - current directory
  snprintf(path, sizeof(path), "./../saves");
  return path;
}

// Get the full save file path
static char* get_save_file_path(void) {
  static char file_path[1024];
  const char* save_dir = get_save_directory();

  // Create the directory structure
  create_directories(save_dir);

  snprintf(file_path, sizeof(file_path), "%s/tetris.score", save_dir);

  return file_path;
}

int getHighscore(void) {
  int highscore = 0;
  const char* file_path = get_save_file_path();
  FILE* file = fopen(file_path, "r");
  if (file) {
    int res = fscanf(file, "%d", &highscore);
    if (!res) highscore = 0;
    fclose(file);
  }
  return highscore;
}

void setHighscore(int new_score) {
  const char* file_path = get_save_file_path();
  FILE* file = fopen(file_path, "w");
  if (file) {
    fprintf(file, "%d", new_score);
    fclose(file);
  }
}