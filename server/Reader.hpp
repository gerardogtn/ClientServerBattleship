// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057

#ifndef READER_H_
#define READER_H_

/** A simple base class that reads and sets the read value into the
 * variable passed as param. 
 */
class Reader {
public:
  virtual void read(char* message, int length) const = 0;
};

#endif  // READER_H_