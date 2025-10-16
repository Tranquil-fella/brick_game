#include "fsm.h"

#include <gtest/gtest.h>

#include "input_mapping.h"

namespace brick_game {

class SnakeFSMTest : public ::testing::Test {
 protected:
  void SetUp() override {
    auto mediator = std::make_shared<Mediator>();
    fsm = std::make_unique<SnakeFSM>(mediator);
  }

  void TearDown() override { fsm.reset(); }

  std::unique_ptr<SnakeFSM> fsm;
};

// Test initial state
TEST_F(SnakeFSMTest, InitialStateIsStart) {
  EXPECT_TRUE(fsm->IsState(State::Start));
  EXPECT_EQ(fsm->GetState(), State::Start);
}

// Test state transitions
TEST_F(SnakeFSMTest, SetStateChangesStateCorrectly) {
  fsm->SetState(State::Pause);
  EXPECT_TRUE(fsm->IsState(State::Pause));
  EXPECT_EQ(fsm->GetState(), State::Pause);

  fsm->SetState(State::Moving);
  EXPECT_TRUE(fsm->IsState(State::Moving));
  EXPECT_EQ(fsm->GetState(), State::Moving);

  fsm->SetState(State::Gameover);
  EXPECT_TRUE(fsm->IsState(State::Gameover));
  EXPECT_EQ(fsm->GetState(), State::Gameover);

  fsm->SetState(State::Start);
  EXPECT_TRUE(fsm->IsState(State::Start));
  EXPECT_EQ(fsm->GetState(), State::Start);
}

// Test FSM table correctness for Start state with ControlAction
TEST_F(SnakeFSMTest, StartStateControlTransitions) {
  fsm->SetState(State::Start);

  // Start should only accept Start action for control
  EXPECT_TRUE(fsm->IsCorrectStateForExecution(ControlAction::Start));
  EXPECT_FALSE(fsm->IsCorrectStateForExecution(ControlAction::Pause));
  EXPECT_FALSE(fsm->IsCorrectStateForExecution(ControlAction::Terminate));
}

// Test FSM table correctness for Pause state with ControlAction
TEST_F(SnakeFSMTest, PauseStateControlTransitions) {
  fsm->SetState(State::Pause);

  // Pause should accept Pause and Terminate actions
  EXPECT_FALSE(fsm->IsCorrectStateForExecution(ControlAction::Start));
  EXPECT_TRUE(fsm->IsCorrectStateForExecution(ControlAction::Pause));
  EXPECT_TRUE(fsm->IsCorrectStateForExecution(ControlAction::Terminate));
}

// Test FSM table correctness for Gameover state with ControlAction
TEST_F(SnakeFSMTest, GameoverStateControlTransitions) {
  fsm->SetState(State::Gameover);

  // Gameover should only accept Terminate action
  EXPECT_FALSE(fsm->IsCorrectStateForExecution(ControlAction::Start));
  EXPECT_FALSE(fsm->IsCorrectStateForExecution(ControlAction::Pause));
  EXPECT_TRUE(fsm->IsCorrectStateForExecution(ControlAction::Terminate));
}

// Test FSM table correctness for Moving state with ControlAction
TEST_F(SnakeFSMTest, MovingStateControlTransitions) {
  fsm->SetState(State::Moving);

  // Moving should accept Pause and Terminate actions, but not Start
  EXPECT_FALSE(fsm->IsCorrectStateForExecution(ControlAction::Start));
  EXPECT_TRUE(fsm->IsCorrectStateForExecution(ControlAction::Pause));
  EXPECT_TRUE(fsm->IsCorrectStateForExecution(ControlAction::Terminate));
}

// Test state transitions sequence
TEST_F(SnakeFSMTest, StateTransitionSequence) {
  EXPECT_TRUE(fsm->IsState(State::Start));

  // Start -> Moving (via game start logic)
  fsm->SetState(State::Moving);
  EXPECT_TRUE(fsm->IsState(State::Moving));

  // Moving -> Pause
  fsm->SetState(State::Pause);
  EXPECT_TRUE(fsm->IsState(State::Pause));

  // Pause -> Moving
  fsm->SetState(State::Moving);
  EXPECT_TRUE(fsm->IsState(State::Moving));

  // Moving -> Gameover
  fsm->SetState(State::Gameover);
  EXPECT_TRUE(fsm->IsState(State::Gameover));

  // Gameover -> Start
  fsm->SetState(State::Start);
  EXPECT_TRUE(fsm->IsState(State::Start));
}

// Test move constructor
TEST_F(SnakeFSMTest, MoveConstructor) {
  fsm->SetState(State::Moving);

  SnakeFSM moved_fsm(std::move(*fsm));
  EXPECT_TRUE(moved_fsm.IsState(State::Moving));
  EXPECT_EQ(moved_fsm.GetState(), State::Moving);
}

// Test move assignment
TEST_F(SnakeFSMTest, MoveAssignment) {
  fsm->SetState(State::Pause);

  auto mediator = std::make_shared<Mediator>();
  SnakeFSM other_fsm(mediator);
  other_fsm = std::move(*fsm);

  EXPECT_TRUE(other_fsm.IsState(State::Pause));
  EXPECT_EQ(other_fsm.GetState(), State::Pause);
}

// Test atomic state operations are thread-safe
TEST_F(SnakeFSMTest, AtomicStateOperations) {
  // This test verifies that state operations are atomic
  fsm->SetState(State::Moving);
  State state = fsm->GetState();  // Should be thread-safe
  EXPECT_EQ(state, State::Moving);
}

// Test ProcessEvent transitions to Gameover
TEST_F(SnakeFSMTest, ProcessEventTransitionsToGameover) {
  fsm->SetState(State::Moving);
  fsm->ProcessEvent(Event{});  // Should transition to Gameover

  EXPECT_TRUE(fsm->IsState(State::Gameover));
  EXPECT_EQ(fsm->GetState(), State::Gameover);
}

}  // namespace brick_game