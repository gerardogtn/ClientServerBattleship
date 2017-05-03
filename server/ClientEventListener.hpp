#ifndef SERVER_CLIENTEVENTLISTENER_H_
#define SERVER_CLIENTEVENTLISTENER_H_

#include "constants.h"

class ClientEventListener {
public:
  virtual void connected() = 0;
  virtual void ready() = 0;
  virtual void shoot(int &row, int &col) = 0;
  virtual void sendShips(char ships[ROWS][COLS]) = 0;
  virtual void lost() = 0;
  virtual void won() = 0;
  virtual void onAttack() = 0;
  virtual void onDefend() = 0;
};


#endif  // SERVER_CLIENTEVENTLISTENER_H_