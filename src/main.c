#include "internal.h"
#include "os.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

internal void* global_arena_memory;
internal uintsize global_arena_size;
internal uintsize global_arena_offset;
internal char global_my_path[MAX_PATH_SIZE];

internal void*
PushMemory(uintsize size)
{
	size = AlignUp(size, 15u);
	
	if (global_arena_offset + size > global_arena_size)
	{
		global_arena_size = Max(size, global_arena_size);
		global_arena_offset = 0;
		global_arena_memory = malloc(global_arena_size);
	}
	
	void* result = (uint8*)global_arena_memory + global_arena_offset;
	global_arena_offset += size;
	
	memset(result, 0, size);
	
	return result;
}

internal void
Print(const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

internal void
PrintVarargs(const char* fmt, va_list args)
{
	vfprintf(stderr, fmt, args);
}

#include "lang_c.c"

int main(int argc, char* argv[]) {
	global_arena_size = Megabytes(32);
	global_arena_offset = 0;
	global_arena_memory = malloc(global_arena_size);
	
	OS_GetMyPath(global_my_path, sizeof global_my_path);
	
	// Testing
	LangC_Init(argc, argv);
	
	LangC_ParseFile("tests/pp-test.c");
	
	return 0;
}

// NOTE(ljre): Let it stay here so we can avoid weird OS's headers macros and declarations
#define OS_H_IMPLEMENTATION
#include "os.h"
