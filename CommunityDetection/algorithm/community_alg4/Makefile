CC = g++
CFLAGS = -Wall -O3 -fopenmp
LDFLAGS = -lgomp

all: cluster

cluster.o: cluster.cpp cluster.hpp util.hpp
	$(CC) $(CFLAGS) -c cluster.cpp

util.o: util.cpp util.hpp
	$(CC) $(CFLAGS) -c util.cpp

munkres/src/munkres.o:
	cd munkres/src && $(CC) $(CFLAGS) -c munkres.cpp

cluster: main.cpp cluster.o util.o munkres/src/munkres.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o cluster main.cpp cluster.o util.o munkres/src/munkres.o qpbo/*.cpp

clean:
	rm cluster cluster.o util.o munkres/src/munkres.o
