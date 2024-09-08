
all: server


remoteShell.o: remoteShell.cpp
	g++ -o $@ -c remoteShell.cpp -Wall -pedantic -O2

server.o: server.cpp
	g++ -o $@ -c server.cpp -Wall -pedantic -O2


server: server.o remoteShell.o
	g++ -o $@ server.o remoteShell.o
