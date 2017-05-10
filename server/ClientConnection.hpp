// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057
#ifndef SERVER_CLIENTCONNECTION_H_
#define SERVER_CLIENTCONNECTION_H_

#include "ServerToClientWriter.hpp"
#include "ServerDestroyListener.hpp"
#include "constants.h"

class ClientConnection {
private:
  int fileDescriptor = 0;
  char buffer[BUFFER_SIZE];
  Board* board = nullptr; 
  Board* enemyBoard = nullptr;
  // TODO: Switch to Writer
  ServerToClientWriter* writer;
  ServerDestroyListener* destroyListener;

  void swap(ClientConnection &other) {
    std::swap(fileDescriptor, other.fileDescriptor);
    std::swap(buffer, other.buffer);
    std::swap(board, other.board);
    std::swap(enemyBoard, other.enemyBoard);
    std::swap(writer, other.writer);
    std::swap(destroyListener, other.destroyListener);
  }

public: 
  // TODO: Don't receive fileDescriptor but rather obtain it using accept.
  ClientConnection(int socketFileDescriptor, ServerToClientWriter *writer, ServerDestroyListener *destroyListener):
      writer(writer), 
      destroyListener(destroyListener) {
    fileDescriptor = accept(socketFileDescriptor, NULL, NULL);
    writer->setFileDescriptor(fileDescriptor);
    writer->write(ACT_CONNECTED);
  }

  ClientConnection(const ClientConnection& other) : 
      fileDescriptor(other.fileDescriptor), 
      writer(other.writer), 
      board(other.board), 
      enemyBoard(other.enemyBoard), 
      destroyListener(other.destroyListener) {

  }

  ClientConnection& operator= (ClientConnection other) {
    swap(other);
    return *this;
  }

  ~ClientConnection() {
    if (board) {
      delete board;
    }
    close(fileDescriptor);
  }

  int getFileDescriptor() {
    return fileDescriptor;
  }

  void setBoard(Board *board) {
    this->board = board;
  }

  void setEnemyBoard(Board *board) {
    this->enemyBoard = board;
  }

  /** Call this method when the client is ready to start playing. 
   *  This typically occurs when there's a rival available to play with.
   */ 
   void ready() {
    writer->write(ACT_READY);
  }

  /** Call this method to let the client know that they will start the
   * game by attacking.
   */
   void attack() {
    writer->write(ACT_ATTACK);
  }

  /** Call this method to let the client know that they will start the
   *  game by defending.
   */
   void defend() {
    writer->write(ACT_DEFEND);
  }

  /** Call this method to let the client know that they must perform an 
   * action
   */
   void act() {
    write(fileDescriptor, ACT, strlen(ACT));
    memset(buffer, 0, BUFFER_SIZE);
    read(fileDescriptor, buffer, BUFFER_SIZE - 1);
    char* command; 
    int x;
    int y;
    sscanf(buffer, "%s %d %d", command, &x, &y);

    if (strncmp(buffer, ACT_SHOOT, strlen(ACT_SHOOT)) == 0) {
      printf("Shooting from: %d at position: (%d, %d)\n", fileDescriptor, x, y);
      enemyBoard->shoot(x, y);
    }
  }

  /** Call this method to let the client know that they must send their
   * ships
   */
   Board* getShips(int enemyFileDescriptor) {
    write(fileDescriptor, ACT_SEND, strlen(ACT_SEND));
    board = new Board(ROWS, COLS, destroyListener);
    destroyListener->setFd(fileDescriptor);
    destroyListener->setEnemyFd(enemyFileDescriptor);
    memset(buffer, 0, BUFFER_SIZE);
    read(fileDescriptor, buffer, BUFFER_SIZE - 1);
    for (int i  = 0; i < ROWS; i++) {
      for (int j = 0; j < COLS; j++) {
        board->setAtPosition(i, j, buffer[i * ROWS + j]);
      }
    }
    return board;
  }

  bool hasLost() {
    return board->lost();
  }

  /** Call this method to let the client know that they lost.
   *
   */
  void lost() {
    writer->write(ACT_LOST);
  }

  /** Call this method to let the client know that they won.
   *
   */
  void won() {
    writer->write(ACT_WIN);
  }
};

#endif  // SERVER_CLIENTCONNECTION_H_