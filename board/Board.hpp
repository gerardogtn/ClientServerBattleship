#ifndef BOARD_BOARD_HPP
#define BOARD_BOARD_HPP

#include <stdlib.h>
#include <new>
#include "BoardEntry.hpp"
#include "DestroyListener.hpp"
#include "../ship/Ship.hpp"

class Board {
 private:
  BoardEntry** entries;
  DestroyListener* listener;
  int rows;
  int cols;

 public:
  Board(int rows, int columns, DestroyListener* listener) : listener(listener) {
    this->rows = rows;
    this->cols = columns;
    entries = new BoardEntry*[rows]; 
    for (int i = 0; i < rows; i++) {
      entries[i] = new BoardEntry[columns];
    }
  }

  Board(const Board& other) : Board(other.rows, other.cols, other.listener) {
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        entries[i][j] = other.entries[i][j];
      }
    }
  }

  virtual ~Board() {
    for (int i = 0; i < rows; i++) {
      delete[] entries[i];
    }
    delete[] entries;
  }

  int getRows() { return rows; }
  int getColumns() { return cols; }

  /** Adds a ship to the board
   * PARAMS:
   *   ship -> the ship to add. 
   *   (row, col) -> the x, y position of the initial location to add the ship.
   *   horizontally -> true if the ship orientation is horizontal, false otherwise.
   * RETURNS:
   *   True if the ship was added, false if there was an error adding.
   */ 
  bool addShip(const Ship& ship, int row, int col, bool horizontally) {
    if (horizontally) {
      if (col + ship.getLength() > this->rows) return false;
      for(int i = 0; i < ship.getLength(); i++) {
        entries[row][col + i].setId(ship.getId());
      }
    } else {
      if (row + ship.getLength() > this->cols) return false;
      for(int i = 0; i < ship.getLength(); i++) {
        entries[row + i][col].setId(ship.getId());
      }
    }
    return true;
  }

  char getShipIdAtPosition(int row, int col) {
    return entries[row][col].getId();
  }

  bool destroyed(char shipId) {
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        BoardEntry entry = entries[i][j];
        if (entry.getId() == shipId && !entry.isHit()) {
          return false;
        }
      }
    }
    return true;
  }

  void shoot(int row, int col) {
    BoardEntry& entry = entries[row][col];
    entry.hit();
    if (destroyed(entry.getId()) && entry.getId() != '0') {
      listener->onDestroy(entry.getId());
    } else if (entry.getId() != '0') {
      listener->onHit(row, col);
    } else {
      listener->onMiss(row, col);
    }
  }
};

#endif  // BOARD_BOARD_HPP