#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"
#include "os.h"

#define COMMIT_PAGE Megabytes(32)

internal Arena* global_arena;
internal String global_my_path;

internal void
Print(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stdout, fmt, args);
	va_end(args);
}

internal void
PrintVarargs(const char* fmt, va_list args)
{
	vfprintf(stdout, fmt, args);
}

internal void
Panic(const char* str)
{
	fputs(str, stderr);
	exit(-1);
}

internal void*
PushMemory(uintsize size)
{
	return Arena_Push(global_arena, size);
}

#include "lang_c.c"

int main(int argc, char* argv[])
{
	// NOTE(ljre): Setup global arena
	uintsize desired_size = Gigabytes(100);
	
	do
		global_arena = Arena_Create(desired_size);
	while (!global_arena && Arena_PAGE_SIZE <= (desired_size >>= 1));
	
	if (!global_arena)
		Panic("OCC FATAL ERROR: Could not reserve at least 32MiB.\n");
	
	global_my_path = OS_GetMyPath();
	
	// Testing
	int32 result = LangC_Main(argc, (const char**)argv);
	
	return result;
}

// NOTE(ljre): Let it stay here so we can avoid weird OS's headers macros and declarations
#define OS_H_IMPLEMENTATION
#include "os.h"
