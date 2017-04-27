#ifndef SERVER_CLIENTEVENTLISTENER_H_
#define SERVER_CLIENTEVENTLISTENER_H_

#include "constants.h"

class ClientEventListener {
public:
  virtual void shoot(int &row, int &col) = 0;
  virtual void sendShips(char ships[ROWS][COLS]) = 0;
};


#endif  // SERVER_CLIENTEVENTLISTENER_H_