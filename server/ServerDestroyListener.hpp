// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057

#ifndef SERVER_SERVERDESTROYLISTENER_H_
#define SERVER_SERVERDESTROYLISTENER_H_

#include <stdio.h>
#include <string.h>

#include "../board/DestroyListener.hpp"
#include "constants.h"
#include "ServerToClientWriter.hpp"

class ServerDestroyListener : public DestroyListener {
 private:
  char buffer[BUFFER_SIZE];
  ServerToClientWriter writer;
  ServerToClientWriter enemyWriter;
 public:

  ServerDestroyListener(int fd = 0, int enemyFd = 0) : 
      writer(fd), 
      enemyWriter(enemyFd) {

  }

  ServerDestroyListener(const ServerDestroyListener &other) : 
      writer(other.writer),
      enemyWriter(other.enemyWriter) {

  }

  void setFd(int fd) {
    writer.setFileDescriptor(fd);
  }

  void setEnemyFd(int fd) {
    enemyWriter.setFileDescriptor(fd);
  }

  virtual ~ServerDestroyListener() {

  }

  virtual void onDestroy(char shipId, int row1, int col1, int row2, int col2) {
    memset((char *)buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *)buffer, ACT_DESTROY " %d %d %d %d %d", shipId, row1, col1, row2, col2);
    writer.write(buffer);

    memset((char *)buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *)buffer, ACT_DESTROY " %d %d %d %d %d", shipId, row1, col1, row2, col2);
    enemyWriter.write(buffer);
  }

  virtual void onMiss(int row, int col) const {
    memset((char *)buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *)buffer, ACT_MISS " %d %d", row, col);
    writer.write(buffer);

    memset((char *)buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *)buffer, ACT_MISS " %d %d", row, col);
    writer.write(buffer);
  }

  virtual void onHit(int row, int col, bool hit) const {
    memset((char *)buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *)buffer, ACT_HIT " %d %d %d", row, col, hit);
    writer.write(buffer);

    memset((char *)buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *)buffer, ACT_HIT " %d %d %d", row, col, hit);
    writer.write(buffer);
  }
};

#endif  // SERVER_SERVERDESTROYLISTENER_H_