#ifndef INTERNAL_H
#define INTERNAL_H

#ifdef __GNUC__
#   define alignas(_) __attribute__((aligned(_)))
#elif defined _MSC_VER
#   define alignas(_) __declspec(align(_))
#else
#   define alignas(_)
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

// NOTE(ljre): A 'String' is just an immutable slice of bytes. 'str.data[str.size-1]' can be NULL, but doesn't need to be.
struct String
{
	uintsize size;
	const char* data;
}
typedef String;

#define Str(str) (String) StrInit(str)
#define StrFrom(cstr) (String) { .size = strlen(cstr), .data = (cstr) }
#define StrInit(str) { .size = sizeof(str), .data = (str) }
#define StrFmt(str) (str).size, (str).data
#define StrNull (String) { 0 }
#define StrMake(str,len) (String) { .size = (len), .data = (str) }
#define StrMacro_(x) #x
#define StrMacro(x) StrMacro_(x)

//~ Utility Macros
#define ArrayLength(arr) (sizeof(arr) / sizeof*(arr))
#define AlignUp(val,mask) ((val)+(mask) & ~(mask))
#define Kilobytes(n) ((uintsize)(n)*1024)
#define Megabytes(n) (Kilobytes(n)*1024)
#define Gigabytes(n) (Megabytes(n)*1024)
#define internal static
#define Max(a,b) ((a) > (b) ? (a) : (b))
#define Min(a,b) ((a) < (b) ? (a) : (b))
#define SizeofMember(Struct, name) sizeof(((Struct*)0)->name)
#define SizeofPoly(Struct, name) (sizeof(Struct) + SizeofMember(Struct, member))

// NOTE(ljre): Unreachable may be used in release builds.
#define Unreachable() do { Panic("\nUnreachable code was reached, at '" __FILE__ "' line " StrMacro(__LINE__) ".\n"); } while (0)

// NOTE(ljre): Compiler specifics
#ifdef __clang__
#   define Assume(x) __builtin_assume(x)
#   define DebugBreak_() __builtin_debugtrap()
#   define Likely(x) __builtin_expect(!!(x), 1)
#   define Unlikely(x) __builtin_expect((x), 0)
#   if !defined NDEBUG && defined TRACY_ENABLE
#       include "../../../ext/tracy/TracyC.h"
internal void ___my_tracy_zone_end(TracyCZoneCtx* ctx) { TracyCZoneEnd(*ctx); }
#       define Trace() TracyCZone(_ctx __attribute((cleanup(___my_tracy_zone_end))),true); ((void)_ctx)
#       define TraceName(x) Trace(); { String a = (x); TracyCZoneText(_ctx, a.data, a.size); }
#   else
#       define Trace() ((void)0)
#       define TraceName(x) ((void)0)
#   endif
#elif defined _MSC_VER
#   define Assume(x) __assume(x)
#   define DebugBreak_() __debugbreak()
#   define Likely(x) (x)
#   define Unlikely(x) (x)
#   define Trace(x) ((void)0)
#   define TraceName(x) ((void)0)
#elif defined __GNUC__
#   define Assume(x) do { if (!(x)) __builtin_unreachable(); } while (0)
#   define DebugBreak_() __asm__ __volatile__ ("int $3")
#   define Likely(x) __builtin_expect(!!(x), 1)
#   define Unlikely(x) __builtin_expect((x), 0)
#   define Trace(x) ((void)0)
#   define TraceName(x) ((void)0)
#else
#   define Assume(x) ((void)0)
#   define DebugBreak_() ((void)0)
#   define Likely(x) (x)
#   define Unlikely(x) (x)
#   define Trace(x) ((void)0)
#   define TraceName(x) ((void)0)
#endif

// NOTE(ljre): Assert
#ifdef NDEBUG
#   define Assert(x) Assume(x)
#else
#   define Assert(x) do { if (!(x)) { DebugBreak_(); Panic("\n########## ASSERTION FAILURE\nFile: " __FILE__ "\nLine: " StrMacro(__LINE__) "\nExpression: " #x "\n"); } } while (0)
#endif

internal void* PushMemory(uintsize size);
internal void Print(const char* fmt, ...);
internal void PrintVarargs(const char* fmt, va_list args);
internal void Panic(const char* str);

internal const char* const* global_colors = (const char* const[]) {
	"\x1B[0m", // RESET
	"\x1B[93m", // PATHS
	"\x1B[91m", // ERROR
	"\x1B[95m", // WARNING
	"\x1B[92m", // GOOD/GREEN
	NULL,
};

#include "internal_arena.h"
#include "os.h"
#include "internal_utils.h"
#include "internal_pool.h"
#include "internal_map.h"
#include "internal_littlemap.h"

#include "internal_arena.c"

internal String global_my_path;
internal Arena* global_arena;

#endif //INTERNAL_H
