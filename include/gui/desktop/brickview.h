#ifndef BRICKVIEW_H
#define BRICKVIEW_H

#include <QGraphicsScene>
#include <QGraphicsView>

class BrickView : public QGraphicsView {
  Q_OBJECT

 public:
  explicit BrickView(QWidget* parent = nullptr);
  void InitField(int width, int height);
  void UpdateField(int** data);

 private:
  QGraphicsScene* scene_;
  int cell_size_;
  int grid_width_;
  int grid_height_;
  struct Cell {
    QGraphicsRectItem* item;
    int cashed_value;
  };

  QVector<QVector<Cell>> cells_;
  void AssignColor(Cell& c, QColor);
  void DismissColor(Cell& c);
};

#endif  // BRICKVIEW_H
