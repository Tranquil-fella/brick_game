#include "tetromino.h"

#include <check.h>
#include <stdlib.h>
#include <time.h>

#include "test.h"
#include "tetromino_inner.h"

#define TEST_START_Y 1

// Test fixtures
typedef struct {
  GameInfo_t game_info;
  int* field[FIELD_LENGTH];
  int* next[NEXTF_LENGTH];
} TestGameState;

static TestGameState test_state;
static Tetromino_t test_tetromino;
static int field_arr[FIELD_LENGTH * FIELD_WIDTH] = {0};
static int next_arr[NEXTF_LENGTH * NEXTF_WIDTH + 1];

static void setup(void) {
  srand(time(NULL));

  memset(&test_state, 0, sizeof(test_state));
  memset(field_arr, 0, sizeof(field_arr));
  memset(next_arr, 0, sizeof(next_arr));

  for (int i = 0; i < FIELD_LENGTH; ++i) {
    test_state.field[i] = field_arr + (i * FIELD_WIDTH);
  }
  for (int i = 0; i < NEXTF_LENGTH; ++i) {
    test_state.next[i] = next_arr + (i * NEXTF_WIDTH);
  }

  test_state.game_info.field = test_state.field;
  test_state.game_info.next = test_state.next;

  test_tetromino.centerCoords.x = START_X;
  test_tetromino.centerCoords.y = TEST_START_Y;
  test_tetromino.shape = I_shape;
  test_tetromino.rotation = Angle0;
}

static void teardown(void) {}

// Test cases
START_TEST(test_getMovement_returns_correct_functions) {
  MoveCommand_t commands[] = {
      {.move = 0, .hold = false},  // Left
      {.move = 1, .hold = false},  // Right
      {.move = 3, .hold = false},  // Down
      {.move = 4, .hold = false},  // Rotate
      {.move = 3, .hold = true}    // Smash down
  };

  ck_assert_ptr_eq(getMovement(commands[0]), moveLeft);
  ck_assert_ptr_eq(getMovement(commands[1]), moveRight);
  ck_assert_ptr_eq(getMovement(commands[2]), moveDown);
  ck_assert_ptr_eq(getMovement(commands[3]), rotate);
  ck_assert_ptr_eq(getMovement(commands[4]), smashDown);
}
END_TEST

START_TEST(test_moveDown_successful) {
  int result = moveDown(&test_state.game_info, &test_tetromino);
  ck_assert_int_eq(result, EXIT_SUCCESS);
  ck_assert_int_eq(test_tetromino.centerCoords.y, TEST_START_Y + 1);
}
END_TEST

START_TEST(test_moveDown_collision) {
  test_state.field[TEST_START_Y + 2][START_X] = Settled;

  int result = moveDown(&test_state.game_info, &test_tetromino);
  if (!result) result = moveDown(&test_state.game_info, &test_tetromino);
  ck_assert_int_eq(result, EXIT_FAILURE);
  ck_assert_int_eq(test_tetromino.centerCoords.y, START_Y);
}
END_TEST

START_TEST(test_smashDown_moves_to_bottom) {
  int result = smashDown(&test_state.game_info, &test_tetromino);
  ck_assert_int_eq(result, EXIT_SUCCESS);
  ck_assert_int_eq(test_tetromino.centerCoords.y, START_Y);
}
END_TEST

START_TEST(test_moveLeft_successful) {
  int result = moveLeft(&test_state.game_info, &test_tetromino);
  ck_assert_int_eq(result, EXIT_SUCCESS);
  ck_assert_int_eq(test_tetromino.centerCoords.x, START_X - 1);
}
END_TEST

START_TEST(test_moveLeft_collision) {
  test_tetromino.centerCoords.x = 0;
  int result = moveLeft(&test_state.game_info, &test_tetromino);
  ck_assert_int_eq(result, EXIT_FAILURE);
}
END_TEST

