#ifndef INPUT_MAPPING_H
#define INPUT_MAPPING_H

#include <stdexcept>
#include <variant>

#include "backend.h"

namespace s21 {
enum class MovementAction { Left, Right, Up, Down, Action };
enum class ControlAction { Start, Pause, Terminate };

inline std::variant<MovementAction, ControlAction> MapUserInput(
    UserAction_t action) {
  std::variant<MovementAction, ControlAction> input;
  if (action > Terminate && action <= Action) {
    input = static_cast<MovementAction>(static_cast<int>(action) -
                                        static_cast<int>(Left));
  } else if (action >= Start && action <= Terminate) {
    input = static_cast<ControlAction>(action);
  } else {
    throw std::invalid_argument("unknown value for enum argumen \"action\"");
  }
  return input;
}

}  // namespace s21

#endif