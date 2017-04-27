#ifndef SERVER_SERVERDESTROYLISTENER_H_
#define SERVER_SERVERDESTROYLISTENER_H_

#include <stdio.h>
#include <string.h>

#include "../board/DestroyListener.hpp"
#include "constants.h"


class ServerDestroyListener : public DestroyListener {
 private:
  int fd;
  int enemy;
  char buffer[BUFFER_SIZE];
 public:

  ServerDestroyListener(int fd = 0, int enemyFd = 0) : fd(fd), enemy(enemyFd) {

  }

  ServerDestroyListener(const ServerDestroyListener &other) : ServerDestroyListener(other.fd) {

  }

  void setFd(int fd) {
    this->fd = fd;
  }

  void setEnemyFd(int fd) {
    this->enemy = fd;
  }

  virtual ~ServerDestroyListener() {

  }

  virtual void onDestroy(char shipId, int row1, int col1, int row2, int col2) const {
    printf("Destroying ship: %c at position (%d, %d) (%d, %d)\n", shipId, row1, col1, row2, col2);
    memset((char *)buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *)buffer, ACT_DESTROY " %d %d %d %d %d", shipId, row1, col1, row2, col2);
    write(fd, buffer, strlen(buffer));

    memset((char *)buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *)buffer, ACT_DESTROY " %d %d %d %d %d", shipId, row1, col1, row2, col2);
    write(enemy, buffer, strlen(buffer));
  }

  virtual void onMiss(int row, int col) const {
    memset((char *)buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *)buffer, ACT_MISS " %d %d\n", row, col);
    write(fd, buffer, strlen(buffer));

    memset((char *)buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *)buffer, ACT_MISS " %d %d\n", row, col);
    write(enemy, buffer, strlen(buffer));
  }

  virtual void onHit(int row, int col) const {
    memset((char *)buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *)buffer, ACT_HIT " %d %d\n", row, col);
    write(fd, buffer, strlen(buffer));

    memset((char *)buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *)buffer, ACT_HIT " %d %d\n", row, col);
    write(enemy, buffer, strlen(buffer));
  }
};

#endif  // SERVER_SERVERDESTROYLISTENER_H_