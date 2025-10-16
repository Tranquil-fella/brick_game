#include "stats_keeper.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>

#include "backend.h"

using namespace brick_game;

template <int I = 0>
class MockDataStorage {
 public:
  MockDataStorage() = default;
  ~MockDataStorage() = default;

  // Manual mock implementation
  int ReadHighscore() { return hightscore; }

  void WriteHighscore(int score) {
    write_call_count++;
    last_written_score = score;
  }

  int GetWriteCallCount() const { return write_call_count; }
  int GetLastWrittenScore() const { return last_written_score; }
  void ResetCallCount() { write_call_count = 0; }

 private:
  int hightscore = I;
  int write_call_count = 0;
  int last_written_score = 0;
};

// Test Mediator
class TestMediator : public Mediator {
 public:
  std::vector<Event> notifications;

  void Notify(Event e) override {
    notifications.push_back(e);
    Mediator::Notify(e);
  }
};

class StatsKeeperTest : public ::testing::Test {
 protected:
  void SetUp() override { test_mediator_ = std::make_shared<TestMediator>(); }

  void TearDown() override { test_mediator_.reset(); }

  std::shared_ptr<TestMediator> test_mediator_;
};

// Test construction and basic functionality
TEST_F(StatsKeeperTest, ConstructorInitializesCorrectly) {
  MockDataStorage mock_storage;

  StatsKeeper<MockDataStorage<100>> stats(test_mediator_);

  EXPECT_EQ(stats.GetScore(), 0);
  EXPECT_EQ(stats.GetHighscore(), 100);
}

TEST_F(StatsKeeperTest, ProcessEventIncreasesScore) {
  StatsKeeper<MockDataStorage<0>> stats(test_mediator_);

  int initial_score = stats.GetScore();
  stats.ProcessEvent(Event::ScorePoint);

  EXPECT_EQ(stats.GetScore(), initial_score + kScoreStep);
}

TEST_F(StatsKeeperTest, LevelIncreasesAtThreshold) {
  StatsKeeper<MockDataStorage<0>> stats(test_mediator_);

  // Process events to reach level threshold
  for (int i = 0; i < kLevelTreshold; ++i) {
    stats.ProcessEvent(Event::ScorePoint);
  }

  // Should notify NewLevel and level should increase
  EXPECT_FALSE(test_mediator_->notifications.empty());
  EXPECT_EQ(test_mediator_->notifications.back(), Event::NewLevel);
}

TEST_F(StatsKeeperTest, LevelDoesNotExceedMax) {
  StatsKeeper<MockDataStorage<0>> stats(test_mediator_);

  // Process enough events to reach max level and beyond
  int events_needed = kMaxLevel * kLevelTreshold + 10;
  for (int i = 0; i < events_needed; ++i) {
    stats.ProcessEvent(Event::ScorePoint);
  }

  // Count NewLevel notifications (should be kMaxLevel - 1)
  int new_level_count = 0;
  for (auto event : test_mediator_->notifications) {
    if (event == Event::NewLevel) {
      new_level_count++;
    }
  }

  EXPECT_EQ(new_level_count, kMaxLevel - 1);
}

TEST_F(StatsKeeperTest, PlaceStatsWithoutGameOver) {
  StatsKeeper<MockDataStorage<500>> stats(test_mediator_);

  // Increase score to get to level 2
  for (int i = 0; i < kLevelTreshold; ++i) {
    stats.ProcessEvent(Event::ScorePoint);
  }

  GameInfo_t game_info{};
  stats.PlaceStats(game_info, false);

  EXPECT_EQ(game_info.score, kLevelTreshold);
  EXPECT_EQ(game_info.high_score, 500);
  EXPECT_EQ(game_info.level, 2);
  EXPECT_EQ(game_info.speed, 2);
  EXPECT_EQ(game_info.pause, 0);  // Should be unchanged
}

TEST_F(StatsKeeperTest, PlaceStatsWithGameOver) {
  StatsKeeper<MockDataStorage<1000>> stats(test_mediator_);

  // Increase score and level
  for (int i = 0; i < kLevelTreshold * 3; ++i) {
    stats.ProcessEvent(Event::ScorePoint);
  }

  GameInfo_t game_info;
  game_info.pause = 1;  // Set some value to ensure it's not changed
  stats.PlaceStats(game_info, true);

  EXPECT_EQ(game_info.score, kLevelTreshold * 3);
  EXPECT_EQ(game_info.high_score, 1000);
  EXPECT_EQ(game_info.level, 0);  // Should be 0 on gameover
  EXPECT_EQ(game_info.speed, 0);  // Should be 0 on gameover
  EXPECT_EQ(game_info.pause, 1);  // Should remain unchanged
}

