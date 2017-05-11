// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057

#ifndef SERVER_SERVERTOCLIENTWRITER_H_
#define SERVER_SERVERTOCLIENTWRITER_H_

#include <unistd.h>
#include "Writer.hpp"

/** 
 * A simple writer class that writes from a filedescriptor and waits for a 
 * reply from the filedescriptor, whose content is ignored.
 */
class ServerToClientWriter : public Writer {

private:
  int filedescriptor;
  char buffer[BUFFER_SIZE];

  void swap(ServerToClientWriter &other) {
    std::swap(filedescriptor, other.filedescriptor);
    std::swap(buffer, other.buffer);
  }

public:

  explicit ServerToClientWriter(int filedescriptor = 0) : filedescriptor(filedescriptor) {

  }

  ServerToClientWriter(const ServerToClientWriter &other) : filedescriptor(other.filedescriptor) {
    
  }

  ServerToClientWriter& operator= (ServerToClientWriter other) {
    swap(other);
    return *this;
  }

  virtual ~ServerToClientWriter() {

  }

  void setFileDescriptor(int filedescriptor) {
    this->filedescriptor = filedescriptor;
  }

  virtual void write(char const* message) const {
    printf("%s %s\n", "Wrote: ", message);
    ::write(filedescriptor, message, strlen(message));

    read(filedescriptor, (char *) buffer, BUFFER_SIZE - 1);
    printf("%s %s\n", "Read: ", buffer);
  }

};

#endif  // SERVER_SERVERTOCLIENTWRITER_H_