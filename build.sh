#!/bin/sh

set -xe

# -pedantic 严格遵守 ISO C 标准，禁用 GNU 扩展
# -Wmissing-prototypes 检查未声明原型的函数
CC=${CC:=/usr/bin/cc}
CFLAGS="-Wall -Wextra -Wswitch-enum -Wmissing-prototypes -pedantic -std=c11"
LIBS=

$CC $CFLAGS -o lasm ./src/lasm.c $LIBS
$CC $CFLAGS -o lvm ./src/lvm.c $LIBS
$CC $CFLAGS -o dlsm ./src/delasm.c $LIBS

for example in `find examples/ -name \*.lasm | sed "s/\.lasm//"`; do
    cpp -P "$example.lasm" > "$example.lasm.pp"
    ./lasm "$example.lasm.pp" "$example.lvm"
    rm -r "$example.lasm.pp"
done
