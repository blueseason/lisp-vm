# -pedantic 严格遵守 ISO C 标准，禁用 GNU 扩展
# -Wmissing-prototypes 检查未声明原型的函数
# -Wconversion 启用 隐式类型转换警告，帮助捕获可能丢失数据精度的隐式转换
# -fno-strict-aliasing 禁用严格别名优化规则，允许通过不同类型的指针访问同一内存
# C/C++ 标准规定：通过不兼容类型的指针访问同一对象是未定义行为（UB）。
CFLAGS= -Wall -Wextra -std=c11 -pedantic -Wswitch-enum -Wmissing-prototypes -Wconversion -fno-strict-aliasing
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

