#ifndef MOVE_TIMER_H
#define MOVE_TIMER_H

#include <atomic>
#include <chrono>
#include <thread>

#include "backend.h"
#include "mediator.h"

namespace s21 {
using namespace std::chrono_literals;
using msec = std::chrono::milliseconds;
constexpr msec kStartDelay = 500ms;
constexpr double kDelayDecay = 0.85;

struct MoveTimer : public Component {
  MoveTimer(std::shared_ptr<Mediator> m)
      : Component(m), timer_{[this](auto token) { TimerLoop(token); }} {}
  MoveTimer(MoveTimer&& o)
      : Component(std::move(o)),
        delay_(o.delay_.load()),
        skip_movement_{o.skip_movement_.load()},
        paused{o.paused.load()},
        timer_{[this](auto token) { TimerLoop(token); }} {}

  MoveTimer& operator=(MoveTimer&& o) {
    if (this != &o) {
      Component::operator=(std::move(o));
      delay_ = o.delay_.load();
      skip_movement_ = o.skip_movement_.load();
      paused = o.paused.load();
      timer_ = std::jthread{[this](auto token) { TimerLoop(token); }};
    }
    return *this;
  }

  ~MoveTimer() override { StopTimer(); }

  void PlayerMoved() { skip_movement_.store(true); }

  void IncreaseSpeed() {
    msec current = delay_.load();
    msec new_delay = msec(static_cast<int>(current.count() * kDelayDecay));
    delay_.store(new_delay);
  }

  void ProcessEvent(Event e) override {
    switch (e) {
      case Event::PlayerMoved:
        PlayerMoved();
        break;
      case Event::NewLevel:
        IncreaseSpeed();
        break;
      case Event::Paused:
        paused = true;
        break;
      case Event::Unpaused:
        paused = false;
        paused.notify_one();
        break;
      default:
        break;
    }
  }

 private:
  std::atomic<msec> delay_ = kStartDelay;
  std::atomic_bool skip_movement_{};
  std::atomic_bool paused{};
  std::jthread timer_;

  void TimerLoop(std::stop_token token) {
    while (!token.stop_requested()) {
      std::this_thread::sleep_for(delay_.load());
      if (!skip_movement_ && this->mediator_)
        this->mediator_->Notify(Event::TimeToMove);
      else
        skip_movement_ = false;
      paused.wait(true);
    }
  }

  void StopTimer() {
    timer_.request_stop();
    paused = false;
    paused.notify_one();
  }
};
};  // namespace s21
#endif