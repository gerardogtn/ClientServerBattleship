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

  void setAtPosition(int row, int col, char shipid) {
    entries[row][col].setId(shipid);
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

  bool lost() {
    for (int i = 0; i < rows; i++) {
      for (int j = 0; j < cols; j++) {
        BoardEntry entry = entries[i][j];
        if (entry.getId() != '0' && !entry.isHit()) {
          return false;
        }
      }
    }
    return true;
  }


  void getShipsPosition(char shipId, int *x1, int *y1, int *x2, int *y2) {
    int i;
    int j;
    bool exit = false;
    for (i = 0; i < rows; i++) {
      for (j = 0; j < cols; j++) {
        BoardEntry entry = entries[i][j];
        if (entry.getId() == shipId) {
          *x1 = i; 
          *y1 = j;
          exit = true;
          break;
        }
      }
      if (exit) {
        break;
      }
    }

    int next = i + 1; 
    if (next < rows && entries[next][j].getId() == shipId) {
      while (++next < rows && entries[next][j].getId() == shipId) {
      
      }
      *x2 = next - 1;
      *y2 = j;
      return;
    }
    next = j + 1; 
    if (next < cols && entries[i][next].getId() == shipId) {
      while (++next < cols && entries[i][next].getId() == shipId) {

      }
      *x2 = i; 
      *y2 = next -1;
      return;
    }
    *x2 = next - 1; 
    *y2 = next - 1;

  }

  void shoot(int row, int col) {
    BoardEntry& entry = entries[row][col];
    entry.hit();
    if (destroyed(entry.getId()) && entry.getId() != '0') {
      listener->onHit(row, col);
      int x1, y1, x2, y2;
      getShipsPosition(entry.getId(), &x1, &y1, &x2, &y2);
      listener->onDestroy(entry.getId(), x1, y1, x2, y2);
    } else if (entry.getId() != '0') {
      listener->onHit(row, col);
    } else {
      listener->onMiss(row, col);
    }
  }
};

#endif  // BOARD_BOARD_HPP