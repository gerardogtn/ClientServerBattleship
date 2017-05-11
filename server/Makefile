# Alexis Matuk - A01021143
# Diego Vazquez - A01168095
# Gerardo Garcia Teruel - A01018057
CC=g++
CFLAGS=-I. -std=c++0x -g
DEPS = Server.hpp Client.hpp constants.h Writer.hpp ServerToClientWriter.hpp

all: server client

server: main.o
	g++ -o $@.out $^ $(CFLAGS) 

client: client.o
	g++ -o $@.out $^ $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean: 
	rm -f *.o