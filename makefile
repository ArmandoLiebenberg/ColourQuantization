CC = g++
CFLAGS = -Wall -std=c++11 -fopenmp -lgomp -lm -ldl

TARGET = main

all: $(TARGET)

$(TARGET): src/$(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) src/$(TARGET).cpp

clean:
	$(RM) $(TARGET)
