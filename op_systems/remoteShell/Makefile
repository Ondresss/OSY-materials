CPPFLAGS=-Wall -pedantic
LIBS=-lpthread
TARGET=server
CPPFILES=$(wildcard *.cpp)
OBJFILES=$(CPPFILES:.cpp=.o)

$(TARGET): $(OBJFILES) 
	g++ -o $@ $^ $(LIBS)

%.o: %.cpp
	g++ -c -o $@ $^ $(CPPFLAGS)

all: $(TARGET)

.PHONY=clean
clean:
	@echo "Cleaning files\n"
	rm -rf *.o $(TARGET)
