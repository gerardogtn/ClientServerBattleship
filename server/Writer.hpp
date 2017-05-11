// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057

#ifndef WRITER_H_
#define WRITER_H_

#include <string>

/** A simple abstract class that provides an interface for writing a message */
class Writer {
public:
  virtual void write(char const* message) const = 0;
};

#endif 