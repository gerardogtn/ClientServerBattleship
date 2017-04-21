CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-g -std=c++0x
LDFLAGS=-g
LDLIBS=-lgmock

SRCS=main.cpp board/BoardTest.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: test

test: $(OBJS)
	$(CXX) $(LDFLAGS) -o test $(OBJS) $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	$(RM) ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

distclean: clean
	$(RM) *~ .depend

include .depend