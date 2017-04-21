CC=gcc
CFLAGS=-I.
DEPS = battleship_server.h ship.h default_ships.h
OBJ = battleship_server.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

battleship_server: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)