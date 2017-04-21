#include <gmock/gmock.h>
#include <iostream>

#include "../../board/Board.hpp"
#include "../../ship/Ship.hpp"

using testing::Eq;
using testing::Test;

#include "../../board/DestroyListener.hpp"
class MockDestroyListener : public DestroyListener {
 public:
  MOCK_CONST_METHOD1(onDestroy, void(char shipId));
  MOCK_CONST_METHOD2(onHit, void(int row, int col));
  MOCK_CONST_METHOD2(onMiss, void(int row, int col));
};

class ABoard : public Test {
 public:
  MockDestroyListener mockDestroyListener;
  Board board;
  char shipId = 'A';
  int shipLength = 3;
  Ship ship;

  ABoard() : board(10, 10, &mockDestroyListener), ship(shipId, shipLength) {
  
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


TEST_F(ABoard, HittingAOneSizedShipDestroysTheShip) {
  char oneLengthShipId = 'B';
  Ship oneLengthShip(oneLengthShipId, 1);
  board.addShip(oneLengthShip, 0, 0, true); 

  EXPECT_CALL(mockDestroyListener, onDestroy(oneLengthShipId));

  board.shoot(0, 0); 
}

TEST_F(ABoard, HittingALargerThanOneSizedShipDoesntDestroyTheShip) {
  board.addShip(ship, 0, 0, true); 

  EXPECT_CALL(mockDestroyListener, onDestroy(shipId))
    .Times(0);

  board.shoot(0, 0); 
}

TEST_F(ABoard, HittingAnEmptyTileCallsOnMiss) {
  int row = 1; 
  int col = 1; 

  EXPECT_CALL(mockDestroyListener, onMiss(row, col));

  board.shoot(row, col);
}


TEST_F(ABoard, HittingAShipCallsOnHit) {
  int row = 1 ;
  int col = 1; 

  board.addShip(ship, row, col, true);

  EXPECT_CALL(mockDestroyListener, onHit(row, col));

  board.shoot(row, col);
}
