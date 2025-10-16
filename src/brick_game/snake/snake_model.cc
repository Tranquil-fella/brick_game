#include "snake_model.h"

brick_game::SnakeModel::SnakeModel()
    : mediator_(std::make_shared<SnakeMediator>()),
      fsm_(std::make_shared<SnakeFSM>(mediator_)),
      snake_(std::make_shared<Snake>(mediator_)),
      stats_keeper_(
          std::make_shared<StatsKeeper<SimpleFileStorage>>(mediator_)),
      move_timer_(std::make_shared<MoveTimer>(mediator_)) {
  Connect();
}

void brick_game::SnakeModel::TakeMoveAction(MovementAction a) {
  if (fsm_->IsState(State::Moving)) {
    snake_->Move(a);
  }
}
void brick_game::SnakeModel::TakeGameControlAction(ControlAction a) {
  if (fsm_->IsCorrectStateForExecution(a)) {
    switch (a) {
      case ControlAction::Start:
        fsm_->SetState(State::Moving);
        break;
      case ControlAction::Pause:
        if (fsm_->IsState(State::Pause))
          fsm_->SetState(State::Moving);
        else
          fsm_->SetState(State::Pause);
        break;
      case ControlAction::Terminate:
        Reset();
        break;
      default:
        break;
    }
  }
}
GameInfo_t brick_game::SnakeModel::GetCurrentStateCopy() {
  GameInfo_t info{};
  snake_->PlaceGameInfo(info, fsm_->IsState(State::Gameover));
  stats_keeper_->PlaceStats(info, fsm_->IsState(State::Gameover));
  if (fsm_->IsState(State::Pause)) info.pause = 1;
  return info;
}

void brick_game::SnakeModel::Connect() {
  fsm_->AddObserver(mediator_->GetObserverPtr());
  fsm_->SetState(State::Start);
  mediator_->AddSubscriber(snake_->weak_from_this(), Event::TimeToMove);
  mediator_->AddSubscriber(move_timer_->weak_from_this(), Event::NewLevel);
  mediator_->AddSubscriber(move_timer_->weak_from_this(), Event::PlayerMoved);
  mediator_->AddSubscriber(move_timer_->weak_from_this(), Event::Paused);
  mediator_->AddSubscriber(move_timer_->weak_from_this(), Event::Unpaused);
  mediator_->AddSubscriber(stats_keeper_->weak_from_this(), Event::ScorePoint);
  mediator_->AddSubscriber(fsm_->weak_from_this(), Event::GameOver);
}

void brick_game::SnakeModel::Reset() {
  fsm_->SetState(State::Start);
  *snake_ = Snake(mediator_);
  *stats_keeper_ = StatsKeeper<SimpleFileStorage>(mediator_);
  *move_timer_ = MoveTimer(mediator_);
}