CSW = -O3 -Wall
LSW = -lfd

all:
	make pg3

pg3: pg3.o Makefile
	g++ pg3.o -o pg3 $(LSW)

pg3.o: pg3.cc Makefile
	g++ pg3.cc -c -o pg3.o $(CSW)

pg3.cc: pg3.h Makefile
	touch pg3.cc

clean:
	touch Makefile; make
