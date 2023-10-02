#include "util.h"

class Matrix {
public:
  Matrix(size_t rows, size_t cols);
  float &operator()(size_t i, size_t j);
  float operator()(size_t i, size_t j) const;

private:
  size_t mRows;
  size_t mCols;
  std::vector<float> mData;
};

Matrix::Matrix(size_t rows, size_t cols)
    : mRows(rows), mCols(cols), mData(rows * cols) {}

float &Matrix::operator()(size_t i, size_t j) {
  assert(i < mCols);
  assert(j < mRows);
  return mData[i * mCols + j];
}

float Matrix::operator()(size_t i, size_t j) const {
  assert(i < mCols);
  assert(j < mRows);
  return mData[i * mCols + j];
}

Matrix gen_world_bsp(unsigned int dimx, unsigned int seed, bool invert) {
  auto generator = std::default_random_engine(seed);
  auto res = Matrix(dimx, dimx);
  int rooms = 0;

  while (rooms < clamp(seed % 20, 2, 99)) {
    rooms = rooms + 1;
    int x0 = 0;
    int y0 = 0;
    int x1 = dimx;
    int y1 = dimx;
    int i = 0;
    // bifurcate until room obtained
    while (i < clamp(dimx / 10 - 3, 3, 8)) {
      int is_vert = rint_distr(0, 1)(generator);
      if (is_vert == 1) {
        int dx = int(std::round(std::abs(x1 - x0) / 2));
        int shift_0th = rint_distr(0, 1)(generator);
        if (shift_0th == 1) {
          x0 = x0 + dx;
        } else {
          x1 = x1 - dx;
        }
      } else {
        int dy = int(std::round(std::abs(y1 - y0) / 2));
        int shift_0th = rint_distr(0, 1)(generator);
        if (shift_0th == 1) {
          y0 = y0 + dy;
        } else {
          y1 = y1 - dy;
        }
      }
      i = i + 1;
    }
    // assign 1 to the values that are in the current room
    for (int x = x0; x < x1; x++) {
      for (int y = y0; y < y1; y++) {
        res(x, y) = 1;
      }
    }
  }
  // change 0 -> 1 and 1 -> 0
  if (invert == true) {
    for (int x = 0; x < dimx; x++) {
      for (int y = 0; y < dimx; y++) {
        res(x, y) = res(x, y) * -1 + 1;
      }
    }
  }
  return res;
}