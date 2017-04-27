#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>

#include "Server.hpp"
#include "constants.h"

void setTcpSocketHints(struct addrinfo* hints) {
  memset(hints, 0, sizeof *hints);
  hints->ai_family = AF_INET;
  hints->ai_socktype = SOCK_STREAM;
  hints->ai_flags = AI_PASSIVE;
}

int main(int argc, char const *argv[]){
  struct addrinfo hints;
  setTcpSocketHints(&hints);

  Server server(PORT, &hints, 10); 

  if (server.isValid()) {
    server.mainLoop();
  }

  return 0;
}