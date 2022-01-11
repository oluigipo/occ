@echo off

set WARNINGS=-Wall -Wno-logical-op-parentheses -Wno-switch -Wno-parentheses -Wno-unneeded-internal-declaration
set WARNINGS=%WARNINGS% -Wno-unused-function
clang -g src/main.c -o "occ.exe" -static -std=c99 %WARNINGS% -fuse-ld=lld %*

@echo on
