all: main simple simple_rdtsc

main: main.c
	clang -emit-llvm main.c -c -o main.bc
	opt -enable-new-pm=0 -load /home/sholde/dev/software/llvm-project/llvm/build/lib/LLVMInsertRDTSC.so --insert-rdtsc < main.bc > main_after.bc
	clang main_after.bc -o main

simple: simple.c
	clang -emit-llvm simple.c -c -o simple.bc
	opt -enable-new-pm=0 -load /home/sholde/dev/software/llvm-project/llvm/build/lib/LLVMInsertRDTSC.so --insert-rdtsc < simple.bc > simple_after.bc
	clang simple_after.bc -o simple

simple_rdtsc: simple_rdtsc.c
	clang simple_rdtsc.c -o simple_rdtsc

clean:
	rm -Rf *~ *.bc *.o *.ll main simple simple_rdtsc
