#define _CRT_SECURE_NO_WARNINGS

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "internal.h"

internal Arena* global_arena;

internal void
Print(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	char* mem = Arena_End(global_arena);
	uintsize len = Arena_VPrintf(global_arena, fmt, args);
	va_end(args);
	
	fwrite(mem, 1, len, stdout);
	Arena_Pop(global_arena, mem);
}

internal void
PrintVarargs(const char* fmt, va_list args)
{
	char* mem = Arena_End(global_arena);
	uintsize len = Arena_VPrintf(global_arena, fmt, args);
	
	fwrite(mem, 1, len, stdout);
	Arena_Pop(global_arena, mem);
}

internal void
Panic(const char* str)
{
	fputs(str, stderr);
	DebugBreak_();
	OS_Exit(-1);
}

internal void*
PushMemory(uintsize size)
{ return Arena_Push(global_arena, size); }

#include "ir.c"
#include "lang_c.c"

int main(int argc, char* argv[])
{
#ifdef TRACY_ENABLE
	TracyCFrameMark;
#endif
	
	// NOTE(ljre): Setup basic stuff
	global_arena = Arena_Create(Gigabytes(4));
	global_my_path.data = Arena_End(global_arena);
	global_my_path.size = OS_GetMyPath(global_arena);
	
	// Testing
	int32 result = C_Main(argc, (const char**)argv);
	
	return result;
}

// NOTE(ljre): Let it stay here so we can avoid weird OS's headers macros and declarations
#define OS_H_IMPLEMENTATION
#include "os.h"
