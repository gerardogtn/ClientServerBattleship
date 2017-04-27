#ifndef SERVER_CLIENTCONNECTION_H_
#define SERVER_CLIENTCONNECTION_H_

class ClientConnection {
private:
  int fileDescriptor;
  Board* board; 

public: 
  ClientConnection(int fileDescriptor, Board* board) : fileDescriptor(fileDescriptor),
      board(board) {

  }

  ~ClientConnection() {
    
  }

};

#endif  // SERVER_CLIENTCONNECTION_H_