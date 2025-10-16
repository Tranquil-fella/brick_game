#ifndef GAMESCREEN_H
#define GAMESCREEN_H

#include <QFrame>
#include <QKeyEvent>  // Add this include
#include <QLibrary>
#include <QTimer>

#include "backend.h"

namespace Ui {
class GameScreen;
}

using inputFunc_t = void (*)(UserAction_t, bool);
using updateFunc_t = GameInfo_t (*)();

struct Interface_t {
  QLibrary* handle{};
  inputFunc_t userInput{};
  updateFunc_t updateCurrentState{};
};

class GameScreen : public QFrame {
  Q_OBJECT

 public:
  explicit GameScreen(QWidget* parent = nullptr);
  ~GameScreen();
  Interface_t& GetInterface() { return interface_; }

 public:
  int LoadGameInterface(QString lib_path);
 signals:
  void GameClosed();

 protected:
  void showEvent(QShowEvent* event) override;
  void hideEvent(QHideEvent* event) override;
  void keyPressEvent(QKeyEvent* event) override;  // Add key press handler

 private slots:
  void onGameTimer();

 private:
  Ui::GameScreen* ui;
  Interface_t interface_;
  QTimer* refresh_timer_;
  void UnloadGameInterface();
};

#endif  // GAMESCREEN_H
