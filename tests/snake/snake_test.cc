#include "snake.h"

#include <gtest/gtest.h>

#include <memory>
#include <thread>

using namespace s21;

// Simple test mediator that tracks notifications
class TestMediator : public Mediator {
 public:
  std::vector<Event> notifications;

  void Notify(Event e) override {
    notifications.push_back(e);
    Mediator::Notify(e);
  }
};

class SnakeTest : public ::testing::Test {
 protected:
  void SetUp() override {
    test_mediator_ = std::make_shared<TestMediator>();
    snake_ = std::make_unique<Snake>(test_mediator_);
  }

  void TearDown() override {
    snake_.reset();
    test_mediator_.reset();
  }

  std::shared_ptr<TestMediator> test_mediator_;
  std::unique_ptr<Snake> snake_;
};

// Test basic construction and initialization
TEST_F(SnakeTest, ConstructorInitializesSnakeCorrectly) {
  ASSERT_NE(snake_, nullptr);

  GameInfo_t game_info;
  snake_->PlaceGameInfo(game_info, false);

  // Should be able to place game info without crashing
  SUCCEED();
}

TEST_F(SnakeTest, MoveValidDirection) {
  // Initial direction is Up, so moving Up should work and notify PlayerMoved
  snake_->Move(MovementAction::Up);

  // Should notify PlayerMoved
  EXPECT_FALSE(test_mediator_->notifications.empty());
  EXPECT_EQ(test_mediator_->notifications.back(), Event::PlayerMoved);
}

TEST_F(SnakeTest, MoveInvalidOppositeDirection) {
  // Initial direction is Up, moving Down should be ignored
  test_mediator_->notifications.clear();
  snake_->Move(MovementAction::Down);

  // Should not notify anything since move was invalid
  EXPECT_TRUE(test_mediator_->notifications.empty());
}

TEST_F(SnakeTest, MoveValidLateralDirection) {
  // Moving left or right from initial Up direction should work
  test_mediator_->notifications.clear();
  snake_->Move(MovementAction::Left);

  EXPECT_FALSE(test_mediator_->notifications.empty());
  EXPECT_EQ(test_mediator_->notifications.back(), Event::PlayerMoved);
}

TEST_F(SnakeTest, MoveWithoutPlayerCommand) {
  test_mediator_->notifications.clear();
  snake_->Move(MovementAction::Up, false);

  // Should move but not notify PlayerMoved
  bool found_player_moved = false;
  for (auto event : test_mediator_->notifications) {
    if (event == Event::PlayerMoved) {
      found_player_moved = true;
      break;
    }
  }
  EXPECT_FALSE(found_player_moved);
}

TEST_F(SnakeTest, ProcessEventCausesMovement) {
  test_mediator_->notifications.clear();

  // ProcessEvent should trigger automatic movement
  snake_->ProcessEvent(Event::TimeToMove);

  // Should move but not notify PlayerMoved (since it's automatic)
  bool found_player_moved = false;
  for (auto event : test_mediator_->notifications) {
    if (event == Event::PlayerMoved) {
      found_player_moved = true;
      break;
    }
  }
  EXPECT_FALSE(found_player_moved);
}

TEST_F(SnakeTest, DirectionValidityCheck) {
  Direction dir(MovementAction::Up);

  // Valid directions from Up
  EXPECT_TRUE(dir.IsValidDirection(MovementAction::Left));
  EXPECT_TRUE(dir.IsValidDirection(MovementAction::Right));
  EXPECT_TRUE(dir.IsValidDirection(MovementAction::Up));

  // Invalid opposite direction
  EXPECT_FALSE(dir.IsValidDirection(MovementAction::Down));

  // Action should always be valid
  EXPECT_TRUE(dir.IsValidDirection(MovementAction::Action));
}

TEST_F(SnakeTest, DirectionAssignmentAndConversion) {
  Direction dir;

  // Test assignment
  dir = MovementAction::Left;
  EXPECT_EQ(static_cast<MovementAction>(dir), MovementAction::Left);

  // Test that Action assignment doesn't change direction
  Direction dir_up(MovementAction::Up);
  dir_up = MovementAction::Action;
  EXPECT_EQ(static_cast<MovementAction>(dir_up), MovementAction::Up);
}

