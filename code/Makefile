CC = gcc

ifdef DEV
  CFLAGS = -g -Wall -fprofile-arcs -ftest-coverage
else
  CFLAGS = -O3 -DNDEBUG -funroll-loops
endif

INC =
LIBS = -lglpk -lm
BASE = widereach.h helper.h Makefile
OBJ = samples.o sparse_vector.o env.o indexing.o glpk.o params.o callback.o \
      iheur.o ibranch.o iselect.o irowgen.o solution_data.o ibingo.o \
      traversal.o signature.o children.o paths.o random.o hyperplane.o \
      obstruction.o interdiction.o cuts.o icutgen.o readsamples.o clusters.o exec.o \
      simplex.o
UOBJ = utest.o

.PHONY: all coverage clean dependencies 

all:
	echo $(CFLAGS)

utest:	$(UOBJ) $(OBJ) $(BASE)
	rm --force *.gcno *gcda;\
	$(CC) $(CFLAGS) -o $@ $(UOBJ) $(OBJ) $(LIBS) -lcunit -fopenmp

coverage:	utest
	utest; gcovr
	
itest:	$(OBJ) $(BASE) itest.o
	$(CC) $(CFLAGS) -o $@ $(OBJ) itest.o $(LIBS) -fopenmp

experiment:	$(OBJ) $(BASE) experiment.o
	$(CC) $(CFLAGS) -o $@ $(OBJ) experiment.o $(LIBS) -fopenmp

hyperrnd:	$(OBJ) $(BASE) hyperrnd.o
	$(CC) $(CFLAGS) -o $@ $(OBJ) hyperrnd.o $(LIBS) -fopenmp

dependencies:
	sudo apt install libcunit1-dev glpk-doc libglpk-dev

%.o:	%.c $(BASE)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $< -fopenmp

clean:
	rm --force *.o *.gcno *gcda utest itest experiment experiment-* hyperrnd hyperrnd-*
