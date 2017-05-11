// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057

#ifndef CLIENTTOSERVERWRITER_H_
#define CLIENTTOSERVERWRITER_H_

#include <unistd.h>
#include "Writer.hpp"

/** 
 * A simple writer class that writes from a filedescriptor and waits for a 
 * reply from the filedescriptor, whose content is ignored.
 */
class ClientToServerWriter : public Writer {

private:
  int filedescriptor;
  char buffer[BUFFER_SIZE];

  void swap(ClientToServerWriter &other) {
    std::swap(filedescriptor, other.filedescriptor);
    std::swap(buffer, other.buffer);
  }

public:

  // Main constructor, by default it's set to a filedescriptor of 
  // STDOUT; you must likely want to set this filedescriptor before
  // you make calls to write. 
  explicit ClientToServerWriter(int filedescriptor = 0) : 
      filedescriptor(filedescriptor) {

  }

  /** Copy constructor */
  ClientToServerWriter(const ClientToServerWriter &other) : 
      filedescriptor(other.filedescriptor) {
    
  }

  /** Copy assignment operator */
  ClientToServerWriter& operator= (ClientToServerWriter other) {
    swap(other);
    return *this;
  }

  /** Destructor */
  virtual ~ClientToServerWriter() {

  }

  void setFileDescriptor(int filedescriptor) {
    this->filedescriptor = filedescriptor;
  }

  // Writes a message to filedescriptor and waits for a response from 
  // filedescriptor.
  virtual void write(char const* message) const {
    printf("%s %s\n", "Wrote: ", message);
    ::write(filedescriptor, message, strlen(message));

    read(filedescriptor, (char *) buffer, BUFFER_SIZE);
    printf("%s %s\n", "Read: ", buffer);
  }

};

#endif  // CLIENTTOSERVERWRITER_H_