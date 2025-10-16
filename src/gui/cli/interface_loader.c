#include "interface_loader.h"

#include <ctype.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "path_utils.h"

#define LIB_NAME_MAX_SIZE 24
#define LIB_FULLNAME_MAX_SIZE 1024 + 64

void strToLower(char *dst, const char *src);
void getLibPath(const char *game_name, char *lib_path);

int loadGameInterface(const char *game_name, Interface_t *interface) {
  int result = EXIT_SUCCESS;
  char lib_path[LIB_FULLNAME_MAX_SIZE];

  getLibPath(game_name, lib_path);
  interface->handle = dlopen(lib_path, RTLD_LAZY);

  if (!interface->handle) {
    result = EXIT_FAILURE;
  } else {
    interface->userInput = (inputFunc_t)dlsym(interface->handle, "userInput");
    const char *dlsym_error = dlerror();
    if (dlsym_error) {
      dlclose(interface->handle);
      result = EXIT_FAILURE;
    } else {
      interface->updateCurrentState =
          (updateFunc_t)dlsym(interface->handle, "updateCurrentState");
      dlsym_error = dlerror();
      if (dlsym_error) {
        dlclose(interface->handle);
        result = EXIT_FAILURE;
      }
    }
  }

  return result;
}

/**
 * @brief Constructs the full path to the game library
 */
void getLibPath(const char *game_name, char *lib_path) {
  char name[LIB_NAME_MAX_SIZE];
  char libs_dir[MAX_PATH_LEN];

  strToLower(name, game_name);

  // Get the libs directory path relative to executable
  if (getLibsPath(libs_dir, sizeof(libs_dir))) {
    snprintf(lib_path, LIB_FULLNAME_MAX_SIZE, "%s/lib%s.so", libs_dir, name);
  } else {
    // Fallback: use relative path if executable path resolution fails
    snprintf(lib_path, LIB_FULLNAME_MAX_SIZE, "../lib/lib%s.so", name);
  }
}

void unloadGameInterface(Interface_t *interface) {
  if (interface->handle) {
    dlclose(interface->handle);
  }
  interface->handle = NULL;
  interface->updateCurrentState = NULL;
  interface->userInput = NULL;
}

void strToLower(char *dst, const char *src) {
  size_t i = 0;
  while (i < LIB_NAME_MAX_SIZE - 1 && src[i] != '\0') {
    dst[i] = tolower((unsigned char)src[i]);
    i++;
  }
  dst[i] = '\0';
}