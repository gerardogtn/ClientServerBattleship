// Battleship Server.

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

// For sockets.
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#define CHANNEL_SIZE 10

int channels[CHANNEL_SIZE];

void mainLoop(int socket_fd) {
  while (1) {
    int client1_fd = accept(socket_fd, NULL, NULL);
    int cilent2_fd = accept(socket_fd, NULL, NULL);

    if (newProcess == 0) {
      return;
    } else if (newProcess == - 1) {
      printErrorAndExit("Error forking");
    }


  }
}

void setTcpSocketHints(struct addrinfo* hints) {
  memset(hints, 0, sizeof *hints);
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM;
  hints->ai_flags = AI_PASSIVE;
}

int main(int argc, char const *argv[]) {
  struct addrinfo hints;
  setTcpSocketHints(&hints);

  struct addrinfo *result;

  int temp = getaddrinfo(NULL, PORT, &hints, &result);
  if (temp != 0) {
    printErrorAndExit("Error getting address info");
  }

  int socket_fd = socket(PF_INET, SOCK_STREAM, 0);
  checkForError(socket_fd, -1, "Error creating socket");

  temp = bind(socket_fd, result->ai_addr, result->ai_addrlen);
  checkForError(temp, -1, "Error binding socket");

  temp = listen(socket_fd, MAX_CONNECTIONS);
  checkForError(temp, -1, "Error listening for connections");

  freeaddrinfo(result);
  mainLoop(socket_fd);


  close(socket_fd);

  return 0;
}