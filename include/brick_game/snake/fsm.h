#ifndef FSM_H
#define FSM_H
#include <array>
#include <atomic>

#include "input_mapping.h"
#include "mediator.h"
#include "observable.h"

namespace s21 {

enum class State { Start, Pause, Gameover, Moving };

struct SnakeFSM : public Observable<SnakeFSM>, public Component {
  SnakeFSM(std::shared_ptr<Mediator> m) : Component::Component(m) {}
  ~SnakeFSM() override = default;
  SnakeFSM(const SnakeFSM&) = delete;
  SnakeFSM(SnakeFSM&& o)
      : Component::Component(std::move(o)), state_(o.state_.load()) {}
  SnakeFSM& operator=(SnakeFSM&& o) {
    Component::operator=(std::move(o));
    state_ = o.state_.load();
    return *this;
  }

  void ProcessEvent(Event) override { SetState(State::Gameover); }
  bool IsCorrectStateForExecution(ControlAction a);
  void SetState(State s) {
    bool was_pause = state_ == State::Pause;
    state_ = s;
    NotifyObservers();
    if (s == State::Pause)
      mediator_->Notify(Event::Paused);
    else if (was_pause)
      mediator_->Notify(Event::Unpaused);
  }
  State GetState() const { return state_.load(); }
  bool IsState(State o) { return o == state_; }

 private:
  std::atomic<State> state_{};
};

};  // namespace s21
#endif
