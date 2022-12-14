CC = gcc

ifdef DEV
  CFLAGS = -g -Wall -fprofile-arcs -ftest-coverage
else
  CFLAGS = -O3 -DNDEBUG -funroll-loops
endif

INC = $(GUROBI_HOME)/include/
LIBS = -L$(GUROBI_HOME)/lib -lgurobi95 -lglpk -lm
BASE = widereach.h helper.h general.h glpk.h Makefile
OBJ = samples.o sparse_vector.o env.o indexing.o glpk.o params.o callback.o \
      iheur.o ibranch.o iselect.o irowgen.o solution_data.o ibingo.o \
      traversal.o signature.o children.o paths.o random.o hyperplane.o \
      obstruction.o interdiction.o cuts.o icutgen.o readsamples.o clusters.o \
      exec.o simplex.o \
      gurobi_exec.o gurobi.o
UOBJ = utest.o

.PHONY: all coverage clean dependencies valgrind cleancoverage

all:
	echo $(CFLAGS); echo $(INC)

utest:	$(UOBJ) $(OBJ) $(BASE)
	rm --force *.gcno *gcda;\
	$(CC) $(CFLAGS) -o $@ $(UOBJ) $(OBJ) $(LIBS) -lcunit -fopenmp

coverage:	utest
	utest; gcovr
	
itest:	$(OBJ) $(BASE) itest.o
	$(CC) $(CFLAGS) -o $@ $(OBJ) itest.o $(LIBS) -fopenmp

experiment:	$(OBJ) $(BASE) experiment.o cleancoverage
	$(CC) $(CFLAGS) -o $@ $(OBJ) experiment.o $(LIBS) -fopenmp

hyperrnd:	$(OBJ) $(BASE) hyperrnd.o
	$(CC) $(CFLAGS) -o $@ $(OBJ) hyperrnd.o $(LIBS) -fopenmp
	
valgrind:	utest
	csh -c 'valgrind --tool=memcheck --leak-check=yes utest | & less -p Summary '

dependencies:
	sudo apt install libcunit1-dev glpk-doc libglpk-dev

%.o:	%.c $(BASE)
	$(CC) $(CFLAGS) -I $(INC) -c -o $@ $< -fopenmp
	
cleancoverage:
	rm --force *.gcno *gcda

clean:	cleancoverage
	rm --force *.o utest itest experiment experiment-* hyperrnd hyperrnd-*
