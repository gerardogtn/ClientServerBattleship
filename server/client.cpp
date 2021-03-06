// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057
#include <limits.h>
#include <iostream>
#include "../board/DestroyListener.hpp"
#include "ClientEventListener.hpp"
#include "Client.hpp"

class ConsoleDestroyListener : public DestroyListener {
 public:
  virtual void onDestroy(char shipId, int row1, int col1, int row2, int col2)  {
    std::cout << "Destroyed ship " << shipId << " located on (" << row1 << ", " << col1 << ") (" << row2 << ", " << col2 << ")\n";
  }
  virtual void onMiss(int row, int col) const {
    std::cout << "No hit on" << "(" << row << ", " << col << ")\n";
  }
  virtual void onHit(int row, int col, bool destroyed) const {
    std::cout << "Hit on" << "(" << row << ", " << col << ")\n";
    if (destroyed) {
      std::cout << destroyed << " And it was destroyed\n";
    } else {
      std::cout << destroyed << " And nothing especial happened\n";
    }
  }
};

class MyConsoleDestroyListener : public DestroyListener {
 public:
  virtual void onDestroy(char shipId, int row1, int col1, int row2, int col2)  {
    std::cout << "Your ship was destroyed " << shipId << " located on (" << row1 << ", " << col1 << ") (" << row2 << ", " << col2 << ")\n";
  }
  virtual void onMiss(int row, int col) const {
    std::cout << "The other shot " << "(" << row << ", " << col << ") and missed\n";
  }
  virtual void onHit(int row, int col, bool destroyed) const {
    std::cout << "The other shot on " << "(" << row << ", " << col << ") and hit your ship\n";
    if (destroyed) {
      std::cout << "And it was destroyed\n";
    } else {
      std::cout << "And nothing especial happened\n";
    }
  }
};

class ConsoleClientEventListener : public ClientEventListener {
 public:
  virtual void connected() {
    std::cout << "Connected! Waiting for opponent...\n";
  }

  virtual void ready() {
    std::cout << "Ready to play!\n";
  }

  virtual void shoot(int &row, int &col) {
    std::cout << "Insert the position where you want to shoot in the format (x, y)\n";

    std::cin.clear();
    std::cin.ignore(INT_MAX, '\n');
    if (!(std::cin >> row >> col)) {
      std::cout << "There was an error with cin\n";
    }
  }

  virtual void sendShips(char ships[ROWS][COLS]) {
    std::cout << "write '1' to send the a collection of ships, anything else for the b connection\n"; 
    int input; 
    if (!(std::cin >> input)) {
      std::cout << "cin error\n";
    } 
    for (int i = 0; i < ROWS; i++) {
      for (int j = 0; j < COLS; j++) {
        ships[i][j] = '0';
      }
    }
    if (input == 1) {
      ships[0][0] = 'A';
      ships[0][1] = 'A';
    } else {
      ships[3][3] = 'B';
      ships[3][4] = 'B';
      ships[3][5] = 'B';
    }
  }


  virtual void lost() {
    std::cout << "You lost! :(\n";
  }

  virtual void won() {
    std::cout << "CONGRATULATIONS!! You won!\n";
  }

  virtual void onAttack() {
    std::cout << "You start attacking\n";
  }

  virtual void onDefend() {
    std::cout << "You start defending\n";
  }

};


int main(int argc, char const *argv[]){
  ConsoleDestroyListener destroyListener;
  MyConsoleDestroyListener myConsoleDestroyListener;
  ConsoleClientEventListener clientListener;

  char const * hostname; 

  if (argc > 1) {
    hostname = argv[1];
  } else {
    hostname = "localhost";
  }

  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  hints.ai_family = AF_INET;
  hints.ai_socktype = SOCK_STREAM;

  Client client(PORT, "localhost", &hints, &clientListener, &destroyListener, &myConsoleDestroyListener);
  if (client.isValid()) {
    client.mainLoop();
  }
  return 0;
}