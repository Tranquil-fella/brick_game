#ifndef S21_CONTROLER_H
#define S21_CONTROLER_H

#ifndef TEST_FRIEND
#define TEST_FRIEND
#endif

#include <concepts>

#include "backend.h"
#include "input_mapping.h"
#include "mediator.h"

namespace s21 {
// Концепт для ограничения интерфейса модели
template <typename Model>
concept BrickGameModel =
    requires(Model& m, MovementAction ma, ControlAction ca) {
      m.TakeMoveAction(ma);
      m.TakeGameControlAction(ca);
      m.GetCurrentStateCopy();
    };

// Задаем модель шаблонным параметром, получаем статический полиморфизм
// контролера и возможность переиспользования с другими играми Brickgame
template <BrickGameModel Model>
struct Controler {
  Controler(const Controler&) = delete;
  Controler& operator=(const Controler&) = delete;
  Controler(Controler&& o) : model_(std::move(o.model_), assigner_(this)) {}

  TEST_FRIEND

  static Controler& GetInstance() {
    static Controler instance{};
    return instance;
  }

  void SendInput(const UserAction_t action, bool) {
    auto input = MapUserInput(action);
    std::visit(assigner_, input);
  }

  GameInfo_t getGameInfoCopy() { return model_.GetCurrentStateCopy(); }

 private:
  Controler() = default;
  Model model_{};

  struct ActionAssigner {
    ActionAssigner() = delete;
    ActionAssigner(Controler* const c) { self = c; }
    Controler* self;
    void operator()(MovementAction m) { self->model_.TakeMoveAction(m); }
    void operator()(ControlAction c) { self->model_.TakeGameControlAction(c); }
  };

  ActionAssigner assigner_{this};
};

};  // namespace s21
#endif