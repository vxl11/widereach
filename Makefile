CC = gcc

ifdef DEV
  CFLAGS = -g -Wall
else
  CFLAGS = -O3 -DNDEBUG -funroll-loops
endif

INC =
LIBS = -lglpk -lm
BASE = widereach.h helper.h Makefile
OBJ = samples.o sparse_vector.o env.o indexing.o glpk.o params.o callback.o iheur.o ibranch.o
UOBJ = utest.o

.PHONY: all clean

all:
	echo $(CFLAGS)

widereach:	$(OBJ) $(BASE)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $< $(LIBS)

utest:	$(UOBJ) $(OBJ) $(BASE)
	$(CC) $(CFLAGS) -o $@ $(UOBJ) $(OBJ) $(LIBS) -lcunit

itest:	$(OBJ) $(BASE) itest.o
	$(CC) $(CFLAGS) -o $@ $(OBJ) itest.o $(LIBS)

experiment:	$(OBJ) $(BASE) experiment.o
	$(CC) $(CFLAGS) -o $@ $(OBJ) experiment.o $(LIBS)

%.o:	%.c $(BASE)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	rm *.o
