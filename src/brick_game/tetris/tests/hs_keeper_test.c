#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "../highscore_keeper.h"
#include "test.h"

// Test fixtures
static void setup(void) { remove(SAVE_FILE); }

static void teardown(void) { remove(SAVE_FILE); }

int file_exists(const char* filename) {
  int res = 0;
  FILE* file = fopen(filename, "r");
  if (file) {
    fclose(file);
    res = 1;
  }
  return res;
}

START_TEST(test_getHighscore_no_file_returns_zero) {
  ck_assert_int_eq(getHighscore(), 0);
}
END_TEST

START_TEST(test_setHighscore_creates_file) {
  setHighscore(1000);
  ck_assert(file_exists(SAVE_FILE));
}
END_TEST

START_TEST(test_set_get_highscore_roundtrip) {
  const int test_scores[] = {100, 500, 2500, 9999};

  for (size_t i = 0; i < sizeof(test_scores) / sizeof(test_scores[0]); i++) {
    setHighscore(test_scores[i]);
    ck_assert_int_eq(getHighscore(), test_scores[i]);
  }
}
END_TEST

START_TEST(test_highscore_persistence) {
  setHighscore(3000);

  userInput(Start, false);
  userInput(Terminate, false);
  int score = getHighscore();
  ck_assert_int_eq(score, 3000);
}
END_TEST

START_TEST(test_setHighscore_overwrites_previous) {
  setHighscore(100);
  setHighscore(200);
  ck_assert_int_eq(getHighscore(), 200);
}
END_TEST

// Test suite
Suite* highscore_suite(void) {
  Suite* s;
  TCase* tc_core;

  s = suite_create(NAME("Highscore Keeper"));

  tc_core = tcase_create("Core");
  tcase_add_checked_fixture(tc_core, setup, teardown);

  tcase_add_test(tc_core, test_getHighscore_no_file_returns_zero);
  tcase_add_test(tc_core, test_setHighscore_creates_file);
  tcase_add_test(tc_core, test_set_get_highscore_roundtrip);
  tcase_add_test(tc_core, test_highscore_persistence);
  tcase_add_test(tc_core, test_setHighscore_overwrites_previous);

  suite_add_tcase(s, tc_core);

  return s;
}
