// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057
#ifndef SERVER_GAME_H_
#define SERVER_GAME_H_

#define GAME_BUFFER_SIZE 1024

class Game {

private:
  int clientOneFileDescriptor;
  int clientTwoFileDescriptor

  Board clientOneBoard;
  Board clientTwoBoard;

  DestroyListener* clientOneDestroyListener;
  DestroyListener* clientTwoDestroyListener;

  char buffer[GAME_BUFFER_SIZE];

public:
  Game(int clientOneFileDescriptor, int clientTwoFileDescriptor, int rows, int cols, DestroyListener* clientOneDestroyListener, DestroyListener* clientTwoDestroyListener) 
      : clientOneBoard(rows, cols, clientOneDestroyListener),
        clientTwoBoard(rows, cols, clientTwoDestroyListener),
        clientOneDestroyListener(cilentOneDestroyListener),
        clientTwoDestroyListener(clientTwoDestroyListener) {
    this->clientOneFileDescriptor = clientOneFileDescriptor;
    this->clientTwoFileDescriptor = clientTwoFileDescriptor;
  }

  Game(const Game &other) : Game(other.clientOneFileDescriptor, other.clientTwoFileDescriptor,
      other.rows, other.cols) {
    close(clientOneFileDescriptor);
    close(clientTwoFileDescriptor);
  }

  ~Game();

  void play() {
    getShips(clientOneFileDescriptor, &clientOneBoard);
    getShips(clientTwoFileDescriptor, &clientTwoBoard);

    while (!isGameOver()) {
      play(clientOneFileDescriptor); 
      if (isGameOver()) {
        break;
      } 
      play(clientTwoFileDescriptor);
    }
    onGameOver();
  }

private:
  void onGameOver() {
    if (clientOneBoard.lost()) {
      won(clientOneFileDescriptor);
      lost(clientTwoFileDescriptor);
    } else {
      won(clientTwoFileDescriptor);
      lost(clientOneFileDescriptor);
    }
  }

  void play() {
    
  }

  void getShips(int fileDescriptor, Board * board) {
    read(fileDescriptor, buffer, GAME_BUFFER_SIZE - 1);
    for (int i = 0; i < ROWS; i++) {
      for (int j = 0; j < COLS; j++) {
        board.setAtPosition(buffer[i * ROWS + j], i, j);
      }
    }
  }

  bool isGameOver() {
    return clientOneBoard.lost() || clientTwoBoard.lost();
  }

};

#endif  // SERVER_GAME_H_