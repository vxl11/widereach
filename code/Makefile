CC = gcc

ifdef DEV
  CFLAGS = -g -Wall
else
  CFLAGS = -O3 -DNDEBUG -funroll-loops
endif

INC =
LIBS = -lglpk -lm
BASE = widereach.h helper.h Makefile
OBJ = samples.o sparse_vector.o env.o indexing.o glpk.o params.o callback.o \
      iheur.o ibranch.o iselect.o irowgen.o solution_data.o ibingo.o \
      traversal.o signature.o children.o paths.o random.o
UOBJ = utest.o

.PHONY: all clean dependencies

all:
	echo $(CFLAGS)

utest:	$(UOBJ) $(OBJ) $(BASE)
	$(CC) $(CFLAGS) -o $@ $(UOBJ) $(OBJ) $(LIBS) -lcunit

itest:	$(OBJ) $(BASE) itest.o
	$(CC) $(CFLAGS) -o $@ $(OBJ) itest.o $(LIBS)

experiment:	$(OBJ) $(BASE) experiment.o
	$(CC) $(CFLAGS) -o $@ $(OBJ) experiment.o $(LIBS)

hyperrnd:	$(OBJ) $(BASE) hyperrnd.o
	$(CC) $(CFLAGS) -o $@ $(OBJ) hyperrnd.o $(LIBS)

dependencies:
	sudo apt install libcunit1-dev glpk-doc libglpk-dev

%.o:	%.c $(BASE)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<

clean:
	rm *.o utest itest experiment experiment-* hyperrnd hyperrnd-*
