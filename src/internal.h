#ifndef INTERNAL_H
#define INTERNAL_H

//~ Types
#include <stdint.h>
#include <stddef.h>

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;
typedef intptr_t intptr;
typedef ptrdiff_t intsize;

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef uintptr_t uintptr;
typedef size_t uintsize;

#define true 1
#define false 0

typedef int8_t bool8;
typedef int16_t bool16;
typedef int32_t bool32;
typedef int64_t bool64;

struct String
{
	uintsize size;
	const char* data;
} typedef String;

#define Str(str) (String) StrInit(str)
#define StrFrom(cstr) (String) { .size = strlen(cstr), .data = (cstr) }
#define StrInit(str) { .size = sizeof(str), .data = (str) }
#define StrFmt(str) (int32)(str).size, (str).data
#define StrNull (String) { 0 }

//~ Utility Macros
#define ArrayLength(arr) (sizeof(arr) / sizeof*(arr))
#define AlignUp(val,mask) ((val)+(mask) & ~(mask))
#define Kilobytes(n) ((uintsize)n*1024)
#define Megabytes(n) (Kilobytes(n)*1024)
#define Gigabytes(n) (Megabytes(n)*1024)
#define internal static
#define Max(a,b) ((a) > (b) ? (a) : (b))
#define Min(a,b) ((a) < (b) ? (a) : (b))
#define MAX_PATH_SIZE Kilobytes(1)

//~ Utility Functions
internal uint64
SimpleHash(String str)
{
	uint64 hash = 2166136261ull;
	uintsize i = 0;
	
	while (i < str.size && str.data[i]) {
		uint8 value = (uint64)str.data[i++];
		
		hash ^= value;
		hash *= 16777619ull;
	}
	
	return hash;
}

internal uint64
SimpleHashNullTerminated(const char* str)
{
	uint64 hash = 2166136261ull;
	
	while (*str) {
		uint8 value = *str++;
		
		hash ^= value;
		hash *= 16777619ull;
	}
	
	return hash;
}

internal void* PushMemory(uintsize size);

//~ Stretchy Buffers
#include <string.h>

struct StretchyBufferHeader
{
	uint32 len, cap;
} typedef StretchyBufferHeader;

#define SB__Header(buf) ((buf) ? (StretchyBufferHeader*)(buf)-1 : 0)
#define SB__Len(buf) SB__Header(buf)->len
#define SB__Cap(buf) SB__Header(buf)->cap

#define SB_Cap(buf) ((buf) ? SB__Cap(buf) : 0)
#define SB_Len(buf) ((buf) ? SB__Len(buf) : 0)
#define SB_ReserveAtLeast(buf,s) SB__ReserveAtLeast((void**)&(buf), s, sizeof *(buf))
#define SB_ReserveMore(buf,s) SB_ReserveAtLeast(buf,SB_Len(buf)+(s))
#define SB_Push(buf,item) (SB_ReserveAtLeast(buf, SB_Len(buf)+1), (buf)[SB__Len(buf)++] = (item))
#define SB_End(buf) ((buf) + SB_Len(buf))
#define SB_AddLen(buf,s) (SB__Len(buf) += (s))
#define SB_PushArray(buf,s,data) (SB_ReserveMore(buf, s),\
memcpy(SB_End(buf), data, s),\
SB_AddLen(buf, s))

internal void
SB__ReserveAtLeast(void** buf, uint32 count, uint32 size)
{
	StretchyBufferHeader* header;
	
	if (*buf)
	{
		header = *buf;
		header -= 1;
		
		if (header->cap < count)
		{
			uint32 newcap = header->cap;
			
			do
			{
				newcap += (newcap >> 1) + 1; // ~golden ratio, close enough
			}
			while (newcap < count);
			
			StretchyBufferHeader* new_header = PushMemory(newcap * size + sizeof *header);
			new_header->len = header->len;
			new_header->cap = newcap;
			
			memcpy(new_header + 1, header + 1, header->len * size);
			
			header = new_header;
		}
	}
	else
	{
		header = PushMemory(count * size + sizeof *header);
		
		header->len = 0;
		header->cap = count;
	}
	
	*buf = header + 1;
}

//~ String Helpers
internal const char*
NullTerminateString(String str)
{
	if (str.size == 0)
		return "";
	
	if (str.data[str.size-1] == 0)
		return str.data;
	
	char* result = PushMemory(str.size+1);
	memcpy(result, str.data, str.size);
	result[str.size] = 0;
	
	return result;
}

internal bool32
MatchCString(const char* a, const char* cmp, int32 cmp_len)
{
	while (cmp_len --> 0)
	{
		if (*a++ != *cmp++)
			return false;
	}
	
	if (*a)
		return false;
	
	return true;
}

internal int32
CompareString(String a, String b)
{
	if (a.size != b.size)
		return (int32)a.size - (int32)b.size;
	
	int32 n = (int32)a.size;
	const uint8* s1 = (const uint8*)a.data;
	const uint8* s2 = (const uint8*)b.data;
	
	while (n > 0 && *s1 && *s1 == *s2)
	{
		++s1;
		++s2;
		--n;
	}
	
	if (n == 0)
		return 0;
	
	return (int32)*s1 - (int32)*s2;
}

//~ Debug
#include <assert.h>

#endif //INTERNAL_H
