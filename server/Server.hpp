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
#include "../board/Board.hpp"

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

  Board* getShips(int fd, int enemy, ServerDestroyListener *listener) {
    write(fd, ACT_SEND, strlen(ACT_SEND));
    Board *board = new Board(ROWS, COLS, listener);
    listener->setFd(fd);
    listener->setEnemyFd(enemy);
    memset(buffer, 0, BUFFER_SIZE);
    read(fd, buffer, BUFFER_SIZE - 1);
    for (int i  = 0; i < ROWS; i++) {
      for (int j = 0; j < COLS; j++) {
        board->setAtPosition(i, j, buffer[i * ROWS + j]);
      }
    }
    return board;
  }

  bool isGameOver() {
    return client1Board->lost() || client2Board->lost();
  }

  void act(int fd, Board *board) {
    write(fd, ACT, strlen(ACT));
    memset(buffer, 0, BUFFER_SIZE);
    read(fd, buffer, BUFFER_SIZE - 1);
    char* command; 
    int x;
    int y;
    sscanf(buffer, "%s %d %d", command, &x, &y);

    if (strncmp(buffer, ACT_SHOOT, strlen(ACT_SHOOT)) == 0) {
      printf("Shooting from: %d at position: (%d, %d)\n", fd, x, y);
      board->shoot(x, y);
    }
  }

  void onGameOver(int fd1, Board* board1, int fd2, Board *board2) {
    if (board1->lost()) {
      write(fd1, ACT_LOST, strlen(ACT_LOST));
      write(fd2, ACT_WIN, strlen(ACT_WIN));
    } else {      
      write(fd2, ACT_LOST, strlen(ACT_LOST));
      write(fd1, ACT_WIN, strlen(ACT_WIN));
    }
  }

  void mainLoop() {              
    while (true) {
      int client1_fd = accept(socketFileDescriptor, NULL, NULL);
      // ClientConnection first(client1_fd);

      int client2_fd = accept(socketFileDescriptor, NULL, NULL);
      write(client2_fd, ACT_CONNECTED, strlen(ACT_CONNECTED));
      read(client2_fd, buffer, BUFFER_SIZE - 1);

      write(client1_fd, ACT_READY, strlen(ACT_READY));
      read(client1_fd, buffer, BUFFER_SIZE - 1);

      write(client2_fd, ACT_READY, strlen(ACT_READY));
      read(client2_fd, buffer, BUFFER_SIZE - 1);



      int pid = fork();
      if (pid < 0) {
        printf("%s. %s\n", "Failed with error", strerror(errno));
      } else if (pid == 0) {
        client1Board = getShips(client1_fd, client2_fd, &destroyListener1);
        client2Board = getShips(client2_fd, client1_fd, &destroyListener2); 

        write(client1_fd, ACT_ATTACK, strlen(ACT_ATTACK));
        read(client1_fd, buffer, BUFFER_SIZE - 1);
        write(client2_fd, ACT_DEFEND, strlen(ACT_DEFEND));
        read(client2_fd, buffer, BUFFER_SIZE - 1);

        while(!isGameOver()) {
          act(client1_fd, client2Board); 
          if (isGameOver()) {
            break;
          }
          act(client2_fd, client1Board);
        }
        onGameOver(client1_fd, client1Board, client2_fd, client2Board);  
        close(client1_fd);
        close(client2_fd);
        delete client1Board;
        delete client2Board;
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