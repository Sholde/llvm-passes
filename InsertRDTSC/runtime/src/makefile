LIB_NAME=insertrdtsc

CC=gcc
CFLAGS=-Wall -Wextra -fopenmp
OFLAGS=-O2 -march=native -mtune=native
DFLAGS=-g
LFLAGS=-Wl,-rpath=/home/sholde/dev/master/coa/runtime/ -L/home/sholde/dev/master/coa/runtime/ -l$(LIB_NAME) -fopenmp

TARGET=lib

.PHONY: all clean

all: $(TARGET)

runtime.o: runtime.c
	$(CC) -fPIC -c $(CFLAGS) $(OFLAGS) $(DFLAGS) $< -o $@

runtime.c: runtime.h

lib: runtime.o
	$(CC) -shared $(CFLAGS) $(OFLAGS) $(DFLAGS) $< -o lib$(LIB_NAME).so

clean:
	rm -Rf *~ *.o $(TARGET) *.so
