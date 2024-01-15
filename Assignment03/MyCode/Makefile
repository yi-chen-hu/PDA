CC = g++
CFLAGS = -std=c++11 -O3 -lm
EXECUTABLE = Lab3

all: $(EXECUTABLE)

$(EXECUTABLE): main.cpp
	$(CC) $(CFLAGS) -o $(EXECUTABLE) main.cpp

clean:
	rm -rf *.o $(EXECUTABLE) 
