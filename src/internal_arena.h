#ifndef INTERNAL_ARENA_H
#define INTERNAL_ARENA_H

struct alignas(16) Arena typedef Arena;
struct Arena
{
	uintsize reserved;
	uintsize commited;
	uintsize offset;
	Arena* next;
	uint8 memory[];
};

internal Arena* Arena_Create(uintsize size);
internal void Arena_CommitAtLeast(Arena* arena, uintsize desired_offset);
internal void* Arena_PushDirtyAligned(Arena* arena, uintsize size, uintsize alignment);
internal inline void* Arena_PushDirty(Arena* arena, uintsize size);
internal void* Arena_PushAligned(Arena* arena, uintsize size, uintsize alignment);
internal inline void* Arena_Push(Arena* arena, uintsize size);
internal inline void Arena_Pop(Arena* arena, void* ptr);
internal inline void Arena_Clear(Arena* arena);
internal inline void* Arena_End(Arena* arena);
internal inline void* Arena_PushMemory(Arena* arena, uintsize size, const void* data);
internal uintsize Arena_VPrintf(Arena* arena, const char* fmt, va_list args);
internal uintsize Arena_Printf(Arena* arena, const char* fmt, ...);
internal const char* Arena_NullTerminateString(Arena* arena, String str);
internal void Arena_Destroy(Arena* arena);
internal bool32 Arena_Owns(Arena* arena, const void* ptr);

#endif //INTERNAL_ARENA_H
