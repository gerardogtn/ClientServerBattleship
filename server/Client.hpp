// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057
#ifndef SERVER_CLIENT_H_
#define SERVER_CLIENT_H_

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include "constants.h"
#include "../board/DestroyListener.hpp"
#include "ClientEventListener.hpp"
#include "ClientReader.hpp"
#include "ClientToServerWriter.hpp"

/** A simple class that connects to a socket at a given hostname
 * and port, it handles all the logic for Client Communication 
 * to a Battleship and communicates back to its owner via a 
 * ClientEventListener and DestroyListener. 
 */ 
class Client {
private:
  struct addrinfo *addressInfo;

  int addressInfoResult;
  int socketFileDescriptor;
  int socketConnectionResult;
  char buffer[BUFFER_SIZE];
  ClientEventListener *listener;
  DestroyListener *destroyEventListener;
  DestroyListener *myEventListener;
  ClientReader clientReader;
  ClientToServerWriter writer;

  void swap(Client &other) {
    std::swap(addressInfoResult, other.addressInfoResult);
    std::swap(socketFileDescriptor, other.socketFileDescriptor);
    std::swap(socketConnectionResult, other.socketConnectionResult);
    std::swap(buffer, other.buffer);
    std::swap(listener, other.listener);
    std::swap(destroyEventListener, other.destroyEventListener);
    std::swap(myEventListener, other.myEventListener);
    std::swap(clientReader, other.clientReader);
  }

public: 
  /** Main constructor, creates and binds to socket and sets listeners. */
  Client(char const *port, char const *hostname, struct addrinfo *hints, ClientEventListener *listener, 
    DestroyListener *destroyEventListener, DestroyListener *myEventListener) {
    socketFileDescriptor = socket(PF_INET, SOCK_STREAM, 0);
    addressInfoResult = getaddrinfo(hostname, port, hints, &addressInfo);
    socketConnectionResult = connect(socketFileDescriptor, addressInfo->ai_addr, addressInfo->ai_addrlen);
    this->listener = listener;
    this->destroyEventListener = destroyEventListener;
    this->myEventListener = myEventListener;
    writer.setFileDescriptor(socketFileDescriptor);
    clientReader.setFileDescriptor(socketFileDescriptor);
  }

  /** Copy constructor */
  Client(const Client &other) :
    addressInfo(other.addressInfo),
    addressInfoResult(other.addressInfoResult),
    socketFileDescriptor(other.socketFileDescriptor),
    socketConnectionResult(other.socketConnectionResult),
    listener(other.listener),
    destroyEventListener(other.destroyEventListener),
    myEventListener(other.myEventListener),
    clientReader(other.clientReader),
    writer(other.writer) {

  }

  /** Copy assignment operator */
  Client& operator=(Client other) {
    swap(other);
    return *this;
  }

  /** Destructor, frees address info and closes filedescriptor */
  virtual ~Client() {
    freeaddrinfo(addressInfo);
    close(socketFileDescriptor);
  }

  /** Returns true if client is ready to be used, false otherwise */
  bool isValid() {
    return socketFileDescriptor != -1 && addressInfoResult == 0 && socketConnectionResult != -1;
  }

  /** Main method to be called from the client. It handles all the 
   * communication aspects with a Battleship server. Make sure that
   * before calling this method you call isValid() and it yields true,
   * otherwise undefined behaviour may occur. 
   */
  void mainLoop() {
    clientReader.read(buffer, BUFFER_SIZE);
    while(true) {
      int x, y, id, x2, y2;

      if (strncmp(buffer, ACT_WIN, strlen(ACT_WIN)) == 0) {
        listener->won();
      } else if (strncmp(buffer, ACT_LOST, strlen(ACT_LOST)) == 0) {
        listener->lost();
      } else if (strncmp(buffer, ACT_CONNECTED, strlen(ACT_CONNECTED)) == 0) {
        listener->connected();
      } else if (strncmp(buffer, ACT_READY, strlen(ACT_READY)) == 0) {
        listener->ready();
      } else if (strncmp(buffer, ACT_ATTACK, strlen(ACT_ATTACK)) == 0) {
        listener->onAttack();
      } else if (strncmp(buffer, ACT_DEFEND, strlen(ACT_DEFEND)) == 0) {
        listener->onDefend();
      } else if (strncmp(buffer, ACT_SEND, strlen(ACT_SEND)) == 0) {
        sendShips();
      } else if (strncmp(buffer, ACT, strlen(ACT)) == 0) {
        shoot();
      } else if (strncmp(buffer, ACT_HIT, strlen(ACT_HIT)) == 0) {
        int destroyed = 0;
        sscanf(buffer, ACT_HIT " %d %d %d", &x, &y, &destroyed); 
        myEventListener->onHit(x, y, destroyed);
      } else if (strncmp(buffer, ACT_MISS, strlen(ACT_MISS)) == 0) {
        sscanf(buffer, ACT_MISS " %d %d", &x, &y); 
        myEventListener->onMiss(x, y);
      } else if (strncmp(buffer, ACT_DESTROY, strlen(ACT_DESTROY)) == 0) {
        int id, x2, y2;
        sscanf(buffer, ACT_DESTROY " %d %d %d %d %d", &id, &x, &y, &x2, &y2); 
        myEventListener->onDestroy(id, x, y, x2, y2);
      }
      clientReader.read(buffer, BUFFER_SIZE);
    }
  }

private:
  /** Receives the ships positions from the ClientEventListener via a 
   * return parameter. It then writes the ships tot the socket.  
   */
  void sendShips() {
    char ships[ROWS][COLS];
    listener->sendShips(ships);
    int i, j;
    for (int i = 0; i < ROWS; i++) {
      for (int j = 0; j < COLS; j++) {
        buffer[i * ROWS + j] = ships[i][j];
      }
    }

    writer.write(buffer);
  }

  /** Performs a shoot action, it first waits for the ClientEventListener
   * to give the shot parameters, then tells the server the position to shoot. 
   * If a hit action occured and destroyed a ship, then a ship action will follow.
   * Otherwise a simple miss action will be called on this client's DestroyListener.
   */
  void shoot() {
    int x = 3;
    int y = 3;
    listener->shoot(x, y); 

    memset((char *) buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *) buffer, ACT_SHOOT " %d %d", x, y);
    writer.write(buffer);

    writer.write(ACT_SHOOT);
    clientReader.read(buffer, BUFFER_SIZE);
    if (strncmp(buffer, ACT_HIT, strlen(ACT_HIT)) == 0) {
      int destroyed = 0;
      sscanf(buffer, ACT_HIT " %d %d %d", &x, &y, &destroyed); 
      destroyEventListener->onHit(x, y, destroyed);

      if (destroyed == 1) {
        clientReader.read(buffer, BUFFER_SIZE ); 
        int id, x2, y2;
        sscanf(buffer, ACT_DESTROY " %d %d %d %d %d", &id, &x, &y, &x2, &y2); 
        destroyEventListener->onDestroy(id, x, y, x2, y2);
      }
    } else if (strncmp(buffer, ACT_MISS, strlen(ACT_MISS)) == 0) {
      sscanf(buffer, ACT_MISS " %d %d", &x, &y); 
      destroyEventListener->onMiss(x, y);
    } 
  }

};

#endif  // SERVER_CLIENT_H_