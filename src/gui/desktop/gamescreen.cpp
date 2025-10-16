#include "gamescreen.h"

#include <QHideEvent>
#include <QKeyEvent>
#include <QShowEvent>

#include "confirmexit.h"
#include "ui_gamescreen.h"

constexpr int kUpdateInterval = 1000 / 60;

GameScreen::GameScreen(QWidget *parent)
    : QFrame(parent), ui(new Ui::GameScreen), refresh_timer_(new QTimer(this)) {
  refresh_timer_->setInterval(kUpdateInterval);
  connect(refresh_timer_, &QTimer::timeout, this, &GameScreen::onGameTimer);

  ui->setupUi(this);

  QTimer::singleShot(1, this, [this]() {
    // This runs AFTER the constructor returns and layout is complete
    ui->FieldView->InitField(FIELD_WIDTH, FIELD_LENGTH);
    ui->NextView->InitField(NEXTF_WIDTH, NEXTF_LENGTH);
  });
}

GameScreen::~GameScreen() {
  if (refresh_timer_->isActive()) {
    refresh_timer_->stop();
  }
  UnloadGameInterface();
  delete ui;
}

void GameScreen::showEvent(QShowEvent *event) {
  QFrame::showEvent(event);
  setFocus();
  interface_.userInput(Start, false);
  refresh_timer_->start();
}

void GameScreen::hideEvent(QHideEvent *event) {
  QFrame::hideEvent(event);
  refresh_timer_->stop();
}

void GameScreen::keyPressEvent(QKeyEvent *event) {
  UserAction_t action;
  bool isAutoRepeat = event->isAutoRepeat();

  switch (event->key()) {
    case Qt::Key_Left:
      action = Left;
      break;
    case Qt::Key_Right:
      action = Right;
      break;
    case Qt::Key_Up:
      action = Up;
      break;
    case Qt::Key_Down:
      action = Down;
      break;
    case Qt::Key_Space:
      action = Action;
      break;
    case Qt::Key_Enter:
    case Qt::Key_Return:
      action = Start;
      break;
    case Qt::Key_P:
    case Qt::Key_Pause:
      action = Pause;
      break;
    case Qt::Key_Escape:
      if (ConfirmExit("Do you want to stop current game?")) {
        action = Terminate;
        emit GameClosed();
        break;
      } else
        return;
    default:
      QFrame::keyPressEvent(event);
      return;
  }

  interface_.userInput(action, isAutoRepeat);
}

void GameScreen::onGameTimer() {
  GameInfo_t game_info = interface_.updateCurrentState();

  ui->FieldView->UpdateField(game_info.field);
  ui->NextView->UpdateField(game_info.next);
  ui->ScoreLcdNumber->display(game_info.score);
  ui->HighscoreLcdNumber->display(game_info.high_score);
  ui->LevelLcdNumber->display(game_info.level);
  ui->SpeedLcdNumber->display(game_info.speed);
  ui->StatusLabel->setHidden(!game_info.pause);

  if (!game_info.level) {
    refresh_timer_->stop();
    ui->StatusLabel->setText("Game Over");
    ui->StatusLabel->show();
  }
}

int GameScreen::LoadGameInterface(QString lib_path) {
  UnloadGameInterface();
  interface_.handle = new QLibrary(lib_path);

  if (!interface_.handle || !interface_.handle->load()) {
    return EXIT_FAILURE;
  }

  interface_.userInput = (inputFunc_t)interface_.handle->resolve("userInput");
  interface_.updateCurrentState =
      (updateFunc_t)interface_.handle->resolve("updateCurrentState");

  if (!interface_.userInput || !interface_.updateCurrentState) {
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

void GameScreen::UnloadGameInterface() {
  if (interface_.handle) {
    if (interface_.handle->isLoaded()) {
      interface_.handle->unload();
    }
    delete interface_.handle;
    interface_.handle = nullptr;
  }
  interface_.userInput = nullptr;
  interface_.updateCurrentState = nullptr;
}
