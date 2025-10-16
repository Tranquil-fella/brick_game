#ifndef SNAKE_MODEL_H
#define SNAKE_MODEL_H

#include <utility>

#include "backend.h"
#include "fsm.h"
#include "mediator.h"
#include "move_timer.h"
#include "observable.h"
#include "simple_file_storage.h"
#include "snake.h"
#include "stats_keeper.h"

namespace brick_game {

struct SnakeMediator : public Observer<SnakeFSM>, public Mediator {
  ~SnakeMediator() override = default;
  void Notify(Event e) override {
    if (e == Event::TimeToMove && state_ != State::Moving) return;
    this->NotifySubs(e);
  }
  void Update(SnakeFSM* observable) override {
    state_ = observable->GetState();
  }
  std::weak_ptr<Observer<SnakeFSM>> GetObserverPtr() {
    return this->weak_from_this();
  }

 private:
  State state_;
};

struct SnakeModel {
  SnakeModel();
  void TakeMoveAction(MovementAction a);
  void TakeGameControlAction(ControlAction a);
  GameInfo_t GetCurrentStateCopy();

 private:
  std::shared_ptr<SnakeMediator> mediator_;
  std::shared_ptr<SnakeFSM> fsm_;
  std::shared_ptr<Snake> snake_;
  std::shared_ptr<StatsKeeper<SimpleFileStorage>> stats_keeper_;
  std::shared_ptr<MoveTimer> move_timer_;
  void Connect();
  void Reset();
};
};  // namespace brick_game
#endif