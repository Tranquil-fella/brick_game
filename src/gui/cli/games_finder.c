#include "games_finder.h"

#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>

#include "path_utils.h"

#define MAX_LEN 256
#define MAX_NAME_LEN 24

typedef struct {
  char names_arr[MAX_LEN];
  char *current_name;
  size_t len;
} NamesStorage_t;

void scanDirectory(Games_t *games, const char *libs_path);
void cleanupName(const char *filename, const char **start, const char **end);
const char *getSavedName(const char *start, const char *end,
                         NamesStorage_t *names);
const char *getCleanLibName(const char *filename, NamesStorage_t *names);
int is_shared_lib(const char *filename);

const Games_t *getAvailableGames() {
  static Games_t games = {0};
  games.size = 0;  // Reset on each call

  char libs_path[MAX_PATH_LEN];

  if (getLibsPath(libs_path, sizeof(libs_path))) {
    scanDirectory(&games, libs_path);
  }

  return (const Games_t *)&games;
}

const char *getCleanLibName(const char *filename, NamesStorage_t *names) {
  const char *start, *end;
  cleanupName(filename, &start, &end);
  return getSavedName(start, end, names);
}

void cleanupName(const char *filename, const char **start, const char **end) {
  *start = filename;
  if (strncmp(filename, "lib", 3) == 0) {
    *start = filename + 3;
  }
  *end = strstr(*start, ".so");
  if (*end == NULL) {
    *end = *start + strlen(*start);
  }
}

const char *getSavedName(const char *start, const char *end,
                         NamesStorage_t *names) {
  char *last_name = names->current_name;
  size_t len = end - start;
  size_t remaining_space = MAX_LEN - names->len;

  if (len >= remaining_space)
    len = remaining_space - 1;
  else if (len > MAX_NAME_LEN)
    len = MAX_NAME_LEN;

  for (size_t i = 0; i < len; ++i) {
    last_name[i] = toupper(start[i]);
  }
  last_name[len] = '\0';
  names->current_name += len + 1;
  names->len += len + 1;
  return last_name;
}

int is_shared_lib(const char *filename) {
  size_t len = strlen(filename);
  return (len > 3 && strstr(filename, ".so"));
}

// Scan a directory for shared libraries
void scanDirectory(Games_t *games, const char *libs_path) {
  static NamesStorage_t names;
  names.current_name = names.names_arr;
  names.len = 0;

  DIR *dir;
  const struct dirent *entry;

  if ((dir = opendir(libs_path))) {
    while ((entry = readdir(dir)) != NULL && games->size < MAX_LIBS &&
           names.len < MAX_LEN) {
      if (is_shared_lib(entry->d_name)) {
        games->games[games->size++] = getCleanLibName(entry->d_name, &names);
      }
    }
    closedir(dir);
  }
}