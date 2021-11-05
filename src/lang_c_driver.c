// NOTE(ljre): This is the default compiler driver.

internal void
LangC_DefaultDriver_PrintHelp(void)
{
	Print("Our C Compiler -- help:\n"
		  "usage: occ [FILE | FLAG] ...\n"
		  "\n"
		  "flags:\n"
		  "\t[ -help ]\n"
		  "\t[ --help ] Shows this help text and returns 1.\n"
		  "\t[ -o<file>  ]\n"
		  "\t[ -o <file> ] Changes the output file (defaults to \"a.out\").\n"
		  "\t[ -E ] Runs the preprocessor on the single input file.\n"
		  "\n");
}

internal int32
LangC_DefaultDriver(int32 argc, const char** argv)
{
	bool32 result = 0;
	
	LangC_CompilerOptions options = {
		.mode = LangC_InvokationMode_BuildToExecutable,
		.output_file = Str("a.out"),
	};
	
	//~ NOTE(ljre): Setup system include directory
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
			options.include_dirs_count = 1;
			
			uintsize len = last_slash_index+1 + sizeof include;
			char* path = PushMemory(len + 1);
			
			memcpy(path, global_my_path.data, last_slash_index+1);
			memcpy(path + last_slash_index+1, include, sizeof include);
			path[len] = 0;
			
			options.include_dirs[0] = StrMake(path, len + 1);
		}
	}
	
	//~ NOTE(ljre): Parse parameters
	if (argc < 2)
	{
		LangC_DefaultDriver_PrintHelp();
		return 1;
	}
	
	StringList* last_input_file = NULL;
	
	for (const char** arg = argv + 1; *arg; ++arg)
	{
		if (arg[0][0] == '-')
		{
			const char* flag = arg[0] + 1;
			
			if (MatchCString(flag, "o", 1))
			{
				if (flag[1])
					options.output_file = StrFrom(flag + 1);
				else if (arg[1])
				{
					const char* name = arg[1];
					++arg;
					options.output_file = StrFrom(name);
				}
				else
				{
					result = 1;
					Print("error: expected file name after '-o' flag.\n");
				}
			}
			else if (MatchCString(flag, "E", 1))
			{
				options.mode = LangC_InvokationMode_RunPreprocessor;
			}
			else if (MatchCString(flag, "-help", 5) || MatchCString(flag, "--help", 6))
			{
				LangC_DefaultDriver_PrintHelp();
				return 1;
			}
		}
		else
		{
			LangC_AddInputFile(&options.input_files, &last_input_file, StrFrom(arg[0]));
		}
	}
	
	if (!last_input_file)
	{
		Print("error: no input files\n");
		result = 1;
	}
	
	Assert(options.input_files);
	
	//~ NOTE(ljre): Build
	LangC_Context ctx = {
		.options = &options,
		.persistent_arena = Arena_Create(Gigabytes(64)),
		.stage_arena = Arena_Create(Gigabytes(2)),
		
		.abi = &(LangC_ABI) {
			.t_char = { 1, 0, true },
			.t_schar = { 1, 0, false },
			.t_uchar = { 1, 0, true },
			.t_short = { 2, 1, false },
			.t_ushort = { 2, 1, true },
			.t_int = { 4, 3, false },
			.t_uint = { 4, 3, true },
			.t_long = { 4, 3, false },
			.t_ulong = { 4, 3, true },
			.t_longlong = { 8, 7, false },
			.t_ulonglong = { 8, 7, true },
			.t_double = { 8, 7, false },
			.t_float = { 4, 3, false },
			
			.char_bit = 8,
			.index_sizet = 10,
			.index_ptrdifft = 9,
		},
	};
	
	switch (options.mode)
	{
		case LangC_InvokationMode_BuildToExecutable:
		{
			for (StringList* it = options.input_files; it; it = it->next)
			{
				bool32 ok = true;
				
				ok = ok && LangC_Preprocess(&ctx, it->value);
				LangC_FlushWarnings(&ctx);
				Arena_Clear(ctx.stage_arena);
				
				ok = ok && LangC_ParseFile(&ctx); Arena_Clear(ctx.stage_arena);
				ok = ok && LangC_ResolveAst(&ctx); Arena_Clear(ctx.stage_arena);
				
				LangC_FlushWarnings(&ctx);
				
				ok = ok && LangC_GenerateCode(&ctx), Arena_Clear(ctx.stage_arena);
				// TODO
			}
		} break;
		
		case LangC_InvokationMode_RunPreprocessor:
		{
			LangC_Preprocess(&ctx, options.input_files->value);
			LangC_FlushWarnings(&ctx);
			Arena_Clear(ctx.stage_arena);
			
			if (!ctx.pre_source)
				break;
			
			if (options.output_file.size == 0)
			{
				Print("%s", ctx.pre_source);
			}
			else if (!OS_WriteWholeFile(NullTerminateString(options.output_file), ctx.pre_source, strlen(ctx.pre_source)))
			{
				Print("error: could not open output file.\n");
				result = 1;
			}
		} break;
	}
	
	Arena_Destroy(ctx.stage_arena);
	Arena_Destroy(ctx.persistent_arena);
	
	return result;
}
