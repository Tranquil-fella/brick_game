#ifndef SNAKE_H
#define SNAKE_H
#include <concepts>
#include <initializer_list>
#include <mutex>
#include <queue>

#include "field.h"
#include "input_mapping.h"
#include "mediator.h"

namespace brick_game {

struct Direction {
  Direction(MovementAction v = MovementAction::Up) : direction_(v) {}

  constexpr bool IsValidDirection(MovementAction new_direction) const {
    return MovementAction::Action == new_direction ||
           kInvalidDirections[static_cast<int>(direction_)] != new_direction;
  }

  void operator=(MovementAction m) {
    if (m != MovementAction::Action) direction_ = m;
  }
  explicit operator MovementAction() { return direction_; }

 private:
  MovementAction direction_;
  static constexpr MovementAction kInvalidDirections[4] = {
      MovementAction::Right, MovementAction::Left, MovementAction::Down,
      MovementAction::Up};
};

struct Snake : public Component {
  Snake(std::shared_ptr<Mediator> m);
  Snake(const Snake&) = delete;
  Snake(Snake&& o) noexcept;

  Snake& operator=(Snake&& o) noexcept;
  ~Snake() override = default;

  void Move(MovementAction new_direction, bool player_command = true);
  void PlaceGameInfo(GameInfo_t& gi, bool gameover);
  void ProcessEvent(Event) override;

 private:
  using SnakeBody = std::queue<Cell>;
  std::mutex mtx_{};
  Cell apple_;
  bool got_apple_{};
  Field field_{};
  SnakeBody snake_body_;
  Direction direction_{};

  void InitializeSnake();

  void AddSnakeSeg(const Cell& seg);

  void AddSnakeSeg(std::initializer_list<Cell> list);

  void RemoveTailSeg();

  Cell GetNextCell();

  void SpawnApple();

  int GetRandomFreeCellNum();

  bool IsCollision();
};

};  // namespace brick_game

#endif