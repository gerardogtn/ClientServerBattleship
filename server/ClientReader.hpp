// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057

#ifndef CLIENTREADER_H_
#define CLIENTREADER_H_

#include <unistd.h>
#include "Reader.hpp"

/** A client reader that reads from a file descriptor and 
 * then sends an ACK to indicate that the message was read.
 */
class ClientReader : public Reader {

private: 
  int fileDescriptor;

public:
  ClientReader(int fileDescriptor = 0) :
      fileDescriptor(fileDescriptor) {

  }

  ClientReader(const ClientReader &other) :
      fileDescriptor(other.fileDescriptor) {
    
  }

  virtual ~ClientReader() {

  }

  void setFileDescriptor(int fileDescriptor) {
    this->fileDescriptor = fileDescriptor;
  }

  virtual void read(char* message, int length) const {
    memset(message, 0, length - 1);
    ::read(fileDescriptor, message, length - 1);
    printf("%s %s\n", "Read: ", message);

    write(fileDescriptor, ACK, strlen(ACK));
    printf("%s %s\n", "Wrote: ", ACK);
  }
};

#endif  // CLIENTREADER_H_