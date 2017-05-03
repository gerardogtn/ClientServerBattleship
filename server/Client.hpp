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

public: 
  Client(char const *port, char const *hostname, struct addrinfo *hints, ClientEventListener *listener, 
    DestroyListener *destroyEventListener, DestroyListener *myEventListener) {
    socketFileDescriptor = socket(PF_INET, SOCK_STREAM, 0);
    addressInfoResult = getaddrinfo(hostname, port, hints, &addressInfo);
    socketConnectionResult = connect(socketFileDescriptor, addressInfo->ai_addr, addressInfo->ai_addrlen);
    this->listener = listener;
    this->destroyEventListener = destroyEventListener;
    this->myEventListener = myEventListener;
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

    write(socketFileDescriptor, buffer, strlen(buffer));
  }

  void shoot() {
    int x = 3;
    int y = 3;
    listener->shoot(x, y); 
    sprintf(buffer, ACT_SHOOT " %d %d\n", x, y); 
    write(socketFileDescriptor, buffer, strlen(buffer));
    memset(buffer, 0, BUFFER_SIZE - 1);
    read(socketFileDescriptor, buffer, BUFFER_SIZE - 1); 
    if (strncmp(buffer, ACT_HIT, strlen(ACT_HIT)) == 0) {
      sscanf(buffer, ACT_HIT " %d %d", &x, &y); 
      destroyEventListener->onHit(x, y);

      write(socketFileDescriptor, ACT, strlen(ACT));
      printf("Wrote act to %d\n", socketFileDescriptor);
    } else if (strncmp(buffer, ACT_MISS, strlen(ACT_MISS)) == 0) {
      sscanf(buffer, ACT_MISS " %d %d", &x, &y); 
      destroyEventListener->onMiss(x, y);

      write(socketFileDescriptor, ACT, strlen(ACT));
      printf("Wrote act to %d\n", socketFileDescriptor);
    } else if (strncmp(buffer, ACT_DESTROY, strlen(ACT_DESTROY)) == 0) {
      int id, x2, y2;
      sscanf(buffer, ACT_DESTROY " %d %d %d %d %d", &id, &x, &y, &x2, &y2); 
      destroyEventListener->onDestroy(id, x, y, x2, y2);

      write(socketFileDescriptor, ACT, strlen(ACT));
      printf("Wrote act to %d\n", socketFileDescriptor);
    }
  }

  void mainLoop() {
    memset(buffer, 0, BUFFER_SIZE - 1);
    read(socketFileDescriptor, buffer, BUFFER_SIZE - 1);
    while(strncmp(buffer, ACT_WIN, strlen(ACT_WIN)) != 0 && strncmp(buffer, ACT_LOST, strlen(ACT_LOST)) != 0) {
      int x, y, id, x2, y2;

      if (strncmp(buffer, ACT_CONNECTED, strlen(ACT_CONNECTED)) == 0) {
        write(socketFileDescriptor, ACT, strlen(ACT));
        listener->connected();
      } else if (strncmp(buffer, ACT_READY, strlen(ACT_READY)) == 0) {
        write(socketFileDescriptor, ACT, strlen(ACT));
        listener->ready();
      } else if (strncmp(buffer, ACT_ATTACK, strlen(ACT_ATTACK)) == 0) {
        write(socketFileDescriptor, ACT, strlen(ACT));
        listener->onAttack();
      } else if (strncmp(buffer, ACT_DEFEND, strlen(ACT_DEFEND)) == 0) {
        write(socketFileDescriptor, ACT, strlen(ACT));
        listener->onDefend();
      } else if (strncmp(buffer, ACT_SEND, strlen(ACT_SEND)) == 0) {
        sendShips();
      } else if (strncmp(buffer, ACT, strlen(ACT)) == 0) {
        shoot();
      } else if (strncmp(buffer, ACT_HIT, strlen(ACT_HIT)) == 0) {
        sscanf(buffer, ACT_HIT " %d %d", &x, &y); 
        myEventListener->onHit(x, y);

        write(socketFileDescriptor, ACT, strlen(ACT));
        printf("Wrote act to %d\n", socketFileDescriptor);
      } else if (strncmp(buffer, ACT_MISS, strlen(ACT_MISS)) == 0) {
        sscanf(buffer, ACT_MISS " %d %d", &x, &y); 
        myEventListener->onMiss(x, y);

        write(socketFileDescriptor, ACT, strlen(ACT));
        printf("Wrote act to %d\n", socketFileDescriptor);
      } else if (strncmp(buffer, ACT_DESTROY, strlen(ACT_DESTROY)) == 0) {
        printf("%s\n", "New destroy");
        int id, x2, y2;
        sscanf(buffer, ACT_DESTROY " %d %d %d %d %d", &id, &x, &y, &x2, &y2); 
        myEventListener->onDestroy(id, x, y, x2, y2);

        write(socketFileDescriptor, ACT, strlen(ACT));
        printf("Wrote act to %d\n", socketFileDescriptor);
      }
      memset(buffer, 0, BUFFER_SIZE - 1);
      read(socketFileDescriptor, buffer, BUFFER_SIZE - 1);
    }

    if (strncmp(buffer, ACT_WIN, strlen(ACT_WIN)) == 0) {
      listener->won();
    } else if (strncmp(buffer, ACT_LOST, strlen(ACT_LOST)) == 0) {
      listener->lost();
    }
  }

  Client(const Client &other) {

  }

  virtual void end() {

  }

  virtual ~Client() {
    freeaddrinfo(addressInfo);
    close(socketFileDescriptor);
  }

};

#endif  // SERVER_CLIENT_H_