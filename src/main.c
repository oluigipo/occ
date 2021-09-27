#include "internal.h"
#include "os.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define COMMIT_PAGE Megabytes(32)

internal uint8* global_arena_memory; // NOTE(ljre): not 'void*' so we can do arithmetic
internal uintsize global_arena_total_size;
internal uintsize global_arena_commited_size;
internal uintsize global_arena_offset;
internal const char* global_my_path;

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
Panic(const char* fmt)
{
	fputs(fmt, stderr);
	exit(-1);
}

internal void*
PushMemory(uintsize size)
{
	size = AlignUp(size, 7u);
	
	while (global_arena_offset + size > global_arena_commited_size)
	{
		if (global_arena_commited_size >= global_arena_total_size ||
			!OS_CommitMemory(global_arena_memory + global_arena_commited_size, COMMIT_PAGE))
		{
			Panic("OCC FATAL ERROR: Out of memory!\n");
		}
		
		global_arena_commited_size += COMMIT_PAGE;
	}
	
	void* result = global_arena_memory + global_arena_offset;
	global_arena_offset += size;
	
	memset(result, 0, size);
	
	return result;
}

#include "lang_c.c"

int main(int argc, char* argv[])
{
	global_arena_total_size = Gigabytes(100);
	global_arena_offset = 0;
	
	do
		global_arena_memory = OS_ReserveMemory(global_arena_total_size);
	while (!global_arena_memory && COMMIT_PAGE <= (global_arena_total_size >>= 1));
	
	if (!global_arena_memory)
		Panic("OCC FATAL ERROR: Could not reserve at least 32MiB.\n");
	
	global_my_path = OS_GetMyPath();
	
	// Testing
	LangC_Run(argc, (const char**)argv);
	
	Print("\nMax used memory: %zu bytes.\n", global_arena_offset);
	
	return 0;
}

// NOTE(ljre): Let it stay here so we can avoid weird OS's headers macros and declarations
#define OS_H_IMPLEMENTATION
#include "os.h"
