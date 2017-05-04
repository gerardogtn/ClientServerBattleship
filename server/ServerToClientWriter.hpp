#ifndef SERVER_SERVERTOCLIENTWRITER_H_
#define SERVER_SERVERTOCLIENTWRITER_H_

#include <unistd.h>
#include "Writer.hpp"

class ServerToClientWriter : public Writer {

private:
  int filedescriptor;
  char buffer[BUFFER_SIZE];

public:

  ServerToClientWriter(int filedescriptor) : filedescriptor(filedescriptor) {

  }

  virtual ~ServerToClientWriter() {

  }

  virtual void write(char const* message) {
    ::write(filedescriptor, message, strlen(message));
    read(filedescriptor, (char *) buffer, BUFFER_SIZE - 1);
  }

};

#endif  // SERVER_SERVERTOCLIENTWRITER_H_