
#include "snake.h"

#include <random>
namespace s21 {

Snake::Snake(std::shared_ptr<Mediator> m) : Component::Component(m) {
  InitializeSnake();
  SpawnApple();
}

Snake::Snake(Snake&& o) noexcept
    : Component(std::move(o)),
      apple_(std::move(o.apple_)),
      got_apple_(o.got_apple_),
      field_(std::move(o.field_)),
      snake_body_(std::move(o.snake_body_)),
      direction_(std::move(o.direction_)) {}

Snake& Snake::operator=(Snake&& o) noexcept {
  if (this != &o) {
    Component::operator=(std::move(o));
    apple_ = std::move(o.apple_);
    got_apple_ = o.got_apple_;
    field_ = std::move(o.field_);
    snake_body_ = std::move(o.snake_body_);
    direction_ = std::move(o.direction_);
  }
  return *this;
}

void Snake::InitializeSnake() {
  int start_y = FIELD_LENGTH / 2;
  int start_x = FIELD_WIDTH / 2;

  AddSnakeSeg({{start_y + 2, start_x},
               {start_y + 1, start_x},
               {start_y, start_x},
               {start_y - 1, start_x}});
}

void Snake::SpawnApple() {
  apple_ = field_.GetNthFreeCell(GetRandomFreeCellNum());
}

int Snake::GetRandomFreeCellNum() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  const size_t free_cells = field_.kTotalCells - snake_body_.size();
  if (!free_cells) return 0;
  std::uniform_int_distribution<int> dist(0, static_cast<int>(free_cells) - 1);
  return dist(gen);
}

Cell Snake::GetNextCell() {
  Cell offset{0, 0};
  switch (static_cast<MovementAction>(direction_)) {
    case MovementAction::Left:
      offset = {0, -1};
      break;
    case MovementAction::Right:
      offset = {0, 1};
      break;
    case MovementAction::Up:
      offset = {-1, 0};
      break;
    case MovementAction::Down:
      offset = {1, 0};
      break;
    default:
      break;
  }

  return offset + snake_body_.back();
}

void Snake::Move(MovementAction new_direction, bool palyer_action) {
  std::scoped_lock<std::mutex> lock(mtx_);
  if (direction_.IsValidDirection(new_direction)) {
    direction_ = new_direction;
    Cell next = GetNextCell();
    snake_body_.push(next);
    if (IsCollision()) {
      this->mediator_->Notify(Event::GameOver);
      return;
    }
    field_.FillCell(next);
    if (!got_apple_) {
      RemoveTailSeg();
    } else {
      got_apple_ = false;
    }
    if (next == apple_) {
      got_apple_ = true;
      SpawnApple();
      this->mediator_->Notify(Event::ScorePoint);
    }
    if (palyer_action) this->mediator_->Notify(Event::PlayerMoved);
  }
}

bool Snake::IsCollision() {
  const auto [y, x] = snake_body_.back();
  if (y >= FIELD_LENGTH || y < 0 || x >= FIELD_WIDTH || x < 0) return true;
  if (field_.CheckCell(snake_body_.back())) return true;
  return false;
}

void Snake::AddSnakeSeg(const Cell& seg) {
  snake_body_.push(seg);
  field_.FillCell(seg);
}

void Snake::AddSnakeSeg(std::initializer_list<Cell> list) {
  for (auto cell : list) AddSnakeSeg(cell);
}
void Snake::RemoveTailSeg() {
  field_.EmptyCell(snake_body_.front());
  snake_body_.pop();
}

void Snake::PlaceGameInfo(GameInfo_t& gi, bool gameover) {
  std::scoped_lock<std::mutex> lock(mtx_);
  field_.PlaceFieldAndNext(gi, gameover);
  if (!gameover) field_.PlaceApple(apple_);
}

void Snake::ProcessEvent(Event) { Move(MovementAction::Action, false); }

};  // namespace s21
