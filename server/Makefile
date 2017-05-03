CC=gcc
CXXFLAGS=-I. -std=c++0x
DEPS = Server.hpp Client.hpp constants.h Writer.hpp ServerToClientWriter.hpp

all: server client

server: main.o
	g++ -o $@.out $^ $(CFLAGS) 

client: client.o
	g++ -o $@.out $^ $(CFLAGS)

%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)