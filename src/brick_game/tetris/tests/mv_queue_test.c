#include <check.h>
#include <stdlib.h>

#include "../movement_queue.h"
#include "test.h"

static MoveCommand_t test_commands[5] = {{.move = 0, .hold = false},
                                         {.move = 1, .hold = true},
                                         {.move = 2, .hold = false},
                                         {.move = 3, .hold = true},
                                         {.move = 4, .hold = false}};

static void setup(void) {
  initQueue();  // Reset queue before each test
}

static void teardown(void) {}

// Test cases
START_TEST(test_initQueue_creates_empty_queue) { ck_assert(isEmptyQueue()); }
END_TEST

START_TEST(test_pushQueue_adds_elements) {
  pushQueue(test_commands[0]);
  ck_assert(!isEmptyQueue());

  pushQueue(test_commands[1]);
  ck_assert(!isEmptyQueue());
}
END_TEST

START_TEST(test_popQueue_returns_elements_in_order) {
  for (int i = 0; i < 3; i++) {
    pushQueue(test_commands[i]);
  }
  // Verify FIFO order
  for (int i = 0; i < 3; i++) {
    MoveCommand_t cmd = popQueue();
    ck_assert_int_eq(cmd.move, test_commands[i].move);
    ck_assert_int_eq(cmd.hold, test_commands[i].hold);
  }

  ck_assert(isEmptyQueue());
}
END_TEST

START_TEST(test_popQueue_empty_queue_behavior) {
  MoveCommand_t cmd = popQueue();
  ck_assert_int_eq(cmd.move, 0);
}
END_TEST

START_TEST(test_isEmptyQueue_accurate) {
  ck_assert(isEmptyQueue());
  pushQueue(test_commands[0]);
  ck_assert(!isEmptyQueue());
  popQueue();
  ck_assert(isEmptyQueue());
}
END_TEST

START_TEST(test_flushQueue_equivalent_to_init) {
  pushQueue(test_commands[0]);
  pushQueue(test_commands[1]);
  flushQueue();
  ck_assert(isEmptyQueue());

  MoveCommand_t cmd = popQueue();
  ck_assert_int_eq(cmd.move, 0);
}
END_TEST

// Test suite
Suite* queue_suite(void) {
  Suite* s;
  TCase* tc_core;

  s = suite_create(NAME("Move Queue"));

  tc_core = tcase_create("Core");
  tcase_add_checked_fixture(tc_core, setup, teardown);

  tcase_add_test(tc_core, test_initQueue_creates_empty_queue);
  tcase_add_test(tc_core, test_pushQueue_adds_elements);
  tcase_add_test(tc_core, test_popQueue_returns_elements_in_order);
  tcase_add_test(tc_core, test_popQueue_empty_queue_behavior);
  tcase_add_test(tc_core, test_isEmptyQueue_accurate);
  tcase_add_test(tc_core, test_flushQueue_equivalent_to_init);

  suite_add_tcase(s, tc_core);

  return s;
}
