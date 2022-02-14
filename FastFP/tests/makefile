CC=clang
OFLAGS=-O0

PATH_TO_LIB=/home/sholde/dev/software/llvm-project/llvm/build/lib/LLVMFastFP.so

.PHONY: all clean

all: double double_pass main main_pass

double: double.c
	$(CC) $(OFLAGS) $< -o $@

double_pass: double.c
	$(CC) $(OFLAGS) -emit-llvm $< -c -o $@_before.bc
	opt -enable-new-pm=0 -load $(PATH_TO_LIB) --fast-fp < $@_before.bc > $@_after.bc
	$(CC) $(OFLAGS) $@_after.bc -o $@

main: main.c
	$(CC) $(OFLAGS) $< -o $@

main_pass: main.c
	$(CC) $(OFLAGS) -emit-llvm $< -c -o $@_before.bc
	opt -enable-new-pm=0 -load $(PATH_TO_LIB) --fast-fp < $@_before.bc > $@_after.bc
	$(CC) $(OFLAGS) $@_after.bc -o $@

clean:
	rm -Rf *~ *.o *.bc *.ll double double_pass main main_pass
