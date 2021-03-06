// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057
#ifndef BOARD_BOARDENTRY_H_
#define BOARD_BOARDENTRY_H_

class BoardEntry {
 private:
  char id;
  bool _hit; 

 public:
  BoardEntry() {
    this->id = '0';
    this->_hit = false;
  }

  BoardEntry(const BoardEntry &entry) {
    this->id = entry.id;
    this->_hit = entry._hit;
  }

  const char getId() { return id; }
  void setId(char id) { this->id = id; }

  void hit() { this->_hit = true; }
  const bool isHit() { return this->_hit; }
};


#endif  // BOARD_BOARDENTRY_H_