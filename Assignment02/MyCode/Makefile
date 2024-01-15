CC = g++
CFLAGS = -std=c++11 -O3 -lm
EXECUTABLE = Floorplan

all: $(EXECUTABLE)

$(EXECUTABLE): main.cpp sp.o
	$(CC) $(CFLAGS) -o $(EXECUTABLE) main.cpp sp.o veb.o block.o

sp.o: sp.cpp sp.h block.o veb.o
	$(CC) $(CFLAGS) -o sp.o -c sp.cpp

block.o: block.cpp block.h
	$(CC) $(CFLAGS) -o block.o -c block.cpp

veb.o: veb.cpp veb.h
	$(CC) $(CFLAGS) -o veb.o -c veb.cpp

clean:
	rm -rf *.o $(EXECUTABLE) 
