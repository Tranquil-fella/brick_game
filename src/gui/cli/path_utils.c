#include "path_utils.h"

#include <libgen.h>
#include <stdio.h>
#include <string.h>

int getLibsPath(char *libs_path, size_t size) {
  char exe_path[MAX_PATH_LEN];
  char exe_copy[MAX_PATH_LEN];

  ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
  if (len == -1) {
    return 0;
  }
  exe_path[len] = '\0';

  strncpy(exe_copy, exe_path, sizeof(exe_copy) - 1);
  exe_copy[sizeof(exe_copy) - 1] = '\0';

  const char *exe_dir = dirname(exe_copy);
  int result = snprintf(libs_path, size, "%s/../lib", exe_dir);

  return (result > 0 && (size_t)result < size);
}