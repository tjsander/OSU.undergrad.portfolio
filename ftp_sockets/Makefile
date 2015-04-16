# Travis J. Sanders
# CS 372 Introduction to Computer Networks 
# Programming Assignment #2
# Feb 25, 2015

CXX = g++
CXXFLAGS = -std=c++0x

SRCS = ftserver.cpp
PROGS = ftserver

chatserve: ftserver.o
	$(CXX) $(CXXFLAGS) -o ftserver ftserver.o

ftserver.o: ftserver.cpp
	$(CXX) $(CXXFLAGS) -c ftserver.cpp

all: $(PROGS)

clean:
	rm -f $(PROGS) *.o

cleanall:
	rm -f $(PROGS) *.o copy_*
