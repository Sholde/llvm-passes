all: main

main:
	clang -emit-llvm main.c -c -o main.bc
	opt -enable-new-pm=0 -load /home/sholde/dev/software/llvm-project/llvm/build/lib/LLVMInsertRDTSC.so --insert-rdtsc < main.bc > after.bc
	clang after.bc -o main

clean:
	rm -Rf *~ *.bc *.o *.ll main
