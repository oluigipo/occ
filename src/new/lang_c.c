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

#include "lang_c_log.c"
#include "lang_c_lexer.c"
#include "lang_c_preprocessor.c"
#include "lang_c_parser.c"
#include "lang_c_analyzer.c"

internal int32
C_ParseArgsToOptions(C_Context* ctx, C_CompilerOptions* options, int32 argc, const char* const* argv, String* out_file_to_build)
{
	//- NOTE(ljre): Know which dirs to include
	String* include_dirs = Arena_Push(ctx->array_arena, sizeof(*include_dirs));
	*include_dirs = Str("include/");
	
	for (int32 i = 1; i < argc; ++i)
	{
		const char* arg = argv[i];
		
		if (arg[0] == '-' && arg[1] == 'I')
		{
			if (arg[2] == '\0')
				arg = argv[i++];
			else
				arg = arg + 2;
			
			if (!arg) // NOTE(ljre): argv[argv] == NULL
			{
				Print("expected path after -I flag.");
				return 1;
			}
			
			String str;
			uintsize len = strlen(arg);
			
			if (arg[len-1] != '/')
				str = Arena_SPrintf(ctx->tree_arena, "%S/", len, arg);
			else
				str = StrMake(arg, len);
			
			String* ptr = Arena_Push(ctx->array_arena, sizeof(*ptr));
			*ptr = str;
			
			continue;
		}
		
		if (arg[0] != '-' && !out_file_to_build->size)
			*out_file_to_build = StrFrom(arg);
	}
	
	options->include_dirs = include_dirs;
	options->include_dirs_count = (String*)Arena_End(ctx->array_arena) - include_dirs;
	
	return 0;
}

internal int32
C_Main(int32 argc, const char* const* argv)
{
	Trace();
	
	C_CompilerOptions options = {
		.warnings = { 0xFF },
		
		.abi = {
			.char_bit = 8,
			.index_sizet = 11,
			.index_ptrdifft = 10,
			
			.t_bool = { 1, 1, false },
			.t_schar = { 1, 1, false },
			.t_char = { 1, 1, true },
			.t_uchar = { 1, 1, true },
			.t_short = { 2, 2, false },
			.t_ushort = { 2, 2, true },
			.t_int = { 4, 4, false },
			.t_uint = { 4, 4, true },
			.t_long = { 4, 4, false },
			.t_ulong = { 4, 4, true },
			.t_longlong = { 8, 8, false },
			.t_ulonglong = { 8, 8, true },
			.t_float = { 4, 4, false },
			.t_double = { 8, 8, false },
			.t_ptr = { 8, 8, true },
		},
	};
	
	C_Context ctx = {
		.array_arena = Arena_Create(Megabytes(32)),
		.tree_arena = Arena_Create(Megabytes(128)),
		.scratch_arena = Arena_Create(Megabytes(32)),
		
		.options = &options,
	};
	
	String file_to_build = { 0 };
	
	// NOTE(ljre): Setup 'options'
	int32 result = C_ParseArgsToOptions(&ctx, &options, argc, argv, &file_to_build);
	if (result != 0)
		return result;
	
	const char* macro_defs[] = {
		"__STDC__ 1",
		"__STDC_HOSTED__ 1",
		"__STDC_VERSION__ 199901L",
		"__x86_64 1",
		"__x86_64__ 1",
		"_M_AMD64 1",
		"_M_X64 1",
		"_WIN32 1",
		"_WIN64 1",
		"__OCC__ 1",
		"__int64 long long",
		"__int32 int",
		"__int16 short",
		"__int8 char",
		"__inline inline",
		"__inline__ inline",
		"__restrict restrict",
		"__restrict__ restrict",
		"__const const",
		"__const__ const",
		"__volatile volatile",
		"__volatile__ volatile",
		"__attribute __attribute__",
		
		//"__forceinline inline",
		//"__attribute__(...)",
		//"__declspec(...)",
		
		"__builtin_offsetof(_Type, _Field) (&((_Type*)0)->_Field)",
		"__builtin_va_list void*",
		
		// NOTE(ljre): MINGW macros
		"_MSC_VER 1910",
		"_MSC_FULL_VER 191025017",
		
		//"__MINGW_ATTRIB_DEPRECATED_STR(x)",
		//"__MINGW_ATTRIB_NONNULL(x)",
		//"__MINGW_NOTHROW",
		//"__mingw_ovr",
	};
	
	options.predefined_macros = macro_defs;
	options.predefined_macros_count = ArrayLength(macro_defs);
	
	if (file_to_build.size <= 0)
	{
		Print("%C2error:%C0 no input files.\n");
		return 1;
	}
	
	C_TokenSlice source = C_Preprocess(&ctx, file_to_build);
	
	// NOTE(ljre): DEBUG
	{
		String str = C_PrintTokensGnuStyle(&ctx, ctx.array_arena, source);
		OS_WriteWholeFile(Str("test.c"), str.data, str.size, ctx.scratch_arena);
	}
	
	C_AstDecl* ast = C_Parse(&ctx, source);
	
	(void)ast;
	
	if (ctx.error_count > 0)
		Print("%C2compilation failed%C0 with %u errors and %u warnings.\n", ctx.error_count, ctx.warning_count);
	else
		Print("%C4compilation successful%C0 with %u warnings.\n", ctx.warning_count);
	
	Print("\t%C1ctx.array_arena->   commited:%C0 %z\toffset: %z\n", ctx.array_arena->commited, ctx.array_arena->offset);
	Print("\t%C1ctx.tree_arena->    commited:%C0 %z\toffset: %z\n", ctx.tree_arena->commited, ctx.tree_arena->offset);
	Print("\t%C1ctx.scratch_arena-> commited:%C0 %z\toffset: %z\n", ctx.scratch_arena->commited, ctx.scratch_arena->offset);
	
	return 0;
}
