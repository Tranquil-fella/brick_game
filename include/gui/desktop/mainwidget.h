#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QLibrary>
#include <QStringList>
#include <QWidget>

#include "backend.h"
#include "gamescreen.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWidget;
}
QT_END_NAMESPACE

using inputFunc_t = void (*)(UserAction_t, bool);
using updateFunc_t = GameInfo_t (*)();

class MainWidget : public QWidget {
  Q_OBJECT

 public:
  MainWidget(QWidget* parent = nullptr);
  ~MainWidget();

 private slots:
  void on_PlayButton_clicked();
  void on_SelectGameBox_currentIndexChanged(int index);
  void on_ExitButton_clicked();
  void OnGameClosed();

 private:
  Ui::MainWidget* ui;
  struct Interface_t {
    QLibrary* handle{};
    inputFunc_t userInput{};
    updateFunc_t updateCurrentState{};
  };
  Interface_t interface_{};
  QStringList game_list_;
  GameScreen* game_screen_;
  void keyPressEvent(QKeyEvent* e) override;
  void PopulateGameList();
  void SetUpSelectGameButton();
  QString GetLibsDirPath();
};
#endif  // MAINWIDGET_H
