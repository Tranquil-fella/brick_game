#include "controller.h"

#include <check.h>
#include <stdlib.h>

#include "game_data.h"
#include "test.h"

// Test cases
START_TEST(test_getController_returns_valid_controller) {
  Controller_t* controller = getController();
  ck_assert_ptr_nonnull(controller);
  ck_assert_ptr_nonnull(controller->getAction);
  ck_assert_ptr_nonnull(controller->exec);
}
END_TEST

START_TEST(test_controller_game_on_off) {
  ck_assert(isGameState(StartState));
  Controller_t* controller = getController();
  controller->getAction(Start, false);
  controller->exec();
  ck_assert(isGameState(RunState));
  controller->getAction(Terminate, false);
  controller->exec();
  ck_assert(isGameState(StartState));
}
END_TEST

START_TEST(test_controller_game_pause) {
  GameInfo_t info;
  userInput(Start, false);
  ck_assert(isGameState(RunState));
  userInput(Pause, false);
  info = updateCurrentState();
  ck_assert_int_eq(info.pause, 1);
  ck_assert(isGameState(PauseState));
  userInput(Pause, false);
  info = updateCurrentState();
  ck_assert_int_eq(info.pause, 0);
  ck_assert(isGameState(RunState));
  userInput(Pause, false);
  ck_assert(isGameState(PauseState));
  userInput(Terminate, false);
  ck_assert(isGameState(StartState));
}
END_TEST

START_TEST(test_controller_game_over) {
  userInput(Start, false);
  ck_assert(isGameState(RunState));
  for (int i = 0; i < 12; ++i) {
    userInput(Down, true);
    thrd_sleep(&(const struct timespec){.tv_nsec = 10000000}, NULL);
  }
  ck_assert(isGameState(EndState));
  userInput(Start, false);
  ck_assert(isGameState(EndState));
  GameInfo_t info = updateCurrentState();
  ck_assert_int_eq(info.level, 0);
  userInput(Terminate, false);
}
END_TEST

// Test suite
Suite* controller_suite(void) {
  Suite* s;
  TCase* tc_core;

  s = suite_create(NAME("Controller Interface"));
  tc_core = tcase_create("Core");

  tcase_add_test(tc_core, test_getController_returns_valid_controller);
  tcase_add_test(tc_core, test_controller_game_on_off);
  tcase_add_test(tc_core, test_controller_game_pause);
  tcase_add_test(tc_core, test_controller_game_over);

  suite_add_tcase(s, tc_core);

  return s;
}
