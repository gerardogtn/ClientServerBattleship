// Server.hpp
// Alexis Matuk - A01021143
// Diego Vazquez - A01168095
// Gerardo Garcia Teruel - A01018057
#ifndef SERVER_CLIENTEVENTLISTENER_H_
#define SERVER_CLIENTEVENTLISTENER_H_

#include "constants.h"

/** An abstract class that allows a Client to communicate with an implementation
 *  or an implementation to communicate to a Client.
 */
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