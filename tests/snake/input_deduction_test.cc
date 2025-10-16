#include <gtest/gtest.h>

#include <variant>

#include "input_mapping.h"

using namespace brick_game;

class InputDeductionTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  void TearDown() override {}
  using Movement = MovementAction;
  using Command = ControlAction;
};

// Test valid Movement inputs
TEST_F(InputDeductionTest, DeducesLeftMovement) {
  auto result = MapUserInput(Left);
  EXPECT_TRUE(std::holds_alternative<Movement>(result));
  EXPECT_EQ(std::get<Movement>(result), Movement::Left);
}

TEST_F(InputDeductionTest, DeducesRightMovement) {
  auto result = MapUserInput(Right);
  EXPECT_TRUE(std::holds_alternative<Movement>(result));
  EXPECT_EQ(std::get<Movement>(result), Movement::Right);
}

TEST_F(InputDeductionTest, DeducesUpMovement) {
  auto result = MapUserInput(Up);
  EXPECT_TRUE(std::holds_alternative<Movement>(result));
  EXPECT_EQ(std::get<Movement>(result), Movement::Up);
}

TEST_F(InputDeductionTest, DeducesDownMovement) {
  auto result = MapUserInput(Down);
  EXPECT_TRUE(std::holds_alternative<Movement>(result));
  EXPECT_EQ(std::get<Movement>(result), Movement::Down);
}

TEST_F(InputDeductionTest, DeducesActionMovement) {
  auto result = MapUserInput(Action);
  EXPECT_TRUE(std::holds_alternative<Movement>(result));
  EXPECT_EQ(std::get<Movement>(result), Movement::Action);
}

// Test valid Command inputs
TEST_F(InputDeductionTest, DeducesStartCommand) {
  auto result = MapUserInput(Start);
  EXPECT_TRUE(std::holds_alternative<Command>(result));
  EXPECT_EQ(std::get<Command>(result), Command::Start);
}

TEST_F(InputDeductionTest, DeducesPauseCommand) {
  auto result = MapUserInput(Pause);
  EXPECT_TRUE(std::holds_alternative<Command>(result));
  EXPECT_EQ(std::get<Command>(result), Command::Pause);
}

TEST_F(InputDeductionTest, DeducesTerminateCommand) {
  auto result = MapUserInput(Terminate);
  EXPECT_TRUE(std::holds_alternative<Command>(result));
  EXPECT_EQ(std::get<Command>(result), Command::Terminate);
}

// Test edge cases and invalid inputs
TEST_F(InputDeductionTest, ThrowsOnInvalidLowValue) {
  UserAction_t invalidAction = static_cast<UserAction_t>(-1);
  EXPECT_THROW(MapUserInput(invalidAction), std::invalid_argument);
}

TEST_F(InputDeductionTest, ThrowsOnInvalidHighValue) {
  UserAction_t invalidAction = static_cast<UserAction_t>(Action + 1);
  EXPECT_THROW(MapUserInput(invalidAction), std::invalid_argument);
}

// Test that the conversion logic is correct
TEST_F(InputDeductionTest, MovementConversionIsCorrect) {
  // Test that Left (value 3) becomes Movement::Left (value 0)
  auto leftResult = MapUserInput(Left);
  EXPECT_EQ(static_cast<int>(std::get<Movement>(leftResult)), 0);

  // Test that Right (value 4) becomes Movement::Right (value 1)
  auto rightResult = MapUserInput(Right);
  EXPECT_EQ(static_cast<int>(std::get<Movement>(rightResult)), 1);

  // Test that Up (value 5) becomes Movement::Up (value 2)
  auto upResult = MapUserInput(Up);
  EXPECT_EQ(static_cast<int>(std::get<Movement>(upResult)), 2);

  // Test that Down (value 6) becomes Movement::Down (value 3)
  auto downResult = MapUserInput(Down);
  EXPECT_EQ(static_cast<int>(std::get<Movement>(downResult)), 3);

  // Test that Action (value 7) becomes Movement::Action (value 4)
  auto actionResult = MapUserInput(Action);
  EXPECT_EQ(static_cast<int>(std::get<Movement>(actionResult)), 4);
}

// Test that Command values are preserved
TEST_F(InputDeductionTest, CommandConversionIsCorrect) {
  // Test that Start (value 0) becomes Command::Start (value 0)
  auto startResult = MapUserInput(Start);
  EXPECT_EQ(static_cast<int>(std::get<Command>(startResult)), 0);

  // Test that Pause (value 1) becomes Command::Pause (value 1)
  auto pauseResult = MapUserInput(Pause);
  EXPECT_EQ(static_cast<int>(std::get<Command>(pauseResult)), 1);

  // Test that Terminate (value 2) becomes Command::Terminate (value 2)
  auto terminateResult = MapUserInput(Terminate);
  EXPECT_EQ(static_cast<int>(std::get<Command>(terminateResult)), 2);
}

// Test type safety - ensure we can't accidentally get wrong type
TEST_F(InputDeductionTest, TypeSafety) {
  auto movementResult = MapUserInput(Left);
  EXPECT_FALSE(std::holds_alternative<Command>(movementResult));

  auto commandResult = MapUserInput(Start);
  EXPECT_FALSE(std::holds_alternative<Movement>(commandResult));
}
