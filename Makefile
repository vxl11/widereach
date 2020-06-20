CC = gcc

ifdef DEV
  CFLAGS = -g -Wall
else
  CFLAGS = -O3 -DNDEBUG -funroll-loops
endif

INC =
LIBS = -lgplk -lm
BASE = widereach.h Makefile
OBJ = samples.o

.PHONY: all clean

all:
	echo $(CFLAGS)

widereach:	$(OBJ) $(BASE)
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

utest:	$(UOBJ) $(OBJ) $(BASE) utest.h
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

*.o*.c: $(BASE)
	$(CC) $(CFLAGS) $(INC) -o $@ $<

clean:
	rm *.o
