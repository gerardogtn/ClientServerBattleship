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

public: 
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

  Client(const Client &other) {

  }

  virtual ~Client() {
    freeaddrinfo(addressInfo);
    close(socketFileDescriptor);
  }

  bool isValid() {
    return socketFileDescriptor != -1 && addressInfoResult == 0 && socketConnectionResult != -1;
  }

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

  virtual void end() {

  }

};

#endif  // SERVER_CLIENT_H_