START_TEST(test_moveRight_successful) {
  int result = moveRight(&test_state.game_info, &test_tetromino);
  ck_assert_int_eq(result, EXIT_SUCCESS);
  ck_assert_int_eq(test_tetromino.centerCoords.x, START_X + 1);
}
END_TEST

START_TEST(test_moveRight_collision) {
  test_tetromino.centerCoords.x = FIELD_WIDTH - 1;
  int result = moveRight(&test_state.game_info, &test_tetromino);
  ck_assert_int_eq(result, EXIT_FAILURE);
}
END_TEST

START_TEST(test_rotate_successful) {
  int result = rotate(&test_state.game_info, &test_tetromino);
  ck_assert_int_eq(result, EXIT_SUCCESS);
  ck_assert_int_eq(test_tetromino.rotation, Angle90);
}
END_TEST

START_TEST(test_rotate_I_shape_limited) {
  test_tetromino.shape = I_shape;
  test_tetromino.centerCoords.x = -3;
  rotate(&test_state.game_info, &test_tetromino);
  int result = rotate(&test_state.game_info, &test_tetromino);
  ck_assert_int_eq(result, EXIT_FAILURE);
  ck_assert_int_eq(test_tetromino.rotation, Angle0);
}
END_TEST

START_TEST(test_getNextTetromino_returns_valid_tetromino) {
  test_state.next[3][4] = O_shape;
  Tetromino_t tetr = getNextTetromino(test_state.game_info.next);
  ck_assert_int_eq(tetr.shape, O_shape);
  ck_assert_int_eq(tetr.centerCoords.x, START_X);
  ck_assert_int_eq(tetr.centerCoords.y, START_Y);
}
END_TEST

START_TEST(test_setRandomShape_updates_next) {
  setRandomShape(test_state.game_info.next);
  ck_assert_int_ge(test_state.next[3][4], I_shape);
  ck_assert_int_le(test_state.next[3][4], L_shape);
}
END_TEST

START_TEST(test_putTetromino_updates_field) {
  putTetromino(&test_tetromino, test_state.game_info.field);
  ck_assert_int_eq(test_state.field[TEST_START_Y][START_X], I_shape);
}
END_TEST

START_TEST(test_removeTetromino_clears_field) {
  putTetromino(&test_tetromino, test_state.game_info.field);
  removeTetromino(&test_tetromino, test_state.game_info.field);
  ck_assert_int_eq(test_state.field[TEST_START_Y][START_X], Empty);
}
END_TEST

START_TEST(test_settleTetromino_sets_settled_state) {
  settleTetromino(&test_tetromino, test_state.game_info.field);
  ck_assert_int_eq(test_state.field[TEST_START_Y][START_X], Settled);
}
END_TEST

// Test suite
Suite* tetromino_suite(void) {
  Suite* s;
  TCase* tc_core;

  s = suite_create(NAME("Tetromino"));

  tc_core = tcase_create("Core");
  tcase_add_checked_fixture(tc_core, setup, teardown);

  tcase_add_test(tc_core, test_getMovement_returns_correct_functions);
  tcase_add_test(tc_core, test_moveDown_successful);
  tcase_add_test(tc_core, test_moveDown_collision);
  tcase_add_test(tc_core, test_smashDown_moves_to_bottom);
  tcase_add_test(tc_core, test_moveLeft_successful);
  tcase_add_test(tc_core, test_moveLeft_collision);
  tcase_add_test(tc_core, test_moveRight_successful);
  tcase_add_test(tc_core, test_moveRight_collision);
  tcase_add_test(tc_core, test_rotate_successful);
  tcase_add_test(tc_core, test_rotate_I_shape_limited);
  tcase_add_test(tc_core, test_getNextTetromino_returns_valid_tetromino);
  tcase_add_test(tc_core, test_setRandomShape_updates_next);
  tcase_add_test(tc_core, test_putTetromino_updates_field);
  tcase_add_test(tc_core, test_removeTetromino_clears_field);
  tcase_add_test(tc_core, test_settleTetromino_sets_settled_state);

  suite_add_tcase(s, tc_core);

  return s;
}
