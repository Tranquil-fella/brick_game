#include <check.h>
#include <string.h>

#include "../tetromino.h"
#include "../tetromino_mover.c"
#include "test.h"

// Test fixtures
typedef struct {
  GameInfo_t game_info;
  int* field[FIELD_LENGTH];
} TestGameState;

static int field_arr[FIELD_LENGTH * FIELD_WIDTH] = {0};

static TestGameState test_state;
static Tetromino_t test_tetromino;

void setup(void) {
  // Initialize test game state
  memset(&test_state, 0, sizeof(test_state));
  memset(field_arr, 0, sizeof(field_arr));
  for (int i = 0; i < FIELD_LENGTH; ++i) {
    test_state.field[i] = field_arr + (i * FIELD_WIDTH);
  }
  test_state.game_info.field = test_state.field;

  test_tetromino.centerCoords.x = 5;
  test_tetromino.centerCoords.y = -1;
  test_tetromino.shape = I_shape;
  test_tetromino.rotation = 0;
}

void teardown(void) {}

// Helper function to fill a row with specified state
void fill_row(int row, FieldCellState_t state) {
  for (int x = 0; x < FIELD_WIDTH; x++) {
    test_state.field[row][x] = state;
  }
}

// Test cases
START_TEST(test_markRow_marks_correctly) {
  fill_row(5, Settled);
  markRow(test_state.game_info.field, 5);

  for (int x = 0; x < FIELD_WIDTH; x++) {
    ck_assert_int_eq(test_state.field[5][x], Volatile);
  }
  for (int y = 0; y < FIELD_LENGTH; y++) {
    if (y != 5) {
      for (int x = 0; x < FIELD_WIDTH; x++) {
        ck_assert_int_eq(test_state.field[y][x], Empty);
      }
    }
  }
}
END_TEST

START_TEST(test_destroyMarkedRows_no_marked_rows) {
  fill_row(10, Settled);
  fill_row(11, Settled);

  int reward = destroyMarkedRows(&test_state.game_info, &test_tetromino);
  ck_assert_int_eq(reward, 0);

  for (int x = 0; x < FIELD_WIDTH; x++) {
    ck_assert_int_eq(test_state.field[10][x], Settled);
    ck_assert_int_eq(test_state.field[11][x], Settled);
  }
}
END_TEST

START_TEST(test_destroyMarkedRows_single_marked_row) {
  fill_row(10, Settled);
  fill_row(11, Volatile);
  fill_row(12, Settled);

  int reward = destroyMarkedRows(&test_state.game_info, &test_tetromino);
  ck_assert_int_eq(reward, 100);

  for (int x = 0; x < FIELD_WIDTH; x++) {
    ck_assert_int_eq(test_state.field[10][x], Empty);
    ck_assert_int_eq(test_state.field[11][x], Settled);
    ck_assert_int_eq(test_state.field[12][x], Settled);
  }
}
END_TEST

START_TEST(test_destroyMarkedRows_multiple_contiguous_marked_rows) {
  fill_row(8, Settled);
  fill_row(9, Volatile);
  fill_row(10, Volatile);
  fill_row(11, Volatile);
  fill_row(12, Settled);

  int reward = destroyMarkedRows(&test_state.game_info, &test_tetromino);
  ck_assert_int_gt(reward, 100);

  for (int x = 0; x < FIELD_WIDTH; x++) {
    ck_assert_int_eq(test_state.field[8][x], Empty);
    ck_assert_int_eq(test_state.field[9][x], Empty);
    ck_assert_int_eq(test_state.field[10][x], Empty);
    ck_assert_int_eq(test_state.field[11][x], Settled);
    ck_assert_int_eq(test_state.field[12][x], Settled);
  }
}
END_TEST

START_TEST(test_destroyMarkedRows_non_contiguous_marked_rows) {
  fill_row(8, Volatile);
  fill_row(9, Settled);
  fill_row(10, Volatile);
  fill_row(11, Settled);
  fill_row(12, Volatile);

  int reward = destroyMarkedRows(&test_state.game_info, &test_tetromino);
  ck_assert_int_gt(reward, 0);

  for (int x = 0; x < FIELD_WIDTH; x++) {
    ck_assert_int_eq(test_state.field[8][x], Empty);
    ck_assert_int_eq(test_state.field[9][x], Empty);
    ck_assert_int_eq(test_state.field[10][x], Empty);
    ck_assert_int_eq(test_state.field[11][x], Settled);
    ck_assert_int_eq(test_state.field[12][x], Settled);
  }
}
END_TEST

START_TEST(test_destroyRows_shifts_correctly) {
  for (int y = 0; y < 5; y++) {
    fill_row(y, y + 1);
  }

  destroyRows(test_state.game_info.field, 2, 1);

  for (int x = 0; x < FIELD_WIDTH; x++) {
    ck_assert_int_eq(test_state.field[0][x], 0);
    ck_assert_int_eq(test_state.field[1][x], 1);
    ck_assert_int_eq(test_state.field[2][x], 3);
    ck_assert_int_eq(test_state.field[3][x], 4);
    ck_assert_int_eq(test_state.field[4][x], 5);
  }
}
END_TEST

// Test suite
Suite* tetromino_mover_suite(void) {
  Suite* s;
  TCase* tc_core;

  s = suite_create(NAME("Tetromino Mover"));

  tc_core = tcase_create("Core");
  tcase_add_checked_fixture(tc_core, setup, teardown);

  tcase_add_test(tc_core, test_markRow_marks_correctly);
  tcase_add_test(tc_core, test_destroyMarkedRows_no_marked_rows);
  tcase_add_test(tc_core, test_destroyMarkedRows_single_marked_row);
  tcase_add_test(tc_core,
                 test_destroyMarkedRows_multiple_contiguous_marked_rows);
  tcase_add_test(tc_core, test_destroyMarkedRows_non_contiguous_marked_rows);
  tcase_add_test(tc_core, test_destroyRows_shifts_correctly);

  suite_add_tcase(s, tc_core);

  return s;
}
