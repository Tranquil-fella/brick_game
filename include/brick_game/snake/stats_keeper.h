#ifndef STATS_KEEPER_H
#define STATS_KEEPER_H

#include <concepts>

#include "backend.h"
#include "mediator.h"

namespace brick_game {
template <typename DataStorage>
concept IsDataStorage = requires(DataStorage ds) {
  { ds.ReadHighscore() } -> std::same_as<int>;
  ds.WriteHighscore(int{});
};

constexpr int kScoreStep = 1;
constexpr int kLevelTreshold = 5;
constexpr int kMaxLevel = 10;

template <IsDataStorage DataStorage>
struct StatsKeeper : public Component, public DataStorage {
 public:
  StatsKeeper(std::shared_ptr<Mediator> m)
      : Component(std::move(m)), highscore_{this->ReadHighscore()} {}

  StatsKeeper(StatsKeeper&& o)
      : Component(std::move(o)), DataStorage(std::move(o)) {
    UpdateHighscore();
    std::swap(score_, o.score_);
    highscore_ = o.highscore_;
  }
  StatsKeeper& operator=(StatsKeeper&& o) {
    Component::operator=(std::move(o));
    UpdateHighscore();
    std::swap(score_, o.score_);
    o.highscore_ = std::max(highscore_, o.highscore_);
    return *this;
  }

  StatsKeeper(const StatsKeeper&) = delete;

  ~StatsKeeper() override {
    UpdateHighscore();
    SaveHighscore();
  }

  int GetScore() { return score_; }
  int GetHighscore() { return highscore_; }

  void PlaceStats(GameInfo_t& info, bool gameover) {
    info.score = score_;
    info.high_score = highscore_;
    if (gameover) {
      info.speed = info.level = 0;
    } else {
      info.speed = info.level = level_;
    }
  }

  void ProcessEvent(Event) override { IncreaseScore(); }

 private:
  int score_{};
  int level_ = 1;
  int highscore_;

  void UpdateHighscore() {
    if (highscore_ < score_) highscore_ = score_;
  }
  void SaveHighscore() {
    if (highscore_ > this->ReadHighscore()) this->WriteHighscore(highscore_);
  }
  void IncreaseScore() {
    score_ += kScoreStep;
    if (score_ % kLevelTreshold == 0) {
      if (IncreaseLevel()) this->mediator_->Notify(Event::NewLevel);
    }
  }
  bool IncreaseLevel() {
    if (level_ < kMaxLevel) {
      ++level_;
      return true;
    }
    return false;
  }
};

};  // namespace brick_game
#endif