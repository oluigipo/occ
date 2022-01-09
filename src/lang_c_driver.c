// NOTE(ljre): This is the default compiler driver.

internal void
C_DefaultDriver_PrintHelp(void)
{
	Print("Our C Compiler v" C_VERSION_STR " -- help:\n"
		  "%C2usage:%C0 occ [FILE | FLAG] ...\n"
		  "\n"
		  "%C2flags:%C0\n"
		  "%C1  -help            %C0 \n"
		  "%C1  --help           %C0 Shows this help text and returns 1.\n"
		  "%C1  -D<name>         %C0 \n"
		  "%C1  -D<name>=<value> %C0 Defines a Macro.\n"
		  "%C1  -E               %C0 Runs the preprocessor on the single input file.\n"
		  "%C1  -I<dir>          %C0 Adds an include directory.\n"
		  "%C1  -o<file>         %C0 Changes the output file. (defaults to \"a.out\")\n"
		  "\n");
}

internal int32
C_DefaultDriver(int32 argc, const char** argv)
{
	int32 result = 0;
	
	C_CompilerOptions options = { 0 };
	StringList* input_files = NULL;
	StringList* last_input_file = NULL;
	String output_file = StrInit("a.out");
	int32 mode = 0;
	C_ABI abi = {
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
		.t_ptr = { 8, 7, true },
		.t_bool = { 1, 0, true },
		
		.char_bit = 8,
		.index_sizet = 10,
		.index_ptrdifft = 9,
	};
	
	C_Context* ctx = &(C_Context) {
		.options = &options,
		.persistent_arena = Arena_Create(Gigabytes(32)),
		.stage_arena = Arena_Create(Gigabytes(8)),
		
		.abi = &abi,
	};
	
	Map_Init(&ctx->pp.obj_macros, sizeof(C_Macro*), sizeof(C_Macro*) * 100000);
	Map_Init(&ctx->pp.func_macros, sizeof(C_Macro*), sizeof(C_Macro*) * 10000);
	
	//~ NOTE(ljre): Setup system include directory
	{
		static const char include[] = "_include-mingw/";
		//static const char include[] = "include/";
		
		int32 last_slash_index = -1;
		for (int32 i = 0; i < global_my_path.size; ++i)
		{
			if (global_my_path.data[i] == '/')
				last_slash_index = i;
		}
		
		if (last_slash_index != -1)
		{
			StringList* p = PushMemory(sizeof(*p));
			
			uintsize len = last_slash_index+1 + sizeof include;
			char* path = PushMemory(len + 1);
			
			OurMemCopy(path, global_my_path.data, last_slash_index+1);
			OurMemCopy(path + last_slash_index+1, include, sizeof include);
			path[len] = 0;
			
			p->value = StrMake(path, len + 1);
			options.include_dirs = p;
		}
	}
	
	//~ NOTE(ljre): Parse parameters
	if (argc < 2)
	{
		C_DefaultDriver_PrintHelp();
		return 1;
	}
	
	for (const char** arg = argv + 1; *arg; ++arg)
	{
		if (arg[0][0] != '-')
		{
			C_AddInputFile(&input_files, &last_input_file, StrFrom(arg[0]));
			continue;
		}
		
		const char* flag = arg[0] + 1;
		String strflag = StrFrom(flag);
		
		if (StringStartsWith(strflag, "o"))
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
				result = 1;
				Print("error: expected file name after '-o' flag.\n");
			}
		}
		else if (StringStartsWith(strflag, "E"))
		{
			mode = 1;
		}
		else if (StringStartsWith(strflag, "I"))
		{
			++flag;
			if (!*flag)
			{
				flag = *++arg;
				if (!*flag)
					continue;
			}
			
			uintsize len = OurStrLen(flag);
			uintsize needed_len = len;
			if (flag[len] != '/' || flag[len] != '\\')
				needed_len += 1;
			
			char* dir = PushMemory(needed_len + 1);
			OurMemCopy(dir, flag, len);
			
			if (needed_len > len)
				dir[len] = '/';
			
			for (int32 i = 0; i < len; ++i)
			{
				if (dir[i] == '\\')
					dir[i] = '/';
			}
			
			dir[needed_len] = 0;
			
			StringList* p = PushMemory(sizeof(*p));
			p->value = StrMake(dir, needed_len+1);
			p->next = options.include_dirs;
			
			options.include_dirs = p;
		}
		else if (StringStartsWith(strflag, "D"))
		{
			++flag;
			
			if (!*flag)
			{
				flag = *++arg;
				if (!*flag)
					continue;
			}
			
			const char* name_begin = flag;
			while (C_IsIdentChar(*flag))
				++flag;
			const char* name_end = flag;
			
			if (*flag == '=')
			{
				const char* value_begin = ++flag;
				char* mem = Arena_End(global_arena);
				uintsize len = Arena_Printf(global_arena, "%S %s", name_end - name_begin, name_begin, value_begin);
				C_DefineMacro(ctx, StrMake(mem, len), NULL);
			}
			else
			{
				C_DefineMacro(ctx, StrMake(name_begin, name_end - name_begin), NULL)->persistent = true;
			}
		}
		else if (StringStartsWith(strflag, "help") || StringStartsWith(strflag, "-help"))
		{
			C_DefaultDriver_PrintHelp();
			return 1;
		}
	}
	
	if (!last_input_file)
	{
		Print("error: no input files\n");
		return 1;
	}
	
	Assert(input_files);
	
	//~ NOTE(ljre): Default predefined macros.
	C_DefineMacro(ctx, Str("__STDC__ 1"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__STDC_HOSTED__ 1"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__STDC_VERSION__ 199901L"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__x86_64 1"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__x86_64__ 1"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("_M_AMD64 1"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("_M_X64 1"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("_WIN32 1"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("_WIN64 1"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__OCC__ 1"), NULL)->persistent = true;
	
	// NOTE(ljre): Polyfills
	C_DefineMacro(ctx, Str("__int64 long long"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__int32 int"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__int16 short"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__int8 char"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__inline inline"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__inline__ inline"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__restrict restrict"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__restrict__ restrict"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__const const"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__const__ const"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__volatile volatile"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__volatile__ volatile"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__attribute __attribute__"), NULL)->persistent = true;
	//C_DefineMacro(ctx, Str("__forceinline inline"), NULL)->persistent = true;
	//C_DefineMacro(ctx, Str("__attribute__(...)"), NULL)->persistent = true;
	//C_DefineMacro(ctx, Str("__declspec(...)"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__builtin_offsetof(_Type, _Field) (&((_Type*)0)->_Field)"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("__builtin_va_list void*"), NULL)->persistent = true;
	
#if 1
	// NOTE(ljre): MINGW macros
	C_DefineMacro(ctx, Str("_MSC_VER 1910"), NULL)->persistent = true;
	C_DefineMacro(ctx, Str("_MSC_FULL_VER 191025017"), NULL)->persistent = true;
	//C_DefineMacro(ctx, Str("__MINGW_ATTRIB_DEPRECATED_STR(x)"), NULL)->persistent = true;
	//C_DefineMacro(ctx, Str("__MINGW_ATTRIB_NONNULL(x)"), NULL)->persistent = true;
	//C_DefineMacro(ctx, Str("__MINGW_NOTHROW"), NULL)->persistent = true;
	//C_DefineMacro(ctx, Str("__mingw_ovr"), NULL)->persistent = true;
#endif
	
	//~ NOTE(ljre): Build.
	switch (mode)
	{
		// NOTE(ljre): Build to executable
		case 0:
		{
			for (StringList* it = input_files; it; it = it->next)
			{
				bool32 ok = true;
				ctx->tokens = Arena_Push(ctx->persistent_arena, sizeof(*ctx->tokens));
				
				// NOTE(ljre): 'C_Preprocess' pushes warnings to the stage arena, so we need to flush
				//             before clearing it.
				ok = ok && C_Preprocess(ctx, it->value);
				C_FlushWarnings(ctx);
				Arena_Clear(ctx->stage_arena);
				
				ok = ok && C_ParseFile(ctx); Arena_Clear(ctx->stage_arena);
				//ok = ok && C_ResolveAst(ctx); Arena_Clear(ctx->stage_arena);
				
				//C_FlushWarnings(ctx);
				
				//ok = ok && C_GenIr(ctx); Arena_Clear(ctx->stage_arena);
				// TODO
			}
		} break;
		
		// NOTE(ljre): Run Preprocessor
		case 1:
		{
			C_Preprocess(ctx, input_files->value);
			C_FlushWarnings(ctx);
			Arena_Clear(ctx->stage_arena);
			
			if (!ctx->pre_source)
				break;
			
			if (output_file.size == 0)
			{
				Print("%s", ctx->pre_source);
			}
			else if (!OS_WriteWholeFile(Arena_NullTerminateString(ctx->stage_arena, output_file), ctx->pre_source, OurStrLen(ctx->pre_source), ctx->stage_arena))
			{
				Print("error: could not open output file.\n");
				result = 1;
			}
		} break;
	}
	
	Print("\n====== Memory Usage:\nStage Commited: \t%z bytes\nPersistent Offset:\t%z bytes\n",
		  ctx->stage_arena->commited, ctx->persistent_arena->offset);
	
	//~ NOTE(ljre): Clean-up.
	Arena_Destroy(ctx->stage_arena);
	Arena_Destroy(ctx->persistent_arena);
	
	// TODO(ljre): Remove this.
	if (result == 0)
		Print("\nCompilation status: %C4OK!%C0\n");
	else
		Print("\nCompilation status: %C2Failed!%C0\n");
	
	return result;
}
