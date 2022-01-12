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
		  "%C1  -j -j<N>         %C0 Number of worker threads. When specified, defaults to number of system threads.\n"
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
	uintsize input_file_count = 0;
	String output_file = StrNull;
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
	
	// NOTE(ljre): Those macros are handled internally, but a definition is still needed.
	C_PredefineMacro(&options, Str("__LINE__"));
	C_PredefineMacro(&options, Str("__FILE__"));
	
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
			PushToStringList(global_arena, &input_files, &last_input_file, StrFrom(arg[0]));
			++input_file_count;
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
			options.mode = C_CompilerMode_InputsToPreprocessed;
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
				String def = Arena_SPrintf(global_arena, "%S %s", name_end - name_begin, name_begin, value_begin);
				C_PredefineMacro(&options, def);
			}
			else
			{
				C_PredefineMacro(&options, StrMake(name_begin, name_end - name_begin));
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
	C_PredefineMacro(&options, Str("__STDC__ 1"));
	C_PredefineMacro(&options, Str("__STDC_HOSTED__ 1"));
	C_PredefineMacro(&options, Str("__STDC_VERSION__ 199901L"));
	C_PredefineMacro(&options, Str("__x86_64 1"));
	C_PredefineMacro(&options, Str("__x86_64__ 1"));
	C_PredefineMacro(&options, Str("_M_AMD64 1"));
	C_PredefineMacro(&options, Str("_M_X64 1"));
	C_PredefineMacro(&options, Str("_WIN32 1"));
	C_PredefineMacro(&options, Str("_WIN64 1"));
	C_PredefineMacro(&options, Str("__OCC__ 1"));
	
	// NOTE(ljre): Polyfills
	C_PredefineMacro(&options, Str("__int64 long long"));
	C_PredefineMacro(&options, Str("__int32 int"));
	C_PredefineMacro(&options, Str("__int16 short"));
	C_PredefineMacro(&options, Str("__int8 char"));
	C_PredefineMacro(&options, Str("__inline inline"));
	C_PredefineMacro(&options, Str("__inline__ inline"));
	C_PredefineMacro(&options, Str("__restrict restrict"));
	C_PredefineMacro(&options, Str("__restrict__ restrict"));
	C_PredefineMacro(&options, Str("__const const"));
	C_PredefineMacro(&options, Str("__const__ const"));
	C_PredefineMacro(&options, Str("__volatile volatile"));
	C_PredefineMacro(&options, Str("__volatile__ volatile"));
	C_PredefineMacro(&options, Str("__attribute __attribute__"));
	//C_PredefineMacro(&options, Str("__forceinline inline"));
	//C_PredefineMacro(&options, Str("__attribute__(...)"));
	//C_PredefineMacro(&options, Str("__declspec(...)"));
	C_PredefineMacro(&options, Str("__builtin_offsetof(_Type, _Field) (&((_Type*)0)->_Field)"));
	C_PredefineMacro(&options, Str("__builtin_va_list void*"));
	
#if 1
	// NOTE(ljre): MINGW macros
	C_PredefineMacro(&options, Str("_MSC_VER 1910"));
	C_PredefineMacro(&options, Str("_MSC_FULL_VER 191025017"));
	//C_PredefineMacro(&options, Str("__MINGW_ATTRIB_DEPRECATED_STR(x)"));
	//C_PredefineMacro(&options, Str("__MINGW_ATTRIB_NONNULL(x)"));
	//C_PredefineMacro(&options, Str("__MINGW_NOTHROW"));
	//C_PredefineMacro(&options, Str("__mingw_ovr"));
#endif
	
	//~ NOTE(ljre): Build.
	if (input_file_count == 1)
	{
		C_ThreadWork work = {
			.ctx = {
				.options = &options,
				.persistent_arena = Arena_Create(Gigabytes(32)),
				.stage_arena = Arena_Create(Gigabytes(8)),
				.input_file = input_files->value,
				.output_file = output_file,
				
				.abi = &abi,
			},
		};
		
		C_ThreadDoWork(&work);
		
		Arena_Destroy(work.ctx.stage_arena);
		Arena_Destroy(work.ctx.persistent_arena);
		
		result = (work.ctx.error_count != 0);
	}
	else
	{
		C_ThreadWorkList* worklist = PushMemory(sizeof(*worklist));
		
		worklist->works = PushMemory(sizeof(C_ThreadWork) * input_file_count);
		worklist->work_count = input_file_count;
		worklist->work_done = 0;
		worklist->lock = OS_CreateRWLock();
		worklist->accumulated_error_count = 0;
		
		StringList* it = input_files;
		for (uintsize i = 0; i < input_file_count; ++i)
		{
			String output_file = StrNull;
			String input_file = it->value;
			it = it->next;
			
			switch (options.mode)
			{
				case C_CompilerMode_InputsToPreprocessed:
				{
					break;
					String dir;
					String filename;
					
					StringParsePath(input_file, &dir, &filename, NULL);
					
					Arena_SPrintf(global_arena, "%S%S.i%0", StrFmt(dir), StrFmt(filename));
				} break;
			}
			
			worklist->works[i].ctx = (C_Context) {
				.options = &options,
				.persistent_arena = Arena_Create(Gigabytes(32)),
				.stage_arena = Arena_Create(Gigabytes(8)),
				.input_file = input_file,
				.output_file = output_file,
				
				.abi = &abi,
			};
		}
		
		int32 thread_count = Min(2, input_file_count); // TODO(ljre): Request system info or take -j flag.
		OS_Thread* threads = PushMemory(sizeof(OS_Thread) * thread_count);
		
		for (int32 i = 0; i < thread_count; ++i)
			threads[i] = OS_CreateThread(C_WorkerThreadProc, worklist);
		for (int32 i = 0; i < thread_count; ++i)
		{
			int32 thread_result = OS_JoinThread(threads[i]);
			
			(void)thread_result;
		}
		
		// TODO(ljre): Stuff if we need to reduce all the input files results.
		
		for (uintsize i = 0; i < input_file_count; ++i)
		{
			Arena_Destroy(worklist->works[i].ctx.stage_arena);
			Arena_Destroy(worklist->works[i].ctx.persistent_arena);
		}
		
		result = (worklist->accumulated_error_count != 0);
	}
	
	// TODO(ljre): Remove this.
	if (result == 0)
		Print("\nCompilation status: %C4OK!%C0\n");
	else
		Print("\nCompilation status: %C2Failed!%C0\n");
	
	return result;
}
