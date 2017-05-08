#ifndef WRITER_H_
#define WRITER_H_

#include <string>

class Writer {
public:
  virtual void write(char const* message) const = 0;
};

#endif 