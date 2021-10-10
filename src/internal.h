#ifndef INTERNAL_H
#define INTERNAL_H

#ifdef __GNUC__
#   define alignas(_) __attribute__((aligned(_)))
#else
#   define alignas(_) __declspec(align(_))
#endif

#if 0
;
#endif

//~ Types
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
}
typedef String;

#define Str(str) (String) StrInit(str)
#define StrFrom(cstr) (String) { .size = strlen(cstr), .data = (cstr) }
#define StrInit(str) { .size = sizeof(str), .data = (str) }
#define StrFmt(str) (int32)(str).size, (str).data
#define StrNull (String) { 0 }
#define StrMake(str,len) (String) { .size = (len), .data = (str) }
#define StrMacro_(x) #x
#define StrMacro(x) StrMacro_(x)

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
#define Unreachable() do { Panic("Unreachable code was reached, at '" __FILE__ "' line " StrMacro(__LINE__) ".") } while (0)

internal void* PushMemory(uintsize size);

#include <assert.h>

#include "os.h"

#include "internal_arena.h"
#include "internal_sb.h"
#include "internal_utils.h"

#endif //INTERNAL_H
