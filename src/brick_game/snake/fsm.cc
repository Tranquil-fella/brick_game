#include "fsm.h"

#include <bitset>

static const std::bitset<12> fsm_control_bitset{
    "110"  // Moving
    "100"  // Gameover
    "110"  // Pause
    "001"  // Start
};

bool brick_game::SnakeFSM::IsCorrectStateForExecution(ControlAction a) {
  State curr = state_;
  return fsm_control_bitset[static_cast<unsigned int>(curr) * 3 +
                            static_cast<unsigned int>(a)];
}
