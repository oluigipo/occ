/*
*
* TODO:
*    - Validate UTF-8 Codepoints;
*    - Parsing standard C99 (halfway through);
*    - Type-checking;
*    - Codegen with Succ3s's backend;
*    - Flag (-U): Undefine a macro;
*    - Flag (-v): Print to stdout verbose log;
*    - Flags parser;
*    - Code analyzer: Generate useful warnings;
*    - Flag (-W): Warnings flags;
*    - Flag (-w): Suppress all warnings;
*    - Worker Threads for multiple input files;
*    - Thread-safe file cache (less memory consumption);
*    - Flag (???): Disable colorful log;
*
* IDEAS:
*    - Flag (-Edecl): Output only declarations to output file;
*    - Flag (-Emacro): Output macro defs and undefs;
*    - Flag (-+): Simple C++ features to build C-like C++ projects:
*        - Typedef structs, enums, and unions by default;
*        - Default values for function parameters;
*        - Function Overloading;
*        - extern "C";
*        - {} literals;
*    - Extensions:
*        - void* + T*: add both operands and return a value of type T*;
*        - defer: scoped defer;
*        - Extend compound literals for function objects;
*            Example: fptr = (int(int a, int b)) { return a + b; };
*        - (GNU) Elvis operator (a ?: b);
*        - (GNU) Switch Ranges;
*        - __builtin_assume(expr);
*        - __debugbreak();
*        - Default values for function parameters;
*        - #once directive (http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2742.htm);
*        - Optional configuration file (.ini) in the same folder as the compiler:
*            - Defines some paths the compiler needs for includes, etc.;
*            - Defines other programs's paths, such as linker, etc.;
*        - One of those (last 2 requires the lexer and parser to deal with the entire file *again*):
*            - const char* __builtin_embed(const char* path); size_t __builtin_embed_size(const char* path);
*            - directive #embed ident "path", defines 'ident' as an 'unsigned char[]' of the file's contents;
*            - macro __builtin_embed(path) expands to comma separated integers;
*            - directive #embed "path", expands to { comma separated integers };
*        - Make type 'struct A' a subset of 'struct B' if the beginning of B is *the same* as A (duck typing):
*            - Example: struct A { int n; }; struct B { int pp; double k; }; -- here A is a subset of B;
*            - Example: struct A { long n; }; struct B { int pp; double k; }; -- here A is *not* a subset of B;
*
*/

#define C_VERSION_MAJOR 0
#define C_VERSION_MINOR 0
#define C_VERSION_PATCH 1
#define C_VERSION_STR StrMacro(C_VERSION_MAJOR) "." StrMacro(C_VERSION_MINOR) "." StrMacro(C_VERSION_PATCH)
#define C_PREUNDEFINED_MACRO_PTR ((void*)1)

#include "lang_c_definitions.h"

internal void
C_TraceErrorVarArgs(C_Context* ctx, C_SourceTrace* trace, const char* fmt, va_list args)
{
	PrintVarargs(fmt, args);
}

internal void
C_TraceWarningVarArgs(C_Context* ctx, C_SourceTrace* trace, C_Warning warning, const char* fmt, va_list args)
{
	PrintVarargs(fmt, args);
}

internal inline void
C_TraceError(C_Context* ctx, C_SourceTrace* trace, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	C_TraceErrorVarArgs(ctx, trace, fmt, args);
	va_end(args);
}

internal inline void
C_TraceWarning(C_Context* ctx, C_SourceTrace* trace, C_Warning warning, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	C_TraceWarningVarArgs(ctx, trace, warning, fmt, args);
	va_end(args);
}

internal inline void
C_TraceErrorRd(C_Context* ctx, C_TokenReader* rd, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	if (rd->file_trace)
	{
		C_SourceTrace trace = rd->head->trace;
		trace.file = rd->file_trace;
		
		C_TraceErrorVarArgs(ctx, &trace, fmt, args);
	}
	else
	{
		C_TraceErrorVarArgs(ctx, &rd->head->trace, fmt, args);
	}
	
	va_end(args);
}

internal inline void
C_TraceWarningRd(C_Context* ctx, C_TokenReader* rd, C_Warning warning, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	if (rd->file_trace)
	{
		C_SourceTrace trace = rd->head->trace;
		trace.file = rd->file_trace;
		
		C_TraceWarningVarArgs(ctx, &trace, warning, fmt, args);
	}
	else
	{
		C_TraceWarningVarArgs(ctx, &rd->head->trace, warning, fmt, args);
	}
	
	
	va_end(args);
}

#include "lang_c_lexer.c"
#include "lang_c_preprocessor.c"

internal int32
C_Main(int32 argc, const char** argv)
{
	Trace();
	
	int32 result = 0;
	
	String include_dirs[] = {
		StrInit("include/"),
	};
	
	C_CompilerOptions options = {
		.include_dirs = include_dirs,
		.include_dirs_count = ArrayLength(include_dirs),
	};
	
	C_Context ctx = {
		.array_arena = Arena_Create(Megabytes(32)),
		.tree_arena = Arena_Create(Megabytes(32)),
		.scratch_arena = Arena_Create(Megabytes(32)),
		
		.options = &options,
	};
	
	C_TokenSlice source = C_Preprocess(&ctx, Str("tests/pp-test.c"));
	
	String str = C_PrintTokensGnuStyle(&ctx, ctx.array_arena, source);
	
	OS_WriteWholeFile(Str("test.c"), str.data, str.size, ctx.scratch_arena);
	
	return result;
}
