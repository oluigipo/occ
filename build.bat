@echo off

clang -g src/main.c -o "occ.exe" -std=c99 -Wall -Wno-logical-op-parentheses -Wno-switch -Wno-parentheses -Wno-unneeded-internal-declaration -fuse-ld=lld %*

@echo on
