CC = gcc

ifdef DEV
  CFLAGS = -g -Wall
else
  CFLAGS = -O3 -DNDEBUG -funroll-loops
endif

INC =
LIBS = -lgplk -lm
OBJ = samples.o

.PHONY: all clean

all:
	echo $(CFLAGS)

widereach:	$(OBJ) widereach.h Makefile
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

*.o*.c: widereach.h Makefile
	$(CC) $(CFLAGS) $(INC) -o $@ $<

clean:
	rm *.o
