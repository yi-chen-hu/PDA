CC = g++
CFLAGS = -std=c++14 -O3 -lm
EXECUTABLE = Lab4

all: $(EXECUTABLE)

$(EXECUTABLE): main.cpp
	$(CC) $(CFLAGS) -o $(EXECUTABLE) main.cpp

clean:
	rm -rf *.o $(EXECUTABLE) 
