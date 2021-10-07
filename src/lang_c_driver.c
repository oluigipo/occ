internal void
LangC_DefaultDriver_PrintHelp(void)
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

internal bool32
LangC_DefaultDriver(int32 argc, const char** argv)
{
	bool32 result = true;
	
	// NOTE(ljre): Setup system include directory
	{
		static const char include[] = "include/";
		
		int32 last_slash_index = -1;
		for (int32 i = 0; i < global_my_path.size; ++i)
		{
			if (global_my_path.data[i] == '/')
				last_slash_index = i;
		}
		
		if (last_slash_index != -1)
		{
			LangC_options.include_dirs_count = 1;
			
			uintsize len = last_slash_index+1 + sizeof include;
			char* path = PushMemory(len + 1);
			
			memcpy(path, global_my_path.data, last_slash_index+1);
			memcpy(path + last_slash_index+1, include, sizeof include);
			path[len] = 0;
			
			LangC_options.include_dirs[0] = StrMake(path, len + 1);
		}
	}
	
	// NOTE(ljre): Parse parameters
	if (argc < 2)
	{
		LangC_DefaultDriver_PrintHelp();
		return false;
	}
	
	LangC_options.mode = LangC_InvokationMode_BuildToExecutable;
	LangC_options.output_file = Str("a.out");
	StringList* last_input_file = NULL;
	
	for (const char** arg = argv + 1; *arg; ++arg)
	{
		if (arg[0][0] == '-')
		{
			const char* flag = arg[0] + 1;
			
			if (MatchCString(flag, "o", 1))
			{
				if (flag[1])
					LangC_options.output_file = StrFrom(flag + 1);
				else if (arg[1])
				{
					const char* name = arg[1];
					++arg;
					LangC_options.output_file = StrFrom(name);
				}
				else
				{
					result = false;
					Print("error: expected file name after '-o' flag.\n");
				}
			}
			else if (MatchCString(flag, "E", 1))
			{
				LangC_options.mode = LangC_InvokationMode_RunPreprocessor;
			}
			else if (MatchCString(flag, "-help", 5))
			{
				LangC_DefaultDriver_PrintHelp();
				return false;
			}
		}
		else
		{
			LangC_AddInputFile(&last_input_file, StrFrom(arg[0]));
		}
	}
	
	if (!last_input_file)
	{
		Print("error: no input files\n");
		result = false;
	}
	
	return result;
}
