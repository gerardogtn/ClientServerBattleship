CC=gcc
CFLAGS=-I. -std=c++0x
DEPS = Server.hpp Client.hpp constants.h Writer.hpp ServerToClientWriter.hpp

all: server client

server: main.o
	gcc -o $@.out $^ $(CFLAGS) 

client: client.o
	gcc -o $@.out $^ $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)