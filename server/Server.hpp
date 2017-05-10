// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057

#ifndef SERVER_SERVER_H_
#define SERVER_SERVER_H_

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include "ServerDestroyListener.hpp"
#include "ServerToClientWriter.hpp"
#include "../board/Board.hpp"
#include "ClientConnection.hpp"

#include "constants.h"

class Server {
private:
  struct addrinfo *addressInfo;

  int addressInfoResult;
  int socketFileDescriptor;
  int bindResult;
  int listenResult;

  char buffer[BUFFER_SIZE];
  Board* client1Board = nullptr;
  Board* client2Board = nullptr;
  ServerDestroyListener destroyListener1;
  ServerDestroyListener destroyListener2;

  ServerToClientWriter firstWriter;
  ServerToClientWriter secondWriter;

public:

  /** Create a new server at the given port with maxConnections given the address settings.
   * Following the creation of the server isValid() must be called to know if the resource
   * acquisition succeeded. 
   */ 
  Server(char const *port, struct addrinfo *hints, int maxConnections) {
    addressInfoResult = getaddrinfo(NULL, port, hints, &addressInfo);
    socketFileDescriptor = socket(PF_INET, SOCK_STREAM, 0);
    bindResult = bind(socketFileDescriptor, addressInfo->ai_addr, addressInfo->ai_addrlen);
    listenResult = listen(socketFileDescriptor, maxConnections);
  }

  /** Returns true if the server is ready to be used, false otherwise.
   *
   */
  bool isValid() {
    return addressInfoResult == 0 && socketFileDescriptor != -1 && bindResult != -1 && listenResult != -1;
  }

  bool isGameOver() {
    return client1Board->lost() || client2Board->lost();
  }

  void onGameOver(int fd1, Board* board1, int fd2, Board *board2) {
    if (board1->lost()) {
      firstWriter.write(ACT_LOST);
      secondWriter.write(ACT_WIN);
    } else {      
      firstWriter.write(ACT_WIN);
      secondWriter.write(ACT_LOST);
    }
  }

  bool clientAction(ClientConnection *client, ClientConnection *enemy) {
    if (client->hasLost()) {
      client->lost();
      enemy->won();
      return false;
    } else {
      client->act();
      return true;
    }
  }

  void mainLoop() {              
    while (true) {
      ClientConnection firstConnection(socketFileDescriptor, &firstWriter, &destroyListener1);
      ClientConnection secondConnection(socketFileDescriptor, &secondWriter, &destroyListener2);

      firstConnection.ready();
      secondConnection.ready();

      int pid = fork();
      if (pid < 0) {
        printf("%s. %s\n", "Failed with error", strerror(errno));
      } else if (pid == 0) {
        client1Board = firstConnection.getShips(secondConnection.getFileDescriptor());
        client2Board = secondConnection.getShips(firstConnection.getFileDescriptor()); 

        firstConnection.setEnemyBoard(client2Board);
        secondConnection.setEnemyBoard(client1Board);

        firstConnection.attack();
        secondConnection.defend();
        
        while(true) {
          if (firstConnection.hasLost()) {
            firstConnection.lost();
            secondConnection.won();
            break;
          }
          firstConnection.act();

          if (secondConnection.hasLost()) {
            secondConnection.lost();
            firstConnection.won();
            break;
          }
          secondConnection.act();
        }
        return;
      } else {

      }
    }
  }

  ~Server() {
    freeaddrinfo(addressInfo);
    close(socketFileDescriptor);
    if (client1Board) {
      delete client1Board;
    }
    if (client2Board) {
      delete client2Board;
    }
  }

};

#endif  // SERVER_SERVER_H_ 