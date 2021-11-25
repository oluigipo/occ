#ifndef INTERNAL_ARENA_H
#define INTERNAL_ARENA_H

#define Arena_PAGE_SIZE Megabytes(32)

struct alignas(16) Arena typedef Arena;
struct Arena
{
	uintsize reserved;
	uintsize commited;
	uintsize offset;
	Arena* next;
	uint8 memory[];
};

internal Arena*
Arena_Create(uintsize size)
{
	Trace();
	
	size = AlignUp(size, Arena_PAGE_SIZE-1);
	Assert(size > 0);
	
	Arena* arena = OS_ReserveMemory(size + sizeof *arena);
	if (arena)
	{
		OS_CommitMemory(arena, sizeof *arena + Arena_PAGE_SIZE);
		
		arena->reserved = size;
		arena->commited = Arena_PAGE_SIZE;
		arena->offset = 0;
		arena->next = NULL;
	}
	
	return arena;
}

internal void
Arena_CommitAtLeast(Arena* arena, uintsize desired_offset)
{
	if (desired_offset > arena->commited)
	{
		uintsize to_commit = AlignUp(arena->commited - desired_offset, Arena_PAGE_SIZE-1);
		
		if (arena->commited + to_commit > arena->reserved)
			Unreachable();
		
		OS_CommitMemory(arena->memory + arena->commited, to_commit);
		arena->commited += to_commit;
	}
}

internal void*
Arena_PushAligned(Arena* arena, uintsize size, uintsize alignment)
{
	arena->offset = AlignUp(arena->offset, alignment-1);
	
	void* ptr = arena->memory + arena->offset;
	uintsize desired_size = arena->offset + size;
	
	Arena_CommitAtLeast(arena, desired_size);
	
	memset(ptr, 0, size);
	arena->offset += size;
	return ptr;
}

internal inline void*
Arena_Push(Arena* arena, uintsize size)
{
	return Arena_PushAligned(arena, size, 8);
}

internal inline void
Arena_Clear(Arena* arena)
{
	arena->offset = 0;
}

internal inline void*
Arena_End(Arena* arena)
{
	return arena->memory + arena->offset;
}

internal inline void*
Arena_PushMemory(Arena* arena, uintsize size, const void* data)
{
	return memcpy(Arena_PushAligned(arena, size, 1), data, size);
}

internal uintsize
Arena_VPrintf(Arena* arena, const char* fmt, va_list args)
{
	va_list args_copy;
	va_copy(args_copy, args);
	
	uintsize len = OurVPrintfSize(fmt, args_copy);
	char* buf = Arena_PushAligned(arena, len, 1);
	OurVPrintf(buf, len, fmt, args);
	
	va_end(args_copy);
	
	return len;
}

internal uintsize
Arena_Printf(Arena* arena, const char* fmt, ...)
{
	va_list list;
	va_start(list, fmt);
	uintsize len = Arena_VPrintf(arena, fmt, list);
	va_end(list);
	
	return len;
}

internal const char*
Arena_NullTerminateString(Arena* arena, String str)
{
	if (str.size == 0)
		return "";
	if (str.data[str.size-1] == 0)
		return str.data;
	
	char* mem = Arena_PushAligned(arena, str.size+1, 1);
	memcpy(mem, str.data, str.size);
	mem[str.size] = 0;
	
	return mem;
}

internal void
Arena_Destroy(Arena* arena)
{
	OS_FreeMemory(arena, arena->reserved + sizeof *arena);
}

#endif //INTERNAL_ARENA_H
