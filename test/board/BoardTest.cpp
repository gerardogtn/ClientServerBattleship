#include <gmock/gmock.h>
#include <iostream>

#include "../../board/Board.hpp"
#include "../../ship/Ship.hpp"

using testing::Eq;
using testing::Test;

class ABoard : public Test {
 public:
  Board board;
  char shipId = 'A';
  int shipLength = 3;
  Ship ship;

  ABoard() : board(10, 10), ship(shipId, shipLength) {
  
  }

  ~ABoard() {

  }
};

TEST_F(ABoard, AddingShipHorizontallyFillsEntries) {
  Board copy = board;

  ASSERT_THAT(board.addShip(ship, 0, 0, true), Eq(true));

  for(int i = 0; i < board.getRows(); i++) {
    for(int j = 0; j < board.getColumns(); j++) {
      if (i == 0 && (j == 0 || j == 1 || j == 2)) {
        ASSERT_THAT(board.getShipIdAtPosition(i, j), Eq(shipId));
      } else {
        ASSERT_THAT(board.getShipIdAtPosition(i, j), Eq(copy.getShipIdAtPosition(i, j)));
      }
    }
  }
}

TEST_F(ABoard, AddingShipVerticallyFillsEntries) {
  Board copy = board;

  ASSERT_THAT(board.addShip(ship, 0, 0, false), Eq(true));

  for(int i = 0; i < board.getRows(); i++) {
    for(int j = 0; j < board.getColumns(); j++) {
      if (j == 0 && (i == 0 || i == 1 || i == 2)) {
        ASSERT_THAT(board.getShipIdAtPosition(i, j), Eq(shipId));
      } else {
        ASSERT_THAT(board.getShipIdAtPosition(i, j), Eq(copy.getShipIdAtPosition(i, j)));
      }
    }
  }
}

TEST_F(ABoard, AddingShipThatDoesntFitReturnsFalse) {
  ASSERT_THAT(board.addShip(ship, 0, 9, true), Eq(false)); 
}
