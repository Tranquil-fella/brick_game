#include "mainwidget.h"

#include <qdir.h>

#include <QShortcut>

#include "./ui_mainwidget.h"
#include "confirmexit.h"

constexpr const char* kLibsPath = "../lib/";

MainWidget::MainWidget(QWidget* parent)
    : QWidget(parent),
      ui(new Ui::MainWidget),
      game_screen_{new GameScreen(this)} {
  ui->setupUi(this);
  ui->PlayButton->hide();
  setTabOrder(ui->SelectGameBox, ui->PlayButton);
  setTabOrder(ui->PlayButton, ui->ExitButton);
  setTabOrder(ui->ExitButton, ui->SelectGameBox);
  ui->SelectGameBox->setFocusPolicy(Qt::StrongFocus);
  ui->PlayButton->setFocusPolicy(Qt::StrongFocus);
  ui->ExitButton->setFocusPolicy(Qt::StrongFocus);

  game_screen_->hide();
  PopulateGameList();
  SetUpSelectGameButton();

  connect(game_screen_, &GameScreen::GameClosed, this,
          &MainWidget::OnGameClosed);
}

MainWidget::~MainWidget() { delete ui; }

void MainWidget::PopulateGameList() {
  QDir libsDir(GetLibsDirPath());
  if (libsDir.exists()) {
    QStringList filters;
    filters << "*.so";
    libsDir.setNameFilters(filters);
    game_list_ = libsDir.entryList(QDir::Files);
  }
}

void MainWidget::SetUpSelectGameButton() {
  for (QString game_name : game_list_) {
    if (game_name.startsWith("lib")) {
      game_name = game_name.mid(3);
    }
    if (game_name.endsWith(".so")) {
      game_name = game_name.left(game_name.length() - 3);
    }
    game_name = game_name.toUpper();
    ui->SelectGameBox->addItem(game_name);
  }
}

QString MainWidget::GetLibsDirPath() {
  return QDir(QCoreApplication::applicationDirPath()).filePath(kLibsPath);
}

void MainWidget::on_PlayButton_clicked() {
  this->ui->Menu->hide();
  game_screen_->show();
}

void MainWidget::on_SelectGameBox_currentIndexChanged(int index) {
  QString lib_path = GetLibsDirPath() + game_list_[index];

  if (game_screen_->LoadGameInterface(lib_path) == EXIT_SUCCESS) {
    ui->PlayButton->show();
  } else {
    ui->PlayButton->hide();
  }
}

void MainWidget::on_ExitButton_clicked() {
  if (ConfirmExit("Do you want to quit application?")) {
    QApplication::exit(0);
  }
}

void MainWidget::keyPressEvent(QKeyEvent* event) {
  if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
    QWidget* focused = focusWidget();
    if (focused == ui->SelectGameBox) {
      ui->SelectGameBox->showPopup();
    } else if (QAbstractButton* button =
                   qobject_cast<QAbstractButton*>(focused)) {
      button->animateClick();
    }
  } else
    QWidget::keyPressEvent(event);
}

void MainWidget::OnGameClosed() {
  this->ui->Menu->show();
  game_screen_->hide();
}
