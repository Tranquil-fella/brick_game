#ifndef FIELD_H
#define FIELD_H
#include <bitset>
#include <memory>

#include "backend.h"
#include "colors.h"

namespace brick_game {

struct Cell : public std::pair<int, int> {
  Cell(int f = 0, int s = 0) : std::pair<int, int>(f, s) {}
  Cell operator+(const Cell& o) const {
    Cell res{(first + o.first), (second + o.second)};
    return res;
  }
};

struct FieldView {
  FieldView()
      : field_ptrs_(std::make_unique_for_overwrite<int*[]>(FIELD_LENGTH)),
        next_field_ptrs_(std::make_unique_for_overwrite<int*[]>(NEXTF_LENGTH)),
        flat_data_(
            std::make_unique<int[]>(FIELD_LENGTH * FIELD_WIDTH + NEXTF_WIDTH)) {
    for (int i{}; i < FIELD_LENGTH; ++i) {
      field_ptrs_[i] = &flat_data_[i * FIELD_WIDTH];
    }
    // assign all Next field ptrs to same empty array
    for (int i{}; i < NEXTF_LENGTH; ++i) {
      next_field_ptrs_[i] = &flat_data_[FIELD_LENGTH * FIELD_WIDTH];
    }
  }
  int** GetField() { return field_ptrs_.get(); }
  int** GetNext() { return next_field_ptrs_.get(); }

 private:
  std::unique_ptr<int*[]> field_ptrs_;
  std::unique_ptr<int*[]> next_field_ptrs_;
  std::unique_ptr<int[]> flat_data_;
};

struct Field {
  static const int kTotalCells = FIELD_LENGTH * FIELD_WIDTH;
  void FillCell(Cell c) {
    bitset_[GetCellNum(c)] = 1;
    field_view_outdated_ = true;
  }
  void EmptyCell(Cell c) { bitset_[GetCellNum(c)] = 0; }
  bool CheckCell(Cell c) { return bitset_[GetCellNum(c)]; }

  Cell GetNthFreeCell(int n) {
    int curr_cell{};

    while (true) {
      while (curr_cell < kTotalCells && bitset_[curr_cell]) curr_cell++;
      if (!n) break;
      --n;
      ++curr_cell;
    }
    if (curr_cell >= kTotalCells) curr_cell = 0;
    return {curr_cell / FIELD_WIDTH, curr_cell % FIELD_WIDTH};
  }

  void PlaceFieldAndNext(GameInfo_t& gi, bool gameover) {
    if (field_view_outdated_ || gameover) UpdateFieldView(gameover);

    gi.field = field_view_.GetField();
    gi.next = field_view_.GetNext();
  }
  void PlaceApple(Cell apple) {
    auto [y, x] = apple;
    field_view_.GetField()[y][x] = Red;
  }

 private:
  std::bitset<kTotalCells> bitset_{};
  FieldView field_view_{};
  bool field_view_outdated_ = true;

  void UpdateFieldView(bool gameover) {
    int body_color = gameover ? Damaged : Green;
    for (int y{}; y < FIELD_LENGTH; ++y) {
      for (int x{}; x < FIELD_WIDTH; ++x) {
        field_view_.GetField()[y][x] = CheckCell({y, x}) ? body_color : Empty;
      }
    }
    field_view_outdated_ = false;
  }
  int GetCellNum(const Cell c) {
    const auto [y, x] = c;
    return y * FIELD_WIDTH + x;
  }
};

}  // namespace brick_game

#endif