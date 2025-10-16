#ifndef CONFIRMEXIT_H
#define CONFIRMEXIT_H

#include <QMessageBox>

inline bool ConfirmExit(const QString msg) {
  QMessageBox msgBox;
  msgBox.setWindowTitle("Confirm Exit");
  msgBox.setText(msg);
  msgBox.setIcon(QMessageBox::Question);
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);

  // Modern styling
  msgBox.setStyleSheet(
      "QMessageBox {"
      "    background-color: #f8f9fa;"
      "    font-family: 'Segoe UI', Arial;"
      "}"
      "QMessageBox QLabel {"
      "    color: #2c3e50;"
      "    font-size: 14px;"
      "}"
      "QMessageBox QPushButton {"
      "    background-color: #3498db;"
      "    color: white;"
      "    padding: 8px 16px;"
      "    border: none;"
      "    border-radius: 4px;"
      "    min-width: 80px;"
      "}"
      "QMessageBox QPushButton:hover {"
      "    background-color: #2980b9;"
      "}"
      "QMessageBox QPushButton#yesButton {"
      "    background-color: #e74c3c;"
      "}"
      "QMessageBox QPushButton#yesButton:hover {"
      "    background-color: #c0392b;"
      "}");

  return msgBox.exec() == QMessageBox::Yes;
}
#endif  // CONFIRMEXIT_H
