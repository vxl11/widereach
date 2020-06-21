CC = gcc

ifdef DEV
  CFLAGS = -g -Wall
else
  CFLAGS = -O3 -DNDEBUG -funroll-loops
endif

INC =
LIBS = -lglpk -lm
BASE = widereach.h helper.h Makefile
OBJ = samples.o sparse_vector.o env.o indexing.o glpk.o params.o
UOBJ = utest.o

.PHONY: all clean

all:
	echo $(CFLAGS)

widereach:	$(OBJ) $(BASE)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $< $(LIBS)

utest:	$(UOBJ) $(OBJ) $(BASE)
	$(CC) $(CFLAGS) -o $@ $(UOBJ) $(OBJ) $(LIBS) -lcunit

%.o:	%.c $(BASE)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	rm *.o
