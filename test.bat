@echo off
cls
call build.bat
@echo on
occ tests/pp-test.c -o program.exe