PASS_PATH=/home/sholde/dev/software/llvm-project/llvm/build/lib/LLVMInsertRDTSC.so
LIB_PATH=/home/sholde/dev/master/llvm-passes/InsertRDTSC/runtime/src

CC=clang
LFLAGS=-Wl,-rpath=$(LIB_PATH) -L$(LIB_PATH) -linsertrdtsc $(LIB_PATH)/libinsertrdtsc.so

.PHONY: all clean

all: main simple simple_rdtsc omp_main pthread_main link

main: main.c
	$(CC) -emit-llvm main.c -c -o main.bc
	opt -enable-new-pm=0 -load $(PASS_PATH) --insert-rdtsc < main.bc > main_after.bc
	$(CC) -O2 main_after.bc -o main -latomic $(LFLAGS)

simple: simple.c
	$(CC) -emit-llvm simple.c -c -o simple.bc
	opt -enable-new-pm=0 -load $(PASS_PATH) --insert-rdtsc < simple.bc > simple_after.bc
	$(CC) simple_after.bc -o simple -latomic $(LFLAGS)

simple_rdtsc: simple_rdtsc.c
	$(CC) simple_rdtsc.c -o simple_rdtsc

omp_main: omp_main.c
	$(CC) -emit-llvm omp_main.c -c -o omp_main.bc -fopenmp=libomp
	opt -enable-new-pm=0 -load $(PASS_PATH) --insert-rdtsc < omp_main.bc > omp_main_after.bc
	$(CC) omp_main_after.bc -o omp_main -latomic -lomp $(LFLAGS)

pthread_main: pthread_main.c
	$(CC) -emit-llvm pthread_main.c -c -o pthread_main.bc
	opt -enable-new-pm=0 -load $(PASS_PATH) --insert-rdtsc < pthread_main.bc > pthread_main_after.bc
	$(CC) pthread_main_after.bc -o pthread_main -latomic -pthread  $(LFLAGS)

link: link.c
	$(CC) -emit-llvm link.c -c -o link.bc
	opt -enable-new-pm=0 -load $(PASS_PATH) --insert-rdtsc < link.bc > link_after.bc
	$(CC) -O0 link_after.bc -o link -latomic $(LFLAGS)

clean:
	rm -Rf *~ *.bc *.o *.ll main simple simple_rdtsc omp_main pthread_main link output-insert-rdtsc.*
