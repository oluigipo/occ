@echo off

set CFLAGS=-std=c99 -Wall -Wno-logical-op-parentheses -Wno-switch -D_CRT_SECURE_NO_WARNINGS
set LDFLAGS=-fuse-ld=lld-link

clang -g src/main.c -o "occ.exe" %CFLAGS% %LDFLAGS%

@echo on
