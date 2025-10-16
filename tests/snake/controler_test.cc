#include <stdexcept>

#include "backend.h"
#include "gtest/gtest.h"
#include "input_mapping.h"

template <typename Model>
struct ControlerTestImpl;
#define TEST_FRIEND friend struct ControlerTestImpl<Model>;
#include "controler.h"
using namespace s21;

struct MockModel {
  void TakeMoveAction(MovementAction a) {
    lastAction = a;
    moveActionCalled = true;
    controlActionCalled = false;
  }

  void TakeGameControlAction(ControlAction a) {
    lastAction = a;
    controlActionCalled = true;
    moveActionCalled = false;
  }

  GameInfo_t GetCurrentStateCopy() {
    GameInfo_t gi{};
    gi.score = 69;
    return gi;
  }

  std::variant<MovementAction, ControlAction> lastAction{};
  bool moveActionCalled{false};
  bool controlActionCalled{false};
};

template <typename Model>
struct ControlerTestImpl {
  s21::Controler<Model>& controller_ = s21::Controler<Model>::GetInstance();
  Model& GetModel() { return controller_.model_; }
};

class ControlerTest : public ::testing::Test {
 protected:
  void SetUp() override {}
  ControlerTestImpl<MockModel> ctrl_test_;
};

// Test concept satisfaction
TEST_F(ControlerTest, MockModelSatisfiesConcept) {
  static_assert(s21::BrickGameModel<MockModel>,
                "MockModel should satisfy BrickGameModel concept");
}

// Test SendInput with move action
TEST_F(ControlerTest, SendInputCallsTakeMoveActionForMoveActions) {
  UserAction_t moveAction = Action;
  bool hold = true;

  ctrl_test_.controller_.SendInput(moveAction, hold);
  auto& model = ctrl_test_.GetModel();

  EXPECT_TRUE(model.moveActionCalled);
  EXPECT_FALSE(model.controlActionCalled);
  EXPECT_EQ(model.lastAction, MapUserInput(moveAction));
}

// Test SendInput with control action
TEST_F(ControlerTest, SendInputCallsTakeGameControlActionForControlActions) {
  UserAction_t controlAction = Start;
  bool hold = false;

  ctrl_test_.controller_.SendInput(controlAction, hold);
  auto& model = ctrl_test_.GetModel();

  EXPECT_TRUE(model.controlActionCalled);
  EXPECT_FALSE(model.moveActionCalled);
  EXPECT_EQ(model.lastAction, MapUserInput(controlAction));
}

// Test exception for unknown action
TEST_F(ControlerTest, SendInputThrowsForUnknownAction) {
  UserAction_t unknownAction = static_cast<UserAction_t>(999);

  EXPECT_THROW(ctrl_test_.controller_.SendInput(unknownAction, false),
               std::invalid_argument);
}

// Test getGameInfoCopy
TEST_F(ControlerTest, GetGameInfoCopyReturnsModelState) {
  GameInfo_t info = ctrl_test_.controller_.getGameInfoCopy();
  EXPECT_EQ(info.score, 69);
}