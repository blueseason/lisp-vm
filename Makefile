# -pedantic 严格遵守 ISO C 标准，禁用 GNU 扩展
# -Wmissing-prototypes 检查未声明原型的函数
CFLAGS=-Wall -Wextra -std=c11 -pedantic -Wswitch-enum -Wmissing-prototypes
LIBS=
PHONY: all
all: lasm lvm dlsm

lasm: lasm.c lvm.h
	$(CC) $(CFLAGS) -o lasm lasm.c $(LIBS)

lvm: lvm.h lvm.c
	$(CC) $(CFLAGS) -o lvm lvm.c $(LIBS)

dlsm: lvm.h delasm.c
	$(CC) $(CFLAGS) -o dlsm delasm.c $(LIBS)

clean:
	rm lasm lvm dlsm

.PHONY: examples
examples: ./examples/fib.lvm

./examples/fib.lvm: ./examples/fib.lasm
	./lasm ./examples/fib.lasm ./examples/fib.lvm
