#include "brickview.h"

#include <QBrush>
#include <QGraphicsRectItem>
#include <QPen>

#include "colors.h"

BrickView::BrickView(QWidget* parent) : QGraphicsView(parent) {
  scene_ = new QGraphicsScene(this);
  setScene(scene_);
  setRenderHint(QPainter::Antialiasing);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setFocusPolicy(Qt::StrongFocus);
}

void BrickView::InitField(int width, int height) {
  grid_width_ = width;
  grid_height_ = height;

  // Calculate cell size based on current widget dimensions
  int availableWidth = this->width();
  int availableHeight = this->height();

  // Calculate maximum cell size that fits both dimensions
  int cellSizeFromWidth = availableWidth / width;
  int cellSizeFromHeight = availableHeight / height;

  // Use the smaller cell size to ensure everything fits
  cell_size_ = qMin(cellSizeFromWidth, cellSizeFromHeight);

  // Ensure minimum cell size for visibility
  if (cell_size_ < 4) {
    cell_size_ = 4;  // Minimum reasonable size
  }

  scene_->clear();
  cells_.clear();
  cells_.resize(height);

  // Create all cell items upfront
  for (int y = 0; y < height; y++) {
    cells_[y].resize(width);
    for (int x = 0; x < width; x++) {
      QGraphicsRectItem* cell = new QGraphicsRectItem(
          x * cell_size_, y * cell_size_, cell_size_, cell_size_);

      scene_->addItem(cell);
      cells_[y][x] = {cell, 0};
      DismissColor(cells_[y][x]);
    }
  }

  // Update scene and view size
  setSceneRect(0, 0, width * cell_size_, height * cell_size_);
}

void BrickView::UpdateField(int** data) {
  for (int y = 0; y < grid_height_; y++) {
    for (int x = 0; x < grid_width_; x++) {
      int color = data[y][x];
      if (cells_[y][x].cashed_value == color) continue;
      cells_[y][x].cashed_value = color;

      if (color == Empty) {
        DismissColor(cells_[y][x]);
      } else {
        QColor q_color;
        switch (color) {
          case Static:
            q_color = QColor(255, 255, 255);
            break;  // White
          case Damaged:
            q_color = QColor(255, 0, 0, 128);
            break;  // Semi-transparent red

          case Red:
            q_color = QColor(255, 0, 0);
            break;
          case Magenta:
            q_color = QColor(255, 0, 255);
            break;
          case Green:
            q_color = QColor(0, 255, 0);
            break;
          case Cyan:
            q_color = QColor(0, 255, 255);
            break;
          case Yellow:
            q_color = QColor(255, 255, 0);
            break;
          case Orange:
            q_color = QColor(255, 165, 0);
            break;
          case Blue:
            q_color = QColor(0, 0, 255);
            break;

          default:
            q_color = Qt::white;
        }
        AssignColor(cells_[y][x], q_color);
      }
    }
  }
}

void BrickView::AssignColor(Cell& c, QColor color) {
  c.item->setBrush(QBrush(color));
  c.item->setPen(QPen(color.darker(), 2));
}
void BrickView::DismissColor(Cell& c) {
  c.item->setBrush(QBrush(Qt::gray));
  c.item->setPen(QPen(Qt::black, 1));
}
