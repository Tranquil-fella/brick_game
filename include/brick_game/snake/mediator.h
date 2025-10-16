#ifndef MEDIATOR_H
#define MEDIATOR_H

#include <array>
#include <memory>
#include <vector>

#include "events.h"

namespace s21 {

struct Mediator;

struct Component : public std::enable_shared_from_this<Component> {
  Component(std::shared_ptr<Mediator> m) : mediator_(std::move(m)) {}
  Component(Component&&) = default;
  Component& operator=(Component&&) = default;

  Component(const Component&) = delete;

  virtual void ProcessEvent(Event) = 0;
  virtual ~Component() = default;

 protected:
  std::shared_ptr<Mediator> mediator_;
};

struct Mediator {
  Mediator() = default;
  Mediator(const Mediator&) = delete;
  Mediator(Mediator&&) = default;
  Mediator& operator=(Mediator&&) = default;
  ~Mediator() = default;

  void AddSubscriber(std::weak_ptr<Component> sub, Event event) {
    subscribers_[EventToInt(event)].push_back(sub);
  }
  virtual void Notify(Event e) { NotifySubs(e); }

  void Reset() {
    for (auto& list : subscribers_) list.clear();
  }

 protected:
  using NotifyList = std::vector<std::weak_ptr<Component>>;
  std::array<NotifyList, kEventsSize> subscribers_ = {};

  void NotifySubs(Event e) {
    for (auto& weak : subscribers_[EventToInt(e)]) {
      if (auto sub = weak.lock()) sub->ProcessEvent(e);
    }
  }
};

};  // namespace s21

#endif