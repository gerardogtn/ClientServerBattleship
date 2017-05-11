// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057
#ifndef SERVER_CLIENTCONNECTION_H_
#define SERVER_CLIENTCONNECTION_H_

#include "ServerToClientWriter.hpp"
#include "ServerDestroyListener.hpp"
#include "ServerReader.hpp"
#include "constants.h"

/** A ClientConnection holds all communication from a Battleship
 * server to a Battleship client. It's an internal class that 
 * you must likely won't use in your projects, it's better to delegate your 
 * behaviour to a Server.hpp. 
 */
class ClientConnection {
private:
  int fileDescriptor = 0;
  char buffer[BUFFER_SIZE];
  Board* board = nullptr; 
  Board* enemyBoard = nullptr;
  ServerToClientWriter* writer;
  ServerDestroyListener* destroyListener;
  ServerReader reader;

  void swap(ClientConnection &other) {
    std::swap(fileDescriptor, other.fileDescriptor);
    std::swap(buffer, other.buffer);
    std::swap(board, other.board);
    std::swap(enemyBoard, other.enemyBoard);
    std::swap(writer, other.writer);
    std::swap(destroyListener, other.destroyListener);
    std::swap(reader, other.reader);
  }

public: 
  /** Main constructor, accepts a connection from a socket, sets writer, reader, and destroylistener variables */
  ClientConnection(int socketFileDescriptor, ServerToClientWriter *writer, ServerDestroyListener *destroyListener) :
      writer(writer), 
      destroyListener(destroyListener) {
    fileDescriptor = accept(socketFileDescriptor, NULL, NULL);
    writer->setFileDescriptor(fileDescriptor);
    writer->write(ACT_CONNECTED);
    reader.setFileDescriptor(fileDescriptor);
  }

  /** Copy constructor */
  ClientConnection(const ClientConnection& other) : 
      fileDescriptor(other.fileDescriptor), 
      writer(other.writer), 
      board(other.board), 
      enemyBoard(other.enemyBoard), 
      destroyListener(other.destroyListener), 
      reader(other.fileDescriptor) {

  }

  /** Copy assignment operator */
  ClientConnection& operator= (ClientConnection other) {
    swap(other);
    return *this;
  }

  /** Destructor */
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
    writer->write(ACT);
    reader.read(buffer, BUFFER_SIZE);
    char* command; 
    int x;
    int y;
    sscanf(buffer, "%s %d %d", command, &x, &y);

    if (strncmp(buffer, ACT_SHOOT, strlen(ACT_SHOOT)) == 0) {
      enemyBoard->shoot(x, y);
    }
  }

  /** Call this method to let the client know that they must send their
   * ships
   */
   Board* getShips(int enemyFileDescriptor) {
    writer->write(ACT_SEND);

    board = new Board(ROWS, COLS, destroyListener);
    destroyListener->setFd(fileDescriptor);
    destroyListener->setEnemyFd(enemyFileDescriptor);

    reader.read(buffer, BUFFER_SIZE);
    for (int i  = 0; i < ROWS; i++) {
      for (int j = 0; j < COLS; j++) {
        board->setAtPosition(i, j, buffer[i * ROWS + j]);
      }
    }
    return board;
  }

  /** Return true if a client has lost the game, and thus the connection 
   * is ready to be terminated. 
   */
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