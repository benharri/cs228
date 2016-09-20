CSW = -O3 -Wall
LSW = -lfd

all:
	make client

client: client.o Makefile
	g++ client.o -o client $(LSW)

client.o: client.cc Makefile
	g++ client.cc -c -o client.o $(CSW)

client.cc: client.h Makefile
	touch client.cc

clean:
	touch Makefile; make

