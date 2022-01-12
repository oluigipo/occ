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

#include "lang_c_definitions.h"

internal void
C_PredefineMacro(C_CompilerOptions* options, String def)
{ PushToStringList(global_arena, &options->predefined_macros, &options->last_predefined_macro, def); }

internal void
C_PreundefineMacro(C_CompilerOptions* options, String def)
{ PushToStringList(global_arena, &options->preundefined_macros, &options->last_preundefined_macro, def); }

internal inline bool32
C_IsWarningEnabled(C_Context* ctx, C_Warning warning)
{
	Assert(warning >= C_Warning_Null && warning < C_Warning__Count);
	uint32 w = (uint32)warning;
	
	uint32 index = w >> 5;
	uint32 bit = w & 63;
	
	return !!(ctx->options->enabled_warnings[index] & bit);
}

internal void
C_PushWarning(C_Context* ctx, C_Warning warning, const char* message)
{
	Arena* arena = (ctx->use_stage_arena_for_warnings) ? ctx->stage_arena : ctx->persistent_arena;
	
	C_QueuedWarning* newone = Arena_Push(arena, sizeof(C_QueuedWarning));
	
	if (!ctx->last_queued_warning)
		ctx->queued_warning = ctx->last_queued_warning = newone;
	else
		ctx->last_queued_warning = ctx->last_queued_warning->next = newone;
	
	ctx->last_queued_warning->warning = warning;
	ctx->last_queued_warning->to_print = message;
}

internal void
C_FlushWarnings(C_Context* ctx)
{
	C_QueuedWarning* warning = ctx->queued_warning;
	
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
#include "lang_c_analyzer.c"
#include "lang_c_gen.c"

struct C_ThreadWork
{
	C_Context ctx;
}
typedef C_ThreadWork;

struct C_ThreadWorkList
{
	OS_RWLock lock;
	C_ThreadWork* works;
	uint32 work_count;
	volatile uint32 work_done;
	volatile uint32 accumulated_error_count;
}
typedef C_ThreadWorkList;

internal void
C_ThreadDoWork(C_ThreadWork* work)
{
#ifdef TRACY_ENABLE
	static _Thread_local bool32 aa = false;
	TraceColor((aa=!aa) ? 0xFF0000 : 0x0000FF);
	TraceSetName(work->ctx.input_file);
#endif
	
	C_Context* ctx = &work->ctx;
	
	switch (ctx->options->mode)
	{
		case C_CompilerMode_InputsToExecutable:
		{
			ctx->tokens = Arena_Push(ctx->persistent_arena, sizeof(*ctx->tokens));
			
			// NOTE(ljre): 'C_Preprocess' pushes warnings to the stage arena, so we need to flush
			//             before clearing it.
			bool32 ok = C_Preprocess(ctx);
			C_FlushWarnings(ctx);
			Arena_Clear(ctx->stage_arena);
			
			ok = ok && C_ParseFile(ctx); Arena_Clear(ctx->stage_arena);
			//ok = ok && C_ResolveAst(ctx); Arena_Clear(ctx->stage_arena);
			
			C_FlushWarnings(ctx);
			
			//ok = ok && C_GenIr(ctx); Arena_Clear(ctx->stage_arena);
			// TODO
			
			if (ctx->output_file.size > 0)
			{
				
			}
		} break;
		
		case C_CompilerMode_InputsToPreprocessed:
		{
			bool32 ok = C_Preprocess(ctx);
			C_FlushWarnings(ctx);
			Arena_Clear(ctx->stage_arena);
			
			if (!ok)
				break;
			
			if (ctx->output_file.size > 0)
			{
				if (!OS_WriteWholeFile(Arena_NullTerminateString(ctx->stage_arena, ctx->output_file), ctx->pre_source, OurStrLen(ctx->pre_source), ctx->stage_arena))
				{
					Print("error: could not open output file.\n");
					++ctx->error_count;
				}
			}
		} break;
		
		default: Unreachable();
	}
	
	Print("\n====== Memory Usage (%S):\nStage Commited: \t%z bytes\nPersistent Offset:\t%z bytes\n",
		  StrFmt(ctx->input_file), ctx->stage_arena->commited, ctx->persistent_arena->offset);
}

internal void
C_WorkerThreadProc(void* user_data)
{
	{
		Trace();
		
		C_ThreadWorkList* worklist = user_data;
		C_ThreadWork* work = NULL;
		
		for (;;)
		{
			OS_LockRWLockWrite(worklist->lock);
			
			if (work)
			{
				worklist->accumulated_error_count += work->ctx.error_count;
				work = NULL;
			}
			
			if (worklist->work_done < worklist->work_count)
				work = &worklist->works[worklist->work_done++];
			
			OS_UnlockRWLockWrite(worklist->lock);
			
			if (!work)
				break;
			
			C_ThreadDoWork(work);
		}
	}
	
	OS_ExitThisThread(0);
}

#include "lang_c_driver.c"

internal int32
C_Main(int32 argc, const char** argv)
{
	Trace();
	
	int32 result = 0;
	
	// TODO(ljre): cl.exe compiler driver just for testing.
	result = C_DefaultDriver(argc, argv);
	
	return result;
}
