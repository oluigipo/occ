/*
*
* TODO:
*    - Parsing standard C99 (halfway through);
*    - Type-checking;
*    - Codegen with Succ3s's backend;
*    - Flag (--help);
*    - Flag (-I): Add include directory;
*    - Flag (-D): Define a macro;
*    - Flag (-U): Undefine a macro;
*    - Flag (-v): Print to stdout commands the compiler is executing;
*    - Warnings pipeline: Generate, filter, and print if no errors occured;
*    - Flags parser;
*    - Code analyzer: Generate useful warnings;
*    - Trace() macro so we can use Tracy :);
*    - Flag (-W): Warnings flags;
*    - Flag (-w): Suppress all warnings;
*
* IDEAS:
*    - Flag (-Edecl): Output declarations to output file;
*    - Flag (-+): Simple C++ features to build C-like C++ projects:
*        - Typedef structs, enums, and unions by default;
*        - Default values for function parameters;
*        - Function Overloading;
*        - extern "C";
*        - {} literals;
*
*/

#define LangC_MAX_INCLUDE_DIRS 64

internal char LangC_include_dirs[LangC_MAX_INCLUDE_DIRS][MAX_PATH_SIZE];
internal uintsize LangC_include_dirs_count;
internal int32 LangC_error_count = 0;

enum LangC_InvokationMode
{
	LangC_InvokationMode_BuildToExecutable,
	LangC_InvokationMode_RunPreprocessor,
}
typedef LangC_InvokationMode;

#include "lang_c_definitions.h"
#include "lang_c_lexer.c"
#include "lang_c_preprocessor.c"
#include "lang_c_parser.c"

internal void
LangC_PrintHelp(void)
{
	Print("Our C Compiler -- help:\n"
		  "usage: occ [FILE | FLAG] ...\n"
		  "\n"
		  "flags:\n"
		  "\t[ -o<file>  ]\n"
		  "\t[ -o <file> ] Changes the output file (defaults to \"a.out\").\n"
		  "\t[ -E ] Runs the preprocessor on the single input file.\n"
		  "\n");
}

internal void
LangC_Main(int32 argc, const char** argv)
{
	// NOTE(ljre): Setup system include directory
	{
		LangC_include_dirs_count = 1;
		char* basic_path = LangC_include_dirs[0];
		
		int32 len = strlen(global_my_path);
		memcpy(basic_path, global_my_path, len);
		
		int32 last_slash_index = -1;
		for (int32 i = 0; i < len; ++i)
		{
			if (basic_path[i] == '/')
				last_slash_index = i;
		}
		
		if (last_slash_index != -1)
		{
			const char include[] = "include/";
			memcpy(basic_path + last_slash_index+1, include, sizeof include);
		}
	}
	
	// NOTE(ljre): Parse parameters
	if (argc < 2)
	{
		LangC_PrintHelp();
		return;
	}
	
	LangC_InvokationMode mode = LangC_InvokationMode_BuildToExecutable;
	bool32 error_while_parsing_args = false;
	String file = StrNull;
	String output_file = Str("a.out");
	
	for (const char** arg = argv + 1; *arg; ++arg)
	{
		if (arg[0][0] == '-')
		{
			const char* flag = arg[0] + 1;
			
			if (MatchCString(flag, "o", 1))
			{
				if (flag[1])
					output_file = StrFrom(flag + 1);
				else if (arg[1])
				{
					const char* name = arg[1];
					++arg;
					output_file = StrFrom(name);
				}
				else
				{
					error_while_parsing_args = true;
					Print("error: expected file name after '-o' flag.\n");
				}
			}
			else if (MatchCString(flag, "E", 1))
			{
				mode = LangC_InvokationMode_RunPreprocessor;
			}
		}
		else
		{
			file = StrFrom(arg[0]);
		}
	}
	
	if (error_while_parsing_args)
		return;
	
	if (file.size == 0)
	{
		Print("error: no input files\n");
	}
	
	switch (mode)
	{
		case LangC_InvokationMode_BuildToExecutable:
		{
			const char* src = LangC_Preprocess(file);
			if (!src)
				break;
			
			LangC_ParseFile(src);
			
			// TODO
		} break;
		
		case LangC_InvokationMode_RunPreprocessor:
		{
			const char* src = LangC_Preprocess(file);
			if (!src)
				break;
			
			if (!OS_WriteWholeFile(NullTerminateString(output_file), src, SB_Len(src)))
			{
				Print("error: could not open output file.\n");
			}
		} break;
	}
}
