#include "backend.h"

#include "controler.h"
#include "snake_model.h"

void userInput(const UserAction_t action, bool hold) {
  s21::Controler<s21::SnakeModel>::GetInstance().SendInput(action, hold);
}
GameInfo_t updateCurrentState() {
  return s21::Controler<s21::SnakeModel>::GetInstance().getGameInfoCopy();
}