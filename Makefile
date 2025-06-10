# -pedantic 严格遵守 ISO C 标准，禁用 GNU 扩展
# -Wmissing-prototypes 检查未声明原型的函数
CFLAGS=-Wall -Wextra -std=c11 -pedantic -Wswitch-enum -Wmissing-prototypes
LIBS=
EXAMPLES = $(patsubst %.lasm,%.lvm,$(wildcard ./examples/*.lasm))
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
examples: $(EXAMPLES)

%.lvm: %.lasm lasm
	./lasm $< $@

