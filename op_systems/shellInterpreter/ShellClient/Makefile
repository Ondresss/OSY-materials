COMPILER=g++

.PHONY=all

client.o: client.cpp
	$(COMPILER) -o $@ -c client.cpp -Wall -pedantic -O2

client: client.o
	$(COMPILER) -o $@ client.o

all: client
