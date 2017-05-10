#ifndef BOARD_DESTROYLISTENER_H_
#define BOARD_DESTROYLISTENER_H_

class DestroyListener {
 public:
  virtual void onDestroy(char shipId, int row1, int col1, int row2, int col2) = 0;
  virtual void onMiss(int row, int col) const = 0;
  virtual void onHit(int row, int col, bool destroyed) const = 0;
};

#endif  // BOARD_DESTROYLISTENER_H_