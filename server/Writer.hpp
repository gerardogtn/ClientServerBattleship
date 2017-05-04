#ifndef WRITER_H_
#define WRITER_H_

#include <string>

class Writer {
public:
  virtual void write(const std::string s) {
    write(s.c_str());
  }

  virtual void write(char const* message) = 0;
};

#endif 