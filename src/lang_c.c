#define LangC_MAX_INCLUDE_DIRS 64

internal char LangC_include_dirs[LangC_MAX_INCLUDE_DIRS][MAX_PATH_SIZE];
internal uintsize LangC_include_dirs_count;
internal int32 LangC_error_count = 0;

#include "lang_c_definitions.h"
#include "lang_c_lexer.c"
#include "lang_c_preprocessor.c"
#include "lang_c_parser.c"

internal void
LangC_Init(int32 argc, char** argv)
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