TEST_F(StatsKeeperTest, HighscoreNotUpdatedWhenScoreLower) {
  StatsKeeper<MockDataStorage<1000>> stats(test_mediator_);

  // Process events but don't exceed highscore
  for (int i = 0; i < 50; ++i) {
    stats.ProcessEvent(Event::ScorePoint);
  }

  EXPECT_EQ(stats.GetHighscore(), 1000);
}

TEST_F(StatsKeeperTest, MultipleScoreEvents) {
  StatsKeeper<MockDataStorage<0>> stats(test_mediator_);

  const int num_events = 25;
  for (int i = 0; i < num_events; ++i) {
    stats.ProcessEvent(Event::ScorePoint);
  }

  EXPECT_EQ(stats.GetScore(), num_events * kScoreStep);
}

TEST_F(StatsKeeperTest, LevelCalculation) {
  StatsKeeper<MockDataStorage<0>> stats(test_mediator_);

  // Test level progression
  GameInfo_t game_info;

  // Level 1: 0-4 points
  for (int i = 0; i < kLevelTreshold; ++i) {
    stats.ProcessEvent(Event::ScorePoint);
  }
  stats.PlaceStats(game_info, false);
  EXPECT_EQ(game_info.level, 2);

  // Level 2: 5-9 points
  for (int i = 0; i < kLevelTreshold; ++i) {
    stats.ProcessEvent(Event::ScorePoint);
  }
  stats.PlaceStats(game_info, false);
  EXPECT_EQ(game_info.level, 3);
}

TEST_F(StatsKeeperTest, NoNewLevelNotificationWhenAtMaxLevel) {
  StatsKeeper<MockDataStorage<0>> stats(test_mediator_);

  // Reach max level
  int events_to_max_level = kMaxLevel * kLevelTreshold;
  for (int i = 0; i < events_to_max_level; ++i) {
    stats.ProcessEvent(Event::ScorePoint);
  }

  test_mediator_->notifications.clear();

  // Process more events - should not trigger NewLevel
  for (int i = 0; i < kLevelTreshold; ++i) {
    stats.ProcessEvent(Event::ScorePoint);
  }

  bool found_new_level = false;
  for (auto event : test_mediator_->notifications) {
    if (event == Event::NewLevel) {
      found_new_level = true;
      break;
    }
  }

  EXPECT_FALSE(found_new_level);
}

TEST_F(StatsKeeperTest, ConstantsValues) {
  // Test that constants have expected values
  EXPECT_EQ(kScoreStep, 1);
  EXPECT_EQ(kLevelTreshold, 5);
  EXPECT_EQ(kMaxLevel, 10);
}

// Test with a real file-based storage for integration testing
class FileDataStorage {
 public:
  FileDataStorage() : filename_("test_highscore.txt") {}

  int ReadHighscore() {
    std::ifstream file(filename_);
    int highscore = 0;
    if (file.is_open()) {
      file >> highscore;
    }
    return highscore;
  }

  void WriteHighscore(int score) {
    std::ofstream file(filename_);
    if (file.is_open()) {
      file << score;
    }
  }

  void Cleanup() { std::remove(filename_.c_str()); }

 private:
  std::string filename_;
};

TEST_F(StatsKeeperTest, IntegrationWithFileStorage) {
  FileDataStorage file_storage;
  file_storage.Cleanup();  // Ensure clean start

  {
    StatsKeeper<FileDataStorage> stats(test_mediator_);

    // Add some score
    for (int i = 0; i < 10; ++i) {
      stats.ProcessEvent(Event::ScorePoint);
    }

    // Highscore should be saved on destruction
  }

  // Verify highscore was saved
  FileDataStorage verifier;
  int saved_highscore = verifier.ReadHighscore();
  EXPECT_EQ(saved_highscore, 10);

  file_storage.Cleanup();  // Clean up test file
}

// Test move semantics with manual verification
TEST_F(StatsKeeperTest, MoveSemantics) {
  StatsKeeper<MockDataStorage<200>> stats(test_mediator_);
  // Add some score
  for (int i = 0; i < 50; ++i) {
    stats.ProcessEvent(Event::ScorePoint);
  }
  EXPECT_EQ(stats.GetScore(), 50);
  EXPECT_EQ(stats.GetHighscore(), 200);
  // Move construct
  StatsKeeper<MockDataStorage<200>> moved_stats(std::move(stats));

  EXPECT_EQ(moved_stats.GetScore(), 50);
  EXPECT_EQ(moved_stats.GetHighscore(), 200);
}
