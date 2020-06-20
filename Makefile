CC = gcc

ifdef DEV
  CFLAGS = -g -Wall
else
  CFLAGS = -O3 -DNDEBUG -funroll-loops
endif

INC =
LIBS = -lgplk -lm
BASE = widereach.h helper.h Makefile
OBJ = samples.o

.PHONY: all clean

all:
	echo $(CFLAGS)

widereach:	$(OBJ) $(BASE)
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

utest:	$(UOBJ) $(OBJ) $(BASE) utest.h
	$(CC) $(CFLAGS) -o $@ $< $(LIBS)

%.o:	%.c $(BASE)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	rm *.o
