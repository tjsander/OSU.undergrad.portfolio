#
# Travis J. Sanders
# sandetra@onid.oregonstate.edu
# CS344-400
# Homework #5
#

CC=gcc
DEBUG=-g

CFLAGS=$(DEBUG)
CFLAGS+=-pedantic-errors
CFLAGS+=-lm
CFLAGS+=-Wall
CFLAGS+=-Wshadow
CFLAGS+=-Wunreachable-code
CFLAGS+=-Wredundant-decls
CFLAGS+=-Wmissing-declarations
CFLAGS+=-Wold-style-definition
CFLAGS+=-Wmissing-prototypes
CFLAGS+=-Wdeclaration-after-statement
CFLAGS+=-std=c99

PROGS=prime_pthread prime_mproc

all: $(PROGS)

prime_pthread: prime_pthread.o
	$(CC) $(CFLAGS) -pthread -o prime_pthread prime_pthread.o

prime_pthread.o: prime_pthread.c
	$(CC) $(CFLAGS) -pthread -c prime_pthread.c

prime_mproc: prime_mproc.o
	$(CC) $(CFLAGS) -lrt -o prime_mproc prime_mproc.o

prime_mproc.o: prime_mproc.c
	$(CC) $(CFLAGS) -lrt -c prime_mproc.c

clean:
	rm -f $(PROGS) *.o *~
