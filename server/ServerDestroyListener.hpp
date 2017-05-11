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

/** A ServerDestroyListener listens for hit, miss, and destroy events 
 * and writes the event to the file descriptors of the two players via 
 * a Writer
 */
class ServerDestroyListener : public DestroyListener {
 private:
  char buffer[BUFFER_SIZE];
  ServerToClientWriter writer;
  ServerToClientWriter enemyWriter;

  void swap(ServerDestroyListener& other) {
    std::swap(buffer, other.buffer);
    std::swap(writer, other.writer);
    std::swap(enemyWriter, other.writer);
  }

 public:

  /** Main constructor, defaults to writing to STDOUT */
  ServerDestroyListener(int fd = 0, int enemyFd = 0) : 
      writer(fd), 
      enemyWriter(enemyFd) {

  }

  /** Copy constructor */
  ServerDestroyListener(const ServerDestroyListener &other) : 
      writer(other.writer),
      enemyWriter(other.enemyWriter) {

  }

  /** Copy assignment operator */
  ServerDestroyListener& operator=(ServerDestroyListener other) {
    swap(other);
    return *this;
  }

  void setFd(int fd) {
    writer.setFileDescriptor(fd);
  }

  void setEnemyFd(int fd) {
    enemyWriter.setFileDescriptor(fd);
  }

  /** Destructor */
  virtual ~ServerDestroyListener() {

  }

  /** Notifies both clients that a destroy event occurred. */
  virtual void onDestroy(char shipId, int row1, int col1, int row2, int col2) {
    memset((char *) buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *) buffer, ACT_DESTROY " %d %d %d %d %d", shipId, row1, col1, row2, col2);
    writer.write(buffer);

    memset((char *) buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *) buffer, ACT_DESTROY " %d %d %d %d %d", shipId, row1, col1, row2, col2);
    enemyWriter.write(buffer);
  }

  /** Notifies both clients that a miss event occurred. */
  virtual void onMiss(int row, int col) const {
    memset((char *) buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *) buffer, ACT_MISS " %d %d", row, col);
    writer.write(buffer);

    memset((char *)buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *)buffer, ACT_MISS " %d %d", row, col);
    enemyWriter.write(buffer);
  }

  /** Notifies both clients that a hit event occurred. */
  virtual void onHit(int row, int col, bool hit) const {
    memset((char *) buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *) buffer, ACT_HIT " %d %d %d", row, col, hit);
    writer.write(buffer);

    memset((char *) buffer, 0, BUFFER_SIZE - 1);
    sprintf((char *) buffer, ACT_HIT " %d %d %d", row, col, hit);
    enemyWriter.write(buffer);
  }
};

#endif  // SERVER_SERVERDESTROYLISTENER_H_