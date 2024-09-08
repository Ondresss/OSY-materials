webServer.o: webServer.cpp
	g++ -c -Wall -pedantic -lpthread $^ -o $@

webServer: webServer.o
	g++ $^ -o $@ 

all: webServer
