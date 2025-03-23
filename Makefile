CFLAGS=-Wall -Wextra -std=c11 -pedantic -Wswitch-enum
LIBS=
PHONY: all
all: lasm lvm

lsm: lasm.c lvm.c
	$(CC) $(CFLAGS) -o lasm lasm.c $(LIBS)

lvm: lvm.c main.c
	$(CC) $(CFLAGS) -o lvm main.c $(LIBS)

.PHONY: examples
examples: ./examples/fib.lvm

./examples/fib.lvm: ./examples/fib.lasm
	./lasm ./examples/fib.lasm ./examples/fib.lvm
