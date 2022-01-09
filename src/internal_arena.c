#define Arena_PAGE_SIZE Megabytes(32)

internal Arena*
Arena_Create(uintsize size)
{
	Trace();
	
	size = AlignUp(size, Arena_PAGE_SIZE-1);
	Assert(size > 0);
	
	Arena* arena = OS_ReserveMemory(size + sizeof *arena);
	if (arena)
	{
		void* result = OS_CommitMemory(arena, sizeof *arena + Arena_PAGE_SIZE);
		Assert(result);
		(void)result;
		
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
	if (Unlikely(desired_offset > arena->commited))
	{
		Trace();
		
		uintsize to_commit = AlignUp(desired_offset - arena->commited, Arena_PAGE_SIZE-1);
		if (arena->commited + to_commit > arena->reserved)
			Unreachable();
		
		void* result = OS_CommitMemory(arena->memory + arena->commited, to_commit);
		Assert(result);
		(void)result;
		
		arena->commited += to_commit;
	}
}

internal void*
Arena_PushDirtyAligned(Arena* arena, uintsize size, uintsize alignment)
{
	arena->offset = AlignUp(arena->offset, alignment-1);
	
	void* ptr = arena->memory + arena->offset;
	uintsize desired_size = arena->offset + size;
	
	Arena_CommitAtLeast(arena, desired_size);
	
	arena->offset += size;
	return ptr;
}

internal inline void
Arena_Pop(Arena* arena, void* ptr)
{
	uint8* p = ptr;
	Assert(p >= arena->memory && p <= arena->memory + arena->offset);
	
	arena->offset = p - arena->memory;
}

internal uintsize
Arena_VPrintf(Arena* arena, const char* fmt, va_list args)
{
	va_list args_copy;
	va_copy(args_copy, args);
	
	uintsize len = OurVPrintfSize(fmt, args_copy);
	uintsize offset = arena->offset;
	
	char* buf = Arena_PushDirtyAligned(arena, len, 1);
	
	len = OurVPrintf(buf, len, fmt, args);
	arena->offset = offset + len;
	
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
	
	char* mem = Arena_PushDirtyAligned(arena, str.size+1, 1);
	OurMemCopy(mem, str.data, str.size);
	mem[str.size] = 0;
	
	return mem;
}

internal String
Arena_VSPrintf(Arena* arena, const char* fmt, va_list args)
{
	va_list args_copy;
	va_copy(args_copy, args);
	
	uintsize len = OurVPrintfSize(fmt, args_copy);
	uintsize offset = arena->offset;
	
	char* buf = Arena_PushDirtyAligned(arena, len, 1);
	
	len = OurVPrintf(buf, len, fmt, args);
	arena->offset = offset + len;
	
	va_end(args_copy);
	
	return StrMake(buf, len);
}

internal String
Arena_SPrintf(Arena* arena, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	String result = Arena_VSPrintf(arena, fmt, args);
	
	va_end(args);
	return result;
}

internal bool32
Arena_Owns(Arena* arena, const void* ptr)
{
	const uint8* p = ptr;
	return (p >= arena->memory && p < arena->memory + arena->reserved);
}

internal inline void
Arena_Clear(Arena* arena)
{ arena->offset = 0; }

internal inline void*
Arena_End(Arena* arena)
{ return arena->memory + arena->offset; }

internal inline void*
Arena_PushMemory(Arena* arena, uintsize size, const void* data)
{ return OurMemCopy(Arena_PushDirtyAligned(arena, size, 1), data, size); }

internal void
Arena_Destroy(Arena* arena)
{ OS_FreeMemory(arena, arena->reserved + sizeof *arena); }

internal inline void* Arena_PushDirty(Arena* arena, uintsize size)
{ return Arena_PushDirtyAligned(arena, size, 8); }

internal void*
Arena_PushAligned(Arena* arena, uintsize size, uintsize alignment)
{ return OurMemSet(Arena_PushDirtyAligned(arena, size, alignment), 0, size); }

internal inline void*
Arena_Push(Arena* arena, uintsize size)
{ return OurMemSet(Arena_PushDirtyAligned(arena, size, 8), 0, size); }
