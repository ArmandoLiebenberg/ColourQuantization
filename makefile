CC = g++
CFLAGS = -g -Wall -pg -std=c++11 -O3 -mavx

TARGET = main

all: $(TARGET)

$(TARGET): src/$(TARGET).cpp
	$(CC) $(CFLAGS) -o $(TARGET) src/$(TARGET).cpp

clean:
	$(RM) $(TARGET)
