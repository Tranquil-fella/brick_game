#ifndef PATH_UTILS_H
#define PATH_UTILS_H

#define MAX_PATH_LEN 1024
#include <unistd.h>

int getLibsPath(char *libs_path, size_t size);

#endif