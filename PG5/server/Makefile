CSW = -O3 -Wall
LSW = -lfd

all:
	make server

server: server.o Makefile
	g++ server.o -o server $(LSW)

server.o: server.cc Makefile
	g++ server.cc -c -o server.o $(CSW)

server.cc: server.h Makefile
	touch server.cc

clean:
	touch Makefile; make
