# -pedantic 严格遵守 ISO C 标准，禁用 GNU 扩展
# -Wmissing-prototypes 检查未声明原型的函数
CFLAGS= -Wall -Wextra -std=c11 -pedantic -Wswitch-enum -Wmissing-prototypes
LIBS=
RM?=	rm -f
EXAMPLES!=	find examples/ -name \*.lasm | sed "s/\.lasm/\.lvm/"
BINARIES=	lasm \
		lvm  \
		dlsm

.SUFFIXES: .lasm .lvm

.lasm.lvm:
	./lasm $< $@

PHONY: all
all: $(BINARIES)

lasm: lasm.c lvm.h
	$(CC) $(CFLAGS) -o lasm lasm.c $(LIBS)

lvm: lvm.h lvm.c
	$(CC) $(CFLAGS) -o lvm lvm.c $(LIBS)

dlsm: lvm.h delasm.c
	$(CC) $(CFLAGS) -o dlsm delasm.c $(LIBS)

.PHONY: examples
examples: lasm $(EXAMPLES)

clean:
	${RM} ${EXAMPLES}
	${RM} ${BINARIES}

