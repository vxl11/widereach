CC = gcc

ifdef OPT
  CFLAGS = -O3 -DNDEBUG -funroll-loops
else
  CFLAGS = -g -Wall
endif

.PHONY: all clean

all:
	echo $(CFLAGS)


clean:
	rm *.o
