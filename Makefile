C = gcc

CFLAGS = -g -Wall
CFLAGS = -O3 -funroll-loops

.PHONY: all clean

all:


clean:
	rm *.o
