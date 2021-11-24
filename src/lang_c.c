/*
*
* TODO:
*    - Validate UTF-8 Codepoints;
*    - Parsing standard C99 (halfway through);
*    - Type-checking;
*    - Code analysis for warnings:
*        - "a<<b + 10", misleading spacing between operations;
*        - constness;
*    - Codegen with Succ3s's backend;
*    - Flag (-U): Undefine a macro;
*    - Flag (-v): Print to stdout verbose log;
*    - Flags parser;
*    - Code analyzer: Generate useful warnings;
*    - Flag (-W): Warnings flags;
*    - Flag (-w): Suppress all warnings;
*    - Revision 'LangC_Node';
*    - Worker Threads for multiple input files;
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
*        - Make type 'T[N]' a subset of type 'T[M]' when N < M;
*        - Make type 'struct A' a subset of type 'struct B' if the beginning of B is *the same* as A:
*            - Example: struct A { int n; }; struct B { int pp; double k; }; -- here A is a subset of B;
*            - Example: struct A { long n; }; struct B { int pp; double k; }; -- here A is *not* a subset of B;
*
* IMPORTANT:
*    - Don't implement C2x lambdas (https://twitter.com/__phantomderp/status/1434306584465793028)
*/

#include "lang_c_definitions.h"

internal int32 LangC_error_count = 0;

// TODO(ljre): Maybe better this?
struct LangC_Colors
{
	const char* reset;
	const char* error;
	const char* warning;
	const char* paths;
}
typedef LangC_Colors;

internal LangC_Colors LangC_colors = {
	"", "", "", "", // NOTE(ljre): Default values.
};

internal void
LangC_AddInputFile(StringList** first, StringList** last, String str)
{
	Assert(first);
	Assert(last);
	
	if (!*last)
	{
		*first = *last = PushMemory(sizeof **last);
	}
	else
	{
		*last = (*last)->next = PushMemory(sizeof **last);
	}
	
	(*last)->value = str;
}

internal inline bool32
LangC_IsWarningEnabled(LangC_Context* ctx, LangC_Warning warning)
{
	Assert(warning >= LangC_Warning_Null && warning < LangC_Warning__Count);
	uint32 w = (uint32)warning;
	
	uint32 index = w >> 5;
	uint32 bit = w & 63;
	
	return !!(ctx->options->enabled_warnings[index] & bit);
}

internal void
LangC_PushWarning(LangC_Context* ctx, LangC_Warning warning, const char* message)
{
	Arena* arena = (ctx->use_stage_arena_for_warnings) ? ctx->stage_arena : ctx->persistent_arena;
	
	LangC_QueuedWarning* newone = Arena_Push(arena, sizeof(LangC_QueuedWarning));
	
	if (!ctx->last_queued_warning)
	{
		ctx->queued_warning = ctx->last_queued_warning = newone;
	}
	else
	{
		ctx->last_queued_warning = ctx->last_queued_warning->next = newone;
	}
	
	ctx->last_queued_warning->warning = warning;
	ctx->last_queued_warning->to_print = message;
}

internal void
LangC_FlushWarnings(LangC_Context* ctx)
{
	LangC_QueuedWarning* warning = ctx->queued_warning;
	
	while (warning)
	{
		Print("%s\n", warning->to_print);
		warning = warning->next;
	}
	
	ctx->queued_warning = NULL;
	ctx->last_queued_warning = NULL;
}

// NOTE(ljre): The order of the includes here matters.
#include "lang_c_lexer.c"
#include "lang_c_preprocessor.c"
#include "lang_c_parser.c"
#include "lang_c_resolution.c"
#include "lang_c_gen.c"

#include "lang_c_driver.c"

internal int32
LangC_Main(int32 argc, const char** argv)
{
	Trace();
	
	int32 result = 0;
	
	// TODO(ljre): cl.exe compiler driver just for testing.
	result = LangC_DefaultDriver(argc, argv);
	
	return result;
}
