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

  explicit BoardEntry(const BoardEntry &entry) {
    this->id = entry.id;
    this->_hit = entry._hit;
  }

  const char getId() { return id; }
  void setId(char id) { this->id = id; }

  void hit() { this->_hit = true; }
  const bool isHit() { return this->_hit; }
};


#endif  // BOARD_BOARDENTRY_H_