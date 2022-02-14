all: main simple simple_rdtsc omp_main pthread_main link

main: main.c
	clang -emit-llvm main.c -c -o main.bc
	opt -enable-new-pm=0 -load /home/sholde/dev/software/llvm-project/llvm/build/lib/LLVMInsertRDTSC.so --insert-rdtsc < main.bc > main_after.bc
	clang -O2 main_after.bc -o main -latomic

simple: simple.c
	clang -emit-llvm simple.c -c -o simple.bc
	opt -enable-new-pm=0 -load /home/sholde/dev/software/llvm-project/llvm/build/lib/LLVMInsertRDTSC.so --insert-rdtsc < simple.bc > simple_after.bc
	clang simple_after.bc -o simple -latomic

simple_rdtsc: simple_rdtsc.c
	clang simple_rdtsc.c -o simple_rdtsc

omp_main: omp_main.c
	clang -emit-llvm omp_main.c -c -o omp_main.bc -fopenmp=libomp
	opt -enable-new-pm=0 -load /home/sholde/dev/software/llvm-project/llvm/build/lib/LLVMInsertRDTSC.so --insert-rdtsc < omp_main.bc > omp_main_after.bc
	clang omp_main_after.bc -o omp_main -latomic -lomp

pthread_main: pthread_main.c
	clang -emit-llvm pthread_main.c -c -o pthread_main.bc
	opt -enable-new-pm=0 -load /home/sholde/dev/software/llvm-project/llvm/build/lib/LLVMInsertRDTSC.so --insert-rdtsc < pthread_main.bc > pthread_main_after.bc
	clang pthread_main_after.bc -o pthread_main -latomic -pthread

link: link.c
	clang -emit-llvm link.c -c -o link.bc
	opt -enable-new-pm=0 -load /home/sholde/dev/software/llvm-project/llvm/build/lib/LLVMInsertRDTSC.so --insert-rdtsc < link.bc > link_after.bc
	clang -O2 link_after.bc -o link -latomic -llazyprobe ./liblazyprobe.so -L/home/sholde/dev/master/llvm-passes/tests/InsertRDTSC -Wl,-rpath=/home/sholde/dev/master/llvm-passes/tests/InsertRDTSC

clean:
	rm -Rf *~ *.bc *.o *.ll main simple simple_rdtsc omp_main pthread_main link
