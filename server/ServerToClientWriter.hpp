#ifndef SERVER_SERVERTOCLIENTWRITER_H_
#define SERVER_SERVERTOCLIENTWRITER_H_

#include <unistd.h>
#include "Writer.hpp"

class ServerToClientWriter : public Writer {

private:
  int filedescriptor;
  char buffer[BUFFER_SIZE];

public:

  explicit ServerToClientWriter(int filedescriptor = 0) : filedescriptor(filedescriptor) {

  }

  ServerToClientWriter(const ServerToClientWriter &other) : filedescriptor(other.filedescriptor) {
    
  }

  virtual ~ServerToClientWriter() {

  }

  void setFileDescriptor(int filedescriptor) {
    this->filedescriptor = filedescriptor;
  }

  virtual void write(char const* message) const {
    ::write(filedescriptor, message, strlen(message));
    read(filedescriptor, (char *) buffer, BUFFER_SIZE - 1);
  }

};

#endif  // SERVER_SERVERTOCLIENTWRITER_H_