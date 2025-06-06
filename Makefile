CFLAGS=-Wall -Wextra -std=c11 -pedantic -Wswitch-enum
LIBS=
PHONY: all
all: lasm lvm dlsm

lsm: lasm.c lvm.h
	$(CC) $(CFLAGS) -o lasm lasm.c $(LIBS)

lvm: lvm.h lvm.c
	$(CC) $(CFLAGS) -o lvm lvm.c $(LIBS)

dlsm: lvm.h delasm.c
	$(CC) $(CFLAGS) -o lvm delasm.c $(LIBS)

.PHONY: examples
examples: ./examples/fib.lvm

./examples/fib.lvm: ./examples/fib.lasm
	./lasm ./examples/fib.lasm ./examples/fib.lvm