TEST_F(SnakeTest, MultipleConsecutiveMoves) {
  test_mediator_->notifications.clear();

  // Test multiple moves in sequence
  snake_->Move(MovementAction::Left);
  snake_->Move(MovementAction::Up);
  snake_->Move(MovementAction::Right);

  // Should have 3 PlayerMoved notifications
  int player_moved_count = 0;
  for (auto event : test_mediator_->notifications) {
    if (event == Event::PlayerMoved) {
      player_moved_count++;
    }
  }
  EXPECT_EQ(player_moved_count, 3);
}

TEST_F(SnakeTest, PlaceGameInfoWithGameOver) {
  GameInfo_t game_info;

  // Should not crash when placing game info with gameover flag
  EXPECT_NO_THROW({ snake_->PlaceGameInfo(game_info, true); });

  EXPECT_NO_THROW({ snake_->PlaceGameInfo(game_info, false); });
}

TEST_F(SnakeTest, MoveOperationsThreadSafe) {
  // Test that multiple threads can call Move safely
  std::atomic<bool> exception_thrown{false};

  auto move_thread = [&]() {
    try {
      for (int i = 0; i < 100; ++i) {
        snake_->Move(MovementAction::Up);
      }
    } catch (...) {
      exception_thrown = true;
    }
  };

  std::thread t1(move_thread);
  std::thread t2(move_thread);

  t1.join();
  t2.join();

  EXPECT_FALSE(exception_thrown);
}

TEST_F(SnakeTest, PlaceGameInfoThreadSafe) {
  // Test that PlaceGameInfo can be called concurrently with Move
  GameInfo_t game_info;
  std::atomic<bool> exception_thrown{false};

  auto move_thread = [&]() {
    try {
      for (int i = 0; i < 50; ++i) {
        snake_->Move(MovementAction::Up);
      }
    } catch (...) {
      exception_thrown = true;
    }
  };

  auto info_thread = [&]() {
    try {
      for (int i = 0; i < 50; ++i) {
        snake_->PlaceGameInfo(game_info, false);
      }
    } catch (...) {
      exception_thrown = true;
    }
  };

  std::thread t1(move_thread);
  std::thread t2(info_thread);

  t1.join();
  t2.join();

  EXPECT_FALSE(exception_thrown);
}

TEST_F(SnakeTest, MoveConstructor) {
  // Test move constructor
  Snake moved_snake(std::move(*snake_));

  // Should be able to use moved object
  GameInfo_t game_info;
  EXPECT_NO_THROW({ moved_snake.PlaceGameInfo(game_info, false); });
}

TEST_F(SnakeTest, MoveAssignment) {
  auto another_snake = std::make_unique<Snake>(test_mediator_);

  // Test move assignment
  *another_snake = std::move(*snake_);

  // Should be able to use moved object
  GameInfo_t game_info;
  EXPECT_NO_THROW({ another_snake->PlaceGameInfo(game_info, false); });
}

TEST_F(SnakeTest, RapidDirectionChanges) {
  test_mediator_->notifications.clear();

  // Rapid fire direction changes
  for (int i = 0; i < 10; ++i) {
    snake_->Move(MovementAction::Left);
    snake_->Move(MovementAction::Right);
    snake_->Move(MovementAction::Up);
  }

  // Should not crash and should have some successful moves
  EXPECT_FALSE(test_mediator_->notifications.empty());
}

TEST_F(SnakeTest, PerformanceUnderLoad) {
  auto start = std::chrono::high_resolution_clock::now();

  // Perform many moves quickly
  for (int i = 0; i < 1000; ++i) {
    snake_->Move(MovementAction::Up);
  }

  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

  // Should complete within reasonable time
  EXPECT_LT(duration.count(), 1000);
}

// Test all possible movement actions
TEST_F(SnakeTest, AllMovementActions) {
  std::vector<MovementAction> actions = {
      MovementAction::Up, MovementAction::Down, MovementAction::Left,
      MovementAction::Right, MovementAction::Action};

  for (auto action : actions) {
    EXPECT_NO_THROW({ snake_->Move(action); });
  }
}
