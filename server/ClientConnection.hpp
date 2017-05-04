#ifndef SERVER_CLIENTCONNECTION_H_
#define SERVER_CLIENTCONNECTION_H_

#include "ServerToClientWriter.hpp"

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

  void ready() {
    writer->write(ACT_READY);
  }

};

#endif  // SERVER_CLIENTCONNECTION_H_