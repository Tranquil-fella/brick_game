#include "snake_model.h"

#include <gtest/gtest.h>

#include <atomic>
#include <thread>

#include "backend.h"

using namespace brick_game;

class BackendTest : public ::testing::Test {
 protected:
  void SetUp() override {
    // Reset any global state before each test
    userInput(Terminate, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  void TearDown() override {
    userInput(Terminate, false);
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }
};

// Test basic backend C interface
TEST_F(BackendTest, UserInputAllActions) {
  // Test all possible user actions without crashing
  EXPECT_NO_THROW(userInput(Start, false));
  EXPECT_NO_THROW(userInput(Pause, false));
  EXPECT_NO_THROW(userInput(Terminate, false));
  EXPECT_NO_THROW(userInput(Left, false));
  EXPECT_NO_THROW(userInput(Right, false));
  EXPECT_NO_THROW(userInput(Up, false));
  EXPECT_NO_THROW(userInput(Down, false));
  EXPECT_NO_THROW(userInput(Action, false));

  // Test with hold parameter
  EXPECT_NO_THROW(userInput(Left, true));
  EXPECT_NO_THROW(userInput(Right, true));
}

TEST_F(BackendTest, UpdateCurrentStateReturnsValidStructure) {
  GameInfo_t game_info = updateCurrentState();

  // Verify structure fields exist
  EXPECT_NE(game_info.field, nullptr);
  EXPECT_NE(game_info.next, nullptr);

  // Verify field dimensions
  for (int i = 0; i < FIELD_LENGTH; i++) {
    EXPECT_NE(game_info.field[i], nullptr);
  }
  for (int i = 0; i < NEXTF_LENGTH; i++) {
    EXPECT_NE(game_info.next[i], nullptr);
  }

  // Verify score fields are initialized
  EXPECT_GE(game_info.score, 0);
  EXPECT_GE(game_info.high_score, 0);
  EXPECT_GE(game_info.level, 0);
  EXPECT_GE(game_info.speed, 0);
  EXPECT_TRUE(game_info.pause == 0 || game_info.pause == 1);
}

TEST_F(BackendTest, PauseFunctionality) {
  userInput(Start, false);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  GameInfo_t running_state = updateCurrentState();
  EXPECT_EQ(running_state.pause, 0);

  // Pause the game
  userInput(Pause, false);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  GameInfo_t paused_state = updateCurrentState();
  EXPECT_EQ(paused_state.pause, 1);

  // Unpause
  userInput(Pause, false);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  GameInfo_t unpaused_state = updateCurrentState();
  EXPECT_EQ(unpaused_state.pause, 0);
}

TEST_F(BackendTest, RapidInputSequence) {
  // Test rapid input sequence without crashing
  for (int i = 0; i < 100; i++) {
    userInput(Left, false);
    userInput(Right, false);
    userInput(Up, false);
    userInput(Down, false);
  }

  // Should not crash
  GameInfo_t state = updateCurrentState();
  EXPECT_NE(state.field, nullptr);
}

TEST_F(BackendTest, ConcurrentAccess) {
  std::atomic<bool> exception_thrown{false};

  auto input_thread = [&]() {
    try {
      for (int i = 0; i < 50; i++) {
        userInput(Left, false);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    } catch (...) {
      exception_thrown = true;
    }
  };

  auto state_thread = [&]() {
    try {
      for (int i = 0; i < 50; i++) {
        GameInfo_t state = updateCurrentState();
        EXPECT_NE(state.field, nullptr);
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
      }
    } catch (...) {
      exception_thrown = true;
    }
  };

  userInput(Start, false);

  std::thread t1(input_thread);
  std::thread t2(state_thread);

  t1.join();
  t2.join();

  EXPECT_FALSE(exception_thrown);
}

// SnakeModel specific tests
class SnakeModelTest : public ::testing::Test {
 protected:
  void SetUp() override { model_ = std::make_unique<SnakeModel>(); }

  void TearDown() override { model_.reset(); }

  std::unique_ptr<SnakeModel> model_;
};

TEST_F(SnakeModelTest, Construction) {
  ASSERT_NE(model_, nullptr);

  // Should be able to get initial state
  GameInfo_t initial_state = model_->GetCurrentStateCopy();
  EXPECT_NE(initial_state.field, nullptr);
  EXPECT_NE(initial_state.next, nullptr);
}

TEST_F(SnakeModelTest, TakeMoveActionInMovingState) {
  // First put the game in moving state
  model_->TakeGameControlAction(ControlAction::Start);

  // Now movement actions should be processed
  EXPECT_NO_THROW(model_->TakeMoveAction(MovementAction::Left));
  EXPECT_NO_THROW(model_->TakeMoveAction(MovementAction::Right));
  EXPECT_NO_THROW(model_->TakeMoveAction(MovementAction::Up));
  EXPECT_NO_THROW(model_->TakeMoveAction(MovementAction::Down));
  EXPECT_NO_THROW(model_->TakeMoveAction(MovementAction::Action));
}

TEST_F(SnakeModelTest, TakeMoveActionInNonMovingState) {
  // Game starts in Start state, not Moving
  // Movement actions should be ignored but not crash
  EXPECT_NO_THROW(model_->TakeMoveAction(MovementAction::Left));
  EXPECT_NO_THROW(model_->TakeMoveAction(MovementAction::Right));
}

TEST_F(SnakeModelTest, GameControlActions) {
  // Test all control actions
  EXPECT_NO_THROW(model_->TakeGameControlAction(ControlAction::Start));
  EXPECT_NO_THROW(model_->TakeGameControlAction(ControlAction::Pause));
  EXPECT_NO_THROW(model_->TakeGameControlAction(ControlAction::Terminate));

  // Multiple terminates should be safe
  EXPECT_NO_THROW(model_->TakeGameControlAction(ControlAction::Terminate));
  EXPECT_NO_THROW(model_->TakeGameControlAction(ControlAction::Terminate));
}

TEST_F(SnakeModelTest, PauseToggle) {
  // Start the game
  model_->TakeGameControlAction(ControlAction::Start);

  GameInfo_t running_state = model_->GetCurrentStateCopy();
  EXPECT_EQ(running_state.pause, 0);

  // Pause
  model_->TakeGameControlAction(ControlAction::Pause);
  GameInfo_t paused_state = model_->GetCurrentStateCopy();
  EXPECT_EQ(paused_state.pause, 1);

  // Unpause
  model_->TakeGameControlAction(ControlAction::Pause);
  GameInfo_t unpaused_state = model_->GetCurrentStateCopy();
  EXPECT_EQ(unpaused_state.pause, 0);
}

TEST_F(SnakeModelTest, GetCurrentStateCopyMultipleCalls) {
  // Multiple calls should return valid state
  for (int i = 0; i < 10; i++) {
    GameInfo_t state = model_->GetCurrentStateCopy();
    EXPECT_NE(state.field, nullptr);
    EXPECT_NE(state.next, nullptr);
    EXPECT_GE(state.score, 0);
    EXPECT_GE(state.high_score, 0);
    EXPECT_GE(state.level, 0);
    EXPECT_GE(state.speed, 0);
  }
}

TEST_F(SnakeModelTest, RapidStateTransitions) {
  // Rapidly change states without crashing
  for (int i = 0; i < 20; i++) {
    model_->TakeGameControlAction(ControlAction::Start);
    model_->TakeGameControlAction(ControlAction::Pause);
    model_->TakeGameControlAction(ControlAction::Terminate);
  }

  // Should still be able to get state
  GameInfo_t state = model_->GetCurrentStateCopy();
  EXPECT_NE(state.field, nullptr);
}

TEST_F(SnakeModelTest, MovementAfterStateChanges) {
  // Test movement in different states
  model_->TakeGameControlAction(ControlAction::Start);
  model_->TakeMoveAction(MovementAction::Left);

  model_->TakeGameControlAction(ControlAction::Pause);
  model_->TakeMoveAction(MovementAction::Right);  // Should be ignored

  model_->TakeGameControlAction(ControlAction::Pause);  // Unpause
  model_->TakeMoveAction(MovementAction::Up);

  model_->TakeGameControlAction(ControlAction::Terminate);
  model_->TakeMoveAction(MovementAction::Down);  // Should be ignored

  // Should not crash
  SUCCEED();
}

// Integration test between backend and SnakeModel
TEST_F(BackendTest, BackendIntegration) {
  // Test that backend functions work together
  userInput(Start, false);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  GameInfo_t state1 = updateCurrentState();

  userInput(Left, false);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  GameInfo_t state2 = updateCurrentState();

  userInput(Pause, false);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  GameInfo_t state3 = updateCurrentState();
  EXPECT_EQ(state3.pause, 1);

  userInput(Terminate, false);
  std::this_thread::sleep_for(std::chrono::milliseconds(50));

  GameInfo_t state4 = updateCurrentState();

  // All states should be valid
  EXPECT_NE(state1.field, nullptr);
  EXPECT_NE(state2.field, nullptr);
  EXPECT_NE(state3.field, nullptr);
  EXPECT_NE(state4.field, nullptr);
}

TEST_F(BackendTest, HoldParameterFunctionality) {
  // Test hold parameter behavior
  // Note: Actual behavior depends on implementation
  EXPECT_NO_THROW(userInput(Left, true));
  EXPECT_NO_THROW(userInput(Right, true));
  EXPECT_NO_THROW(userInput(Up, true));
  EXPECT_NO_THROW(userInput(Down, true));

  // Should not crash with hold = true
  SUCCEED();
}

TEST_F(BackendTest, MemoryManagement) {
  // Test that repeated state updates don't leak memory
  for (int i = 0; i < 1000; i++) {
    GameInfo_t state = updateCurrentState();
    EXPECT_NE(state.field, nullptr);
    EXPECT_NE(state.next, nullptr);

    // If there's a cleanup function, it would be called here
    // For now, we're just testing we don't crash
  }
}
