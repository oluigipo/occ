/*
*
* TODO:
*    - Parsing standard C99 (halfway through);
*    - Type-checking;
*    - Code analysis for warnings:
*        - "a<<b + 10", misleading spacing between operations;
*        - constness;
*        - 
*    - Codegen with Succ3s's backend;
*    - Flag (-I): Add include directory;
*    - Flag (-D): Define a macro;
*    - Flag (-U): Undefine a macro;
*    - Flag (-v): Print to stdout verbose log;
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

#include "lang_c_definitions.h"

// NOTE(ljre): LangC_options should never be modified after the compiler driver finished running.
internal LangC_CompilerOptions LangC_options = { 0 };
internal int32 LangC_error_count = 0;
internal LangC_QueuedWarning* LangC_queued_warning = &(LangC_QueuedWarning) { 0 };
internal LangC_QueuedWarning* LangC_last_queued_warning = NULL;

internal void
LangC_AddInputFile(StringList** last, String str)
{
	if (!*last)
	{
		LangC_options.input_files = *last = PushMemory(sizeof **last);
	}
	else
	{
		*last = (*last)->next = PushMemory(sizeof **last);
	}
	
	(*last)->value = str;
}

internal void
LangC_PushWarning(LangC_Warning warning, const char* message)
{
	LangC_last_queued_warning = LangC_last_queued_warning->next = PushMemory(sizeof(LangC_QueuedWarning));
	LangC_last_queued_warning->warning = warning;
	LangC_last_queued_warning->to_print = message;
}

internal void
LangC_FlushWarnings(void)
{
	LangC_QueuedWarning* warning = LangC_queued_warning->next;
	
	while (warning)
	{
		Print("%s\n", warning->to_print);
		warning = warning->next;
	}
	
	LangC_queued_warning->next = NULL;
	LangC_last_queued_warning = LangC_queued_warning;
}

#include "lang_c_lexer.c"
#include "lang_c_preprocessor.c"
#include "lang_c_parser.c"
#include "lang_c_type.c"
#include "lang_c_analysis.c"

#include "lang_c_driver.c"

internal int32
LangC_Main(int32 argc, const char** argv)
{
	int32 result = 0;
	LangC_FlushWarnings();
	
	if (!LangC_DefaultDriver(argc, argv))
		return 1;
	
	LangC_Context ctx = {
		.options = &LangC_options,
		.nodes_arena = Arena_Create(Gigabytes(2)),
	};
	
	switch (LangC_options.mode)
	{
		case LangC_InvokationMode_BuildToExecutable:
		{
			for (StringList* it = LangC_options.input_files; it; it = it->next)
			{
				const char* src = LangC_Preprocess(it->value);
				if (!src)
					break;
				
				LangC_ParseFile(&ctx, src);
				
				LangC_FlushWarnings();
				// TODO
			}
		} break;
		
		case LangC_InvokationMode_RunPreprocessor:
		{
			const char* src = LangC_Preprocess(LangC_options.input_files->value);
			LangC_FlushWarnings();
			
			if (!src)
				break;
			
			if (LangC_options.output_file.size == 0)
			{
				Print("%.*s", src, SB_Len(src));
			}
			else if (!OS_WriteWholeFile(NullTerminateString(LangC_options.output_file), src, SB_Len(src)))
			{
				Print("error: could not open output file.\n");
				result = 1;
			}
		} break;
	}
	
	return result;
}
