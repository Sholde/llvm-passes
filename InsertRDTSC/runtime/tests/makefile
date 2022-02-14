LIB_NAME=insertrdtsc
LIB_PATH=/home/sholde/dev/master/llvm-passes/src/InsertRDTSC/runtime/src

CC=gcc
CFLAGS=-Wall -Wextra -fopenmp
OFLAGS=-O2 -march=native -mtune=native
DFLAGS=-g
IFLAGS=-I$(LIB_PATH)
LFLAGS=-Wl,-rpath=$(LIB_PATH) -L$(LIB_PATH) -l$(LIB_NAME) -fopenmp

TARGET=main

.PHONY: all clean

all: $(TARGET)

$(TARGET): main.o
	$(CC) $(LFLAGS) $< -o $@ $(LIB_PATH)/lib$(LIB_NAME).so

main.o: main.c
	$(CC) -c $(CFLAGS) $(OFLAGS) $(DFLAGS) $(IFLAGS) $< -o $@

clean:
	rm -Rf *~ *.o $(TARGET) output-insert-rdtsc.*
