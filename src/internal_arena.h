#ifndef INTERNAL_ARENA_H
#define INTERNAL_ARENA_H

#define Arena_PAGE_SIZE Megabytes(1)

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
Arena_Commit(Arena* arena)
{
	if (arena->commited + Arena_PAGE_SIZE > arena->reserved)
		Unreachable();
	
	OS_CommitMemory(arena->memory + arena->commited, Arena_PAGE_SIZE);
	arena->commited += Arena_PAGE_SIZE;
}

internal void*
Arena_PushAligned(Arena* arena, uintsize size, uintsize alignment)
{
	arena->offset = AlignUp(arena->offset, alignment-1);
	
	void* ptr = arena->memory + arena->offset;
	uintsize desired_size = arena->offset + size;
	
	while (desired_size > arena->commited)
		Arena_Commit(arena);
	
	memset(ptr, 0, size);
	arena->offset += size;
	return ptr;
}

internal void*
Arena_Push(Arena* arena, uintsize size)
{
	return Arena_PushAligned(arena, size, 8);
}

internal void
Arena_Clear(Arena* arena)
{
	arena->offset = 0;
}

internal void*
Arena_End(Arena* arena)
{
	return arena->memory + arena->offset;
}

internal void*
Arena_PushMemory(Arena* arena, uintsize size, const void* data)
{
	return memcpy(Arena_PushAligned(arena, size, 1), data, size);
}

internal uintsize
Arena_VPrintf(Arena* arena, const char* fmt, va_list args)
{
	va_list args_copy;
	va_copy(args_copy, args);
	
	uintsize len = vsnprintf(NULL, 0, fmt, args_copy);
	char* buf = Arena_PushAligned(arena, len + 1, 1);
	vsnprintf(buf, len + 1, fmt, args);
	arena->offset -= 1;
	
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

internal void
Arena_Destroy(Arena* arena)
{
	OS_FreeMemory(arena, arena->reserved + sizeof *arena);
}

#endif //INTERNAL_ARENA_H
