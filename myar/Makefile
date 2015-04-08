# Travis J. Sanders
# sandetra@onid.oregonstate.edu
# CS344-400
# Homework #3

CC=gcc
DEBUG=-g
CFLAGS=$(DEBUG) -Wall -std=c99 -pedantic-errors

PROGS=sig_demo myar

FILES=1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt 
TXT=$(FILES) ar-ctoc.txt myar-ctoc.txt ar-vtoc.txt myar-vtoc.txt

TESTQ=testq12345 testq135 testq24 testq1234512
TESTT=testt12345 testt135 testt24 
TESTV=testv12345 testv135 testv24 
TESTX=testxall testx12345
TESTD=testdall
TESTA=testAbig

all: $(PROGS)

tests: $(PROGS) $(TESTQ) $(TESTT) $(TESTV)
alltests: $(PROGS) $(TESTX) $(TESTQ) $(TESTT) $(TESTV) $(TESTD) $(TESTA)

myar: myar.o
	$(CC) $(CFLAGS) -o myar myar.o

myar.o: myar.c
	$(CC) $(CFLAGS) -c myar.c

sig_demo: sig_demo.o
	$(CC) $(CFLAGS) -o sig_demo sig_demo.o

sig_demo.o: sig_demo.c
	$(CC) $(CFLAGS) -c sig_demo.c

testq12345:
	rm -f ar12345.ar
	rm -f myar12345.ar
	ar q ar12345.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	myar -q myar12345.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	diff ar12345.ar myar12345.ar

testq1234512:
	rm -f ar1234512.ar
	rm -f myar1234512.ar
	ar q ar1234512.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	myar -q myar1234512.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	ar q ar1234512.ar 1-s.txt 2-s.txt
	myar -q myar1234512.ar 1-s.txt 2-s.txt
	diff ar1234512.ar myar1234512.ar

testq135:
	rm -f ar135.ar
	rm -f myar135.ar
	ar q ar135.ar 1-s.txt 3-s.txt 5-s.txt
	myar -q myar135.ar 1-s.txt 3-s.txt 5-s.txt
	diff ar135.ar myar135.ar

testq24:
	rm -f ar24.ar
	rm -f myar24.ar
	ar q ar24.ar 2-s.txt 4-s.txt
	myar -q myar24.ar 2-s.txt 4-s.txt
	diff ar24.ar ar24.ar

testt12345:
	rm -f ar12345.ar
	ar q ar12345.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	ar t ar12345.ar > ar-ctoc.txt
	myar -t ar12345.ar > myar-ctoc.txt
	diff ar-ctoc.txt myar-ctoc.txt

testt135:
	rm -f ar12345.ar
	ar q ar12345.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	ar t ar12345.ar > ar-ctoc.txt
	myar -t ar12345.ar > myar-ctoc.txt
	diff ar-ctoc.txt myar-ctoc.txt

testt24:
	rm -f ar24.ar
	ar q ar24.ar 2-s.txt 4-s.txt
	ar t ar24.ar > ar-ctoc.txt
	myar -t ar24.ar > myar-ctoc.txt
	diff ar-ctoc.txt myar-ctoc.txt

testv12345:
	rm -f ar12345.ar
	ar q ar12345.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	ar tv ar12345.ar > ar-vtoc.txt
	myar -v ar12345.ar > myar-vtoc.txt
	diff ar-vtoc.txt myar-vtoc.txt

testv135:
	rm -f ar12345.ar
	ar q ar12345.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	ar tv ar12345.ar > ar-vtoc.txt
	myar -v ar12345.ar > myar-vtoc.txt
	diff ar-vtoc.txt myar-vtoc.txt

testv24:
	rm -f ar24.ar
	ar q ar24.ar 2-s.txt 4-s.txt
	ar tv ar24.ar > ar-vtoc.txt
	myar -v ar24.ar > myar-vtoc.txt
	diff ar-vtoc.txt myar-vtoc.txt

testx12345:
	rm -f myar_testx.ar ar_testx.ar
	ar -xo junk.a 1-s.txt 2-s.txt
	ar q ar_testx.ar 1-s.txt 2-s.txt
	myar -x junk.a 2-s.txt 1-s.txt
	myar -q myar_testx.ar 1-s.txt 2-s.txt
	diff myar_testx.ar ar_testx.ar

testxall:
	rm -f $(TXT) testx1.ar testx2.ar
	ar -xo junk.a
	ar q testx1.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	rm -f $(TXT)
	myar -x junk.a
	myar -q testx2.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	diff testx1.ar testx2.ar

testdall:
	rm -f testx.ar testx_my.ar
	ar q testx.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	ar q testx_my.ar 1-s.txt 2-s.txt 3-s.txt 4-s.txt 5-s.txt
	myar -d testx_my.ar 2-s.txt 4-s.txt
	ar d testx.ar 2-s.txt 4-s.txt
	diff testx.ar testx_my.ar
	myar -d testx_my.ar 3-s.txt
	ar d testx.ar 3-s.txt
	diff testx.ar testx_my.ar

testAbig:
	myar -A bigtest.ar
	myar -v bigtest.ar

clean:
	rm -f $(PROGS) *.o *~ *.ar 

cleanall:
	rm -f $(PROGS) *.o *~ *.ar $(TXT)