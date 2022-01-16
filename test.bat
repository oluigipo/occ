@echo off
cls
call build.bat
@echo off

IF "%ERRORLEVEL%" EQU "0" (
	occ tests/pp-test.c -o program.exe
)

@echo on
