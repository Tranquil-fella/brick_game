#include "interface_loader.h"

#include <ctype.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>

#include "games_finder.h"

#define LIB_NAME_MAX_SIZE 24
#define LIB_FULLNAME_MAX_SIZE \
  LIB_NAME_MAX_SIZE + sizeof(LIBS_PATH) + sizeof("/lib.so")

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

void getLibPath(const char *game_name, char *lib_path) {
  char name[LIB_NAME_MAX_SIZE];
  strToLower(name, game_name);
  snprintf(lib_path, LIB_FULLNAME_MAX_SIZE, LIBS_PATH "/lib%s.so", name);
}

void unloadGameInterface(Interface_t *interface) {
  dlclose(interface->handle);
  interface->handle = NULL;
  interface->updateCurrentState = NULL;
  interface->userInput = NULL;
}

void strToLower(char *dst, const char *const src) {
  size_t i = 0;
  while (i < LIB_NAME_MAX_SIZE && src[i] != '\0') {
    dst[i] = tolower((unsigned char)src[i]);
    i++;
  }
  dst[i] = '\0';
}