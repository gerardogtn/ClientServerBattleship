#ifndef SERVER_CLIENTCONNECTION_H_
#define SERVER_CLIENTCONNECTION_H_

#include "ServerToClientWriter.hpp"
#include "constants.h"

class ClientConnection {
private:
  int fileDescriptor = 0;
  Board* board = nullptr; 
  // TODO: Switch to Writer
  ServerToClientWriter* writer;

  void swap(ClientConnection &other) {
    std::swap(fileDescriptor, other.fileDescriptor);
    std::swap(board, other.board);
    std::swap(writer, other.writer);
  }

public: 
  // TODO: Don't receive fileDescriptor but rather obtain it using accept.
  ClientConnection(int fileDescriptor, ServerToClientWriter* writer)
      : fileDescriptor(fileDescriptor), writer(writer) {
      writer->setFileDescriptor(fileDescriptor);
      writer->write(ACT_CONNECTED);
  }

  ClientConnection(const ClientConnection& other) : fileDescriptor(other.fileDescriptor), 
      writer(other.writer), board(other.board) {

  }

  ClientConnection& operator= (ClientConnection other) {
    swap(other);
    return *this;
  }

  ~ClientConnection() {
    
  }

  // TODO: Handle board internally. 
  void setBoard(Board *board) {
    this->board = board;
  }

  /** Call this method when the connection is ready to start playing. 
   *  This typically occurs when there's a rival available to play with.
   */ 
  void ready() {
    writer->write(ACT_READY);
  }

  /** Call this method to let the connection know that they will start the
   * game by attacking.
   */
  void attack() {
    writer->write(ACT_ATTACK);
  }

  /** Call this method to let the connection know that they will start the
   *  game by defending.
   */
  void defend() {
    writer->write(ACT_DEFEND);
  }

};

#endif  // SERVER_CLIENTCONNECTION_H_