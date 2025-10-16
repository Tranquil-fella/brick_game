#include <check.h>
#include <stdio.h>

#include "backend.h"

#define LIGHT_GREEN "\033[32;1m"
#define DARK_RED "\033[31;1m"
#define NOCOLOR "\033[0m"
#define YELLOW "\033[33;1m"
#define NAME(x) YELLOW x NOCOLOR

Suite* controller_suite(void);
Suite* queue_suite(void);
// Suite* highscore_suite(void);
Suite* tetromino_suite(void);
Suite* tetromino_mover_suite(void);
