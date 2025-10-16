#include "backend.h"

#include "controler.h"
#include "snake_model.h"

void userInput(const UserAction_t action, bool hold) {
  brick_game::Controler<brick_game::SnakeModel>::GetInstance().SendInput(action, hold);
}
GameInfo_t updateCurrentState() {
  return brick_game::Controler<brick_game::SnakeModel>::GetInstance().getGameInfoCopy();
}