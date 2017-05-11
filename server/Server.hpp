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
  int connectionNumber = 0;

  char buffer[BUFFER_SIZE];
  Board* client1Board = nullptr;
  Board* client2Board = nullptr;
  ServerDestroyListener destroyListener1;
  ServerDestroyListener destroyListener2;

  ServerToClientWriter firstWriter;
  ServerToClientWriter secondWriter;

  void swap(Server &other) {
    std::swap(addressInfo, other.addressInfo);
    std::swap(addressInfoResult, other.addressInfoResult);
    std::swap(socketFileDescriptor, other.socketFileDescriptor);
    std::swap(bindResult, other.bindResult);
    std::swap(listenResult, other.listenResult);
    std::swap(connectionNumber, other.connectionNumber);
    std::swap(buffer, other.buffer);
    std::swap(client1Board, other.client1Board);
    std::swap(client2Board, other.client2Board);
    std::swap(destroyListener1, other.destroyListener1);
    std::swap(destroyListener2, other.destroyListener2);
    std::swap(firstWriter, other.firstWriter);
    std::swap(secondWriter, other.secondWriter);
  }

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

  /** Copy constructor */
  Server(const Server &other) :
      addressInfo(other.addressInfo),
      socketFileDescriptor(other.socketFileDescriptor),
      bindResult(other.bindResult),
      listenResult(other.listenResult),
      connectionNumber(other.connectionNumber),
      client1Board(other.client1Board),
      client2Board(other.client2Board),
      destroyListener1(other.destroyListener1),
      destroyListener2(other.destroyListener2),
      firstWriter(other.firstWriter),
      secondWriter(other.secondWriter) {

  }

   /** Copy assignment operator */
  Server& operator= (Server other) {
    swap(other);
    return *this;
  }

  /** Destructor */
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

  /** Returns true if the server is ready to be used, false otherwise. */
   bool isValid() {
    return addressInfoResult == 0 && socketFileDescriptor != -1 && bindResult != -1 && listenResult != -1;
  }


  /** Main function to run a Battleship server, waits for two client connections 
   * to occur to later init a game. A call to this function must be afer verifying
   * the validy of the server via a call to isValid(), otherwise undefined behaviour
   * may occur. 
   */
  void mainLoop() {              
    while (true) {
      ClientConnection firstConnection(socketFileDescriptor, &firstWriter, &destroyListener1);
      ClientConnection secondConnection(socketFileDescriptor, &secondWriter, &destroyListener2);
      int pid = fork();
      if (pid < 0) {
        errorProcess();
      } else if (pid == 0) {
        return childProcess(firstConnection, secondConnection);
      } else {
        parentProcess();
      }
    }
  }

private:

  /** Function for parent process, only increases the connection counter */
  void parentProcess() {
    this->connectionNumber++;
  }

  /** Handles all aspects of communication with a client via delegation to two ClientConnections */
  void childProcess(ClientConnection &firstConnection, ClientConnection &secondConnection) {
    firstConnection.ready();
    secondConnection.ready();

    client1Board = firstConnection.getShips(secondConnection.getFileDescriptor());
    client2Board = secondConnection.getShips(firstConnection.getFileDescriptor()); 

    firstConnection.setEnemyBoard(client2Board);
    secondConnection.setEnemyBoard(client1Board);

    firstConnection.attack();
    secondConnection.defend();

    while(true) {
      if (onClientLoose(firstConnection, secondConnection)) {
        break;
      }
      firstConnection.act();

      if (onClientLoose(secondConnection, firstConnection)) {
        break;
      }
      secondConnection.act();
    }
    return;
  }

  /** Function for forking error, simply prints error to STDERR */
  void errorProcess() {
    printf("%s. %s\n", "Failed with error", strerror(errno));
  }

  /** Checks if client has lost and handles communication appropiately */
  bool onClientLoose(ClientConnection &client, ClientConnection &enemy) {
    if (client.hasLost()) {
      client.lost();
      enemy.won();
      return true;
    }
    return false;
  }

};

#endif  // SERVER_SERVER_H_ 