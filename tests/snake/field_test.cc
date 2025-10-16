#include "field.h"

#include <gtest/gtest.h>

#include "field.h"

class FieldTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Initialize any test data if needed
  }

  void TearDown() override {
    // Clean up if needed
  }

  brick_game::Field field;
};

TEST_F(FieldTest, GetNthFreeCell_EmptyField) {
  // Test getting free cells from completely empty field
  auto cell0 = field.GetNthFreeCell(0);
  EXPECT_FALSE(field.CheckCell(cell0));
  EXPECT_EQ(cell0.first, 0);
  EXPECT_EQ(cell0.second, 0);

  auto cell1 = field.GetNthFreeCell(1);
  EXPECT_FALSE(field.CheckCell(cell1));
  EXPECT_EQ(cell1.first, 0);
  EXPECT_EQ(cell1.second, 1);

  // Test some arbitrary positions
  auto cell42 = field.GetNthFreeCell(42);
  EXPECT_FALSE(field.CheckCell(cell42));
  EXPECT_EQ(cell42.first, 4);
  EXPECT_EQ(cell42.second, 2);
}

TEST_F(FieldTest, GetNthFreeCell_PartiallyFilled) {
  // Fill some specific cells
  field.FillCell({0, 0});
  field.FillCell({0, 1});
  field.FillCell({1, 0});

  // First free cell should be at (0, 2)
  auto cell0 = field.GetNthFreeCell(0);
  EXPECT_FALSE(field.CheckCell(cell0));
  EXPECT_EQ(cell0.first, 0);
  EXPECT_EQ(cell0.second, 2);

  // Second free cell should be at (0, 3)
  auto cell1 = field.GetNthFreeCell(1);
  EXPECT_FALSE(field.CheckCell(cell1));
  EXPECT_EQ(cell1.first, 0);
  EXPECT_EQ(cell1.second, 3);

  // Test skipping multiple filled cells
  auto cell10 = field.GetNthFreeCell(10);
  EXPECT_FALSE(field.CheckCell(cell10));
}

TEST_F(FieldTest, GetNthFreeCell_FullyFilledField) {
  // Fill the entire field
  for (int y = 0; y < FIELD_LENGTH; ++y) {
    for (int x = 0; x < FIELD_WIDTH; ++x) {
      field.FillCell({y, x});
    }
  }

  // This should not segfault and should return a valid cell (wraps around to
  // 0,0)
  EXPECT_NO_THROW({
    auto cell = field.GetNthFreeCell(0);
    // Since all cells are filled, it should return (0,0) due to the bounds
    // check
    EXPECT_EQ(cell.first, 0);
    EXPECT_EQ(cell.second, 0);
    EXPECT_TRUE(field.CheckCell(cell));  // This cell is filled
  });

  // Test with large n values that exceed total cells
  EXPECT_NO_THROW({
    auto cell = field.GetNthFreeCell(1000);
    EXPECT_EQ(cell.first, 0);
    EXPECT_EQ(cell.second, 0);
  });

  EXPECT_NO_THROW({
    auto cell = field.GetNthFreeCell(FIELD_LENGTH * FIELD_WIDTH + 100);
    EXPECT_EQ(cell.first, 0);
    EXPECT_EQ(cell.second, 0);
  });
}

TEST_F(FieldTest, GetNthFreeCell_EdgeCases) {
  // Test exactly at the boundary
  auto last_cell = field.GetNthFreeCell(FIELD_LENGTH * FIELD_WIDTH - 1);
  EXPECT_EQ(last_cell.first, FIELD_LENGTH - 1);
  EXPECT_EQ(last_cell.second, FIELD_WIDTH - 1);
  EXPECT_FALSE(field.CheckCell(last_cell));
}

TEST_F(FieldTest, GetNthFreeCell_AlternatingPattern) {
  // Create a checkerboard pattern
  for (int y = 0; y < FIELD_LENGTH; ++y) {
    for (int x = 0; x < FIELD_WIDTH; ++x) {
      if ((y + x) % 2 == 0) {
        field.FillCell({y, x});
      }
    }
  }

  // Verify we can get free cells without segfault
  for (int i = 0; i < 50; ++i) {
    EXPECT_NO_THROW({
      auto cell = field.GetNthFreeCell(i);
      EXPECT_FALSE(field.CheckCell(cell));
      EXPECT_GE(cell.first, 0);
      EXPECT_LT(cell.first, FIELD_LENGTH);
      EXPECT_GE(cell.second, 0);
      EXPECT_LT(cell.second, FIELD_WIDTH);
    });
  }
}

TEST_F(FieldTest, GetNthFreeCell_RandomAccess) {
  // Test random access patterns to ensure no segfaults
  const int test_iterations = 1000;

  for (int i = 0; i < test_iterations; ++i) {
    int random_n = rand() % (FIELD_LENGTH * FIELD_WIDTH * 2);
    EXPECT_NO_THROW({
      auto cell = field.GetNthFreeCell(random_n);
      EXPECT_GE(cell.first, 0);
      EXPECT_LT(cell.first, FIELD_LENGTH);
      EXPECT_GE(cell.second, 0);
      EXPECT_LT(cell.second, FIELD_WIDTH);
    });
  }

  // Fill some random cells and test again
  for (int i = 0; i < FIELD_LENGTH * FIELD_WIDTH / 3; ++i) {
    int y = rand() % FIELD_LENGTH;
    int x = rand() % FIELD_WIDTH;
    field.FillCell({y, x});
  }

  for (int i = 0; i < test_iterations / 10; ++i) {
    int random_n = rand() % (FIELD_LENGTH * FIELD_WIDTH * 2);
    EXPECT_NO_THROW({
      auto cell = field.GetNthFreeCell(random_n);
      EXPECT_GE(cell.first, 0);
      EXPECT_LT(cell.first, FIELD_LENGTH);
      EXPECT_GE(cell.second, 0);
      EXPECT_LT(cell.second, FIELD_WIDTH);
    });
  }
}
