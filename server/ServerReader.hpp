// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057

#ifndef SERVERREADER_H_
#define SERVERREADER_H_

#include <unistd.h>
#include "Reader.hpp"

/** A client reader that reads from a file descriptor and 
 * then sends an ACT to indicate that the message was read.
 */
class ServerReader : public Reader {

private: 
  int fileDescriptor;

public:
  ServerReader(int fileDescriptor = 0) :
      fileDescriptor(fileDescriptor) {

  }

  ServerReader(const ServerReader &other) :
      fileDescriptor(other.fileDescriptor) {
    
  }

  virtual ~ServerReader() {

  }

  void setFileDescriptor(int fileDescriptor) {
    this->fileDescriptor = fileDescriptor;
  }

  virtual void read(char* message, int length) const {
    memset(message, 0, length - 1);
    ::read(fileDescriptor, message, length - 1);
    printf("%s %s\n", "Read: ", message);

    write(fileDescriptor, ACT, strlen(ACT));
    printf("%s %s\n", "Wrote: ", ACK);
  }
};

#endif  // SERVERREADER_H_