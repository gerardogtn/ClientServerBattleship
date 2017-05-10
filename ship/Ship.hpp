// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057
#ifndef SHIP_SHIP_H_
#define SHIP_SHIP_H_

class Ship {
private:
  char id; 
  int length;


public: 

  Ship(char id, int length) : id(id), length(length) {}

  char getId() const { return id; }
  int getLength() const { return length; }
};

#endif  // SHIP_SHIP_H_