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
		assert(false);
	
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
Arena_Destroy(Arena* arena)
{
	OS_FreeMemory(arena, arena->reserved + sizeof *arena);
}

#endif //INTERNAL_ARENA_H
