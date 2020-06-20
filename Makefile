CC = gcc

ifdef DEV
  CFLAGS = -g -Wall
else
  CFLAGS = -O3 -DNDEBUG -funroll-loops
endif

INC =
LIBS = -lgplk -lm

.PHONY: all clean

all:
	echo $(CFLAGS)

widereach:	
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

*.o*.c: widereach.h
	$(CC) $(CFLAGS) $(INC) -o $@ $<

clean:
	rm *.o
