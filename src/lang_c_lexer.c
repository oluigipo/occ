struct LangC_Token
{
	int32 line, col;
	LangC_TokenKind kind;
	String as_string;
	
	union
	{
		int64 value_int;
		uint64 value_uint;
		String value_str;
		String value_ident;
		float value_float;
		double value_double;
	};
} typedef LangC_Token;

struct LangC_Macro typedef LangC_Macro;
struct LangC_Macro
{
	LangC_Macro* next;
	LangC_Macro* previous;
	
	uint64 hash;
	String name;
	const char* def;
	
	bool32 is_func;
};

struct LangC_LexerMacroContext typedef LangC_LexerMacroContext;
struct LangC_LexerMacroContext
{
	LangC_LexerMacroContext* next;
	LangC_Macro* saved_last_macro;
	
	LangC_Macro* expanding_macro;
	
	const char* head;
};

struct LangC_LexerFile typedef LangC_LexerFile;
struct LangC_LexerFile
{
	const char* source;
	const char* head;
	const char* previous_head;
	int32 line, col;
	
	LangC_LexerFile* next_included;
	LangC_LexerFile* previous_included;
	
	char path[MAX_PATH_SIZE];
	
	bool32 active;
};

struct LangC_Lexer typedef LangC_Lexer;
struct LangC_Lexer
{
	LangC_Token token;
	LangC_LexerFile file;
	int32 ifdef_failed_nesting;
	int32 ifdef_nesting;
	bool16 ifdef_already_matched;
	bool16 error;
	bool16 newline_as_token;
	bool16 dont_expand_macros; // 'dont' because ZII
	
	LangC_Macro* first_macro;
	LangC_Macro* last_macro;
	
	LangC_LexerMacroContext* macroctx_first;
	LangC_LexerMacroContext* macroctx_last;
};

internal void LangC_ConsumeEofState(LangC_Lexer* ctx);
internal void LangC_PushMacroContext(LangC_Lexer* ctx, const char** phead, LangC_Macro* macro);
internal bool32 LangC_AssertToken(LangC_Lexer* ctx, LangC_TokenKind kind);
internal void LangC_NextToken(LangC_Lexer* ctx);
internal void LangC_TokenizeSimpleTokens(LangC_Lexer* ctx, const char** phead);
internal int32 LangC_EvalPreProcessorExpr(LangC_Lexer* ctx);

internal int32
LangC_InitLexerFile(LangC_LexerFile* lexfile, const char* path)
{
	// bora fazer isso
	struct CachedFile
	{
		uint64 hash;
		const char* contents;
	};
	
	static struct CachedFile cached_files[1024];
	static int32 cached_files_count;
	
	const char* data = NULL;
	uint64 search_hash = SimpleHashNullTerminated(path);
	int32 i = 0;
	
	for (; i < cached_files_count; ++i)
	{
		if (cached_files[i].hash == search_hash)
		{
			data = cached_files[i].contents;
			break;
		}
	}
	
	if (!data)
	{
		data = OS_ReadWholeFile(path);
		if (!data)
			return -1;
		
		if (cached_files_count >= ArrayLength(cached_files))
			cached_files_count = ArrayLength(cached_files)/2; // fuck it
		
		i = cached_files_count++;
		
		cached_files[i].hash = search_hash;
		cached_files[i].contents = data;
	}
	
	lexfile->source = data;
	lexfile->head = lexfile->source;
	lexfile->previous_head = lexfile->head;
	lexfile->line = lexfile->col = 1;
	lexfile->active = true;
	
	int32 path_len = strlen(path);
	memcpy(lexfile->path, path, path_len);
	lexfile->path[path_len] = 0;
	
	return 0;
}

internal inline bool32
LangC_IsNumeric(char ch, int32 base)
{
	return (base == 2 && (ch == '0' || ch == '1') ||
			(ch >= '0' && ch <= '9' || (base == 16 && (ch >= 'a' && ch <= 'f' || ch >= 'A' && ch <= 'F'))));
}

internal inline bool32
LangC_IsAlpha(char ch)
{
	return (ch >= 'A' & ch <= 'Z') | (ch >= 'a' & ch <= 'z');
}

internal inline bool32
LangC_IsIdentChar(char ch)
{
	return ch == '_' || (ch >= '0' && ch <= '9') || LangC_IsAlpha(ch);
}

internal void
LangC_LexerError(LangC_Lexer* ctx, const char* fmt, ...)
{
	LangC_LexerFile* lexfile = &ctx->file;
	ctx->error = true;
	
	Print("\n");
	
	while (lexfile->next_included && lexfile->next_included->active)
	{
		Print("%s(%i:%i): in included file\n", lexfile->path, lexfile->line, lexfile->col);
		lexfile = lexfile->next_included;
	}
	
	Print("%s(%i:%i): error: ", lexfile->path, lexfile->line, lexfile->col);
	va_list args;
	va_start(args, fmt);
	PrintVarargs(fmt, args);
	va_end(args);
	Print("\n");
}

internal void
LangC_LexerWarning(LangC_Lexer* ctx, const char* fmt, ...)
{
	LangC_LexerFile* lexfile = &ctx->file;
	
	Print("\n");
	
	while (lexfile->next_included && lexfile->next_included->active)
	{
		Print("%s(%i:%i): in included file\n", lexfile->path, lexfile->line, lexfile->col);
		lexfile = lexfile->next_included;
	}
	
	Print("%s(%i:%i): warning: ", lexfile->path, lexfile->line, lexfile->col);
	va_list args;
	va_start(args, fmt);
	PrintVarargs(fmt, args);
	va_end(args);
	Print("\n");
}

internal String
LangC_ConcatStrings(String left, String right)
{
	uintsize size = left.size + right.size;
	
	char* data = PushMemory(size);
	memcpy(data, left.data, left.size);
	memcpy(data + left.size, right.data, right.size);
	
	return (String) {
		.size = size,
		.data = data,
	};
}

internal const char*
LangC_CurrentWorkingPath(LangC_Lexer* ctx)
{
	LangC_LexerFile* lexfile = &ctx->file;
	
	while (lexfile->next_included && lexfile->next_included->active)
		lexfile = lexfile->next_included;
	
	return lexfile->path;
}

internal LangC_LexerFile*
LangC_CurrentLexerFile(LangC_Lexer* ctx)
{
	LangC_LexerFile* lexfile = &ctx->file;
	if (!lexfile->active)
		return NULL;
	
	while (lexfile->next_included && lexfile->next_included->active)
	{
		lexfile = lexfile->next_included;
	}
	
	return lexfile;
}

internal void
LangC_IgnoreWhitespaces(const char** phead, bool32 newline)
{
	for (;;)
	{
		if (!(*phead)[0])
			break;
		
		if ((*phead)[0] == '/' && (*phead)[1] == '/')
		{
			(*phead) += 2;
			
			while ((*phead)[0] && (*phead)[0] != '\n')
				++(*phead);
		}
		else if ((*phead)[0] == '/' && (*phead)[1] == '*')
		{
			(*phead) += 2;
			
			while ((*phead)[0] && ((*phead)[-2] != '*' || (*phead)[-1] != '/'))
				++(*phead);
		}
		else if ((*phead)[0] == ' ' || (*phead)[0] == '\t' || (*phead)[0] == '\r' ||
				 (newline && (*phead)[0] == '\n'))
		{
			++(*phead);
		}
		else if ((*phead)[0] == '\\' && (*phead)[1] == '\n')
		{
			(*phead) += 2;
		}
		else
		{
			break;
		}
	}
}

internal void
LangC_IgnoreUntilPairOfNestedOr(const char** phead, char open, char close, char exception)
{
	int32 paren_nesting = 1;
	
	do
	{
		if (!**phead)
			break;
		
		if (**phead == close)
			--paren_nesting;
		else if (**phead == open)
			++paren_nesting;
		else if (paren_nesting == 1 && **phead == exception)
			break;
	}
	while (paren_nesting > 0 && ++*phead);
}

internal void
LangC_IgnoreUntilEndOfLine(LangC_LexerFile* lexfile)
{
	while (lexfile->head[0] && (lexfile->head[0] != '\n' || lexfile->head[-1] == '\\'))
		++lexfile->head;
}

internal String
LangC_TokenizeIdent(const char** phead)
{
	const char* begin = *phead;
	while (LangC_IsIdentChar(**phead))
		++*phead;
	
	return (String) {
		.data = begin,
		.size = (uintsize)(*phead - begin),
	};
}

internal String
LangC_TokenizeIdentRanged(const char** phead, const char* end)
{
	const char* begin = *phead;
	while (*phead < end && LangC_IsIdentChar(**phead))
		++*phead;
	
	return (String) {
		.data = begin,
		.size = (uintsize)(*phead - begin),
	};
}

internal void
LangC_UpdateFileLineAndCol(LangC_LexerFile* lexfile)
{
	for (; lexfile->previous_head < lexfile->head; ++lexfile->previous_head)
	{
		switch (*lexfile->previous_head)
		{
			case '\n': ++lexfile->line;
			case '\r': lexfile->col = 1; break;
			
			default: ++lexfile->col; break;
		}
	}
}

internal void
LangC_DefineMacro(LangC_Lexer* ctx, String def)
{
	if (!ctx->last_macro)
	{
		if (!ctx->first_macro)
			ctx->first_macro = PushMemory(sizeof *ctx->first_macro);
		
		ctx->last_macro = ctx->first_macro;
	}
	else
	{
		if (!ctx->last_macro->next)
			ctx->last_macro->next = PushMemory(sizeof *ctx->last_macro->next);
		
		ctx->last_macro->next->previous = ctx->last_macro;
		ctx->last_macro = ctx->last_macro->next;
	}
	
	const char* end = def.data + def.size;
	
	const char* name_begin = def.data;
	const char* name_end = name_begin;
	
	while (name_end < end && LangC_IsIdentChar(*name_end))
		++name_end;
	
	String name = {
		.size = (uintsize)(name_end - name_begin),
		.data = name_begin,
	};
	
	ctx->last_macro->is_func = (name_end < end && *name_end == '(');
	ctx->last_macro->def = NullTerminateString(def);
	ctx->last_macro->name = name;
	ctx->last_macro->hash = SimpleHash(name);
}

internal void
LangC_UndefineMacro(LangC_Lexer* ctx, String name)
{
	if (!ctx->first_macro)
		return;
	
	uint64 search_hash = SimpleHash(name);
	if (ctx->first_macro->hash == search_hash)
	{
		ctx->first_macro = ctx->first_macro->next;
		return;
	}
	
	LangC_Macro* current = ctx->first_macro;
	while (current->next && current->next != ctx->last_macro)
	{
		if (search_hash == current->hash)
			current->next = current->next->next;
		
		current = current->next;
	}
}

// Values for 'func':
//     0 = false,
//     1 = true,
//     2 = doesnt matter,
//     3 = want function, but it's ok to not be
internal LangC_Macro*
LangC_FindMacro(LangC_Lexer* ctx, String name, int32 func)
{
	if (func == 3)
	{
		LangC_Macro* result = LangC_FindMacro(ctx, name, 1);
		
		if (!result)
			result = LangC_FindMacro(ctx, name, 0);
		
		return result;
	}
	
	// NOTE(ljre): This is a backwards search.
	
	uint64 search_hash = SimpleHash(name);
	LangC_Macro* current = ctx->last_macro;
	
	if (current)
	{
		do
		{
			if (search_hash == current->hash && (func == 2 || current->is_func == func))
				return current;
		}
		while (current != ctx->first_macro && (current = current->previous));
	}
	
	return NULL;
}

internal bool32
LangC_IsIdentMacroParameter(String ident, LangC_Macro* macro)
{
	if (!macro->is_func)
		return false;
	
	const char* head = macro->def;
	LangC_TokenizeIdent(&head);
	
	assert(*head == '(');
	++head;
	
	if (*head != ')')
	{
		do
		{
			LangC_IgnoreWhitespaces(&head, false);
			String param = LangC_TokenizeIdent(&head);
			
			if (CompareString(ident, param) == 0)
				return true;
			
			LangC_IgnoreWhitespaces(&head, false);
		}
		while (*head == ',' && ++head);
	}
	
	return false;
}

internal void
LangC_PopMacroContext(LangC_Lexer* ctx)
{
	LangC_LexerMacroContext* macroctx = ctx->macroctx_first;
	
	if (ctx->macroctx_first == ctx->macroctx_last)
	{
		ctx->last_macro = macroctx->saved_last_macro;
		ctx->macroctx_last = ctx->macroctx_last = NULL;
	}
	else
	{
		while (macroctx->next != ctx->macroctx_last)
			macroctx = macroctx->next;
		
		ctx->last_macro = macroctx->next->saved_last_macro;
		ctx->macroctx_last = macroctx;
	}
}

internal bool32
LangC_IsCurrentMacroContext(LangC_Lexer* ctx, LangC_LexerMacroContext* macroctx)
{
	if (!ctx->macroctx_last)
		return false;
	
	LangC_LexerMacroContext* current = ctx->macroctx_first;
	do
	{
		if (current == macroctx)
			return true;
	}
	while (current != ctx->macroctx_last && (current = current->next));
	
	return false;
}

internal void
LangC_ExpandMacroToStretchyBuffer(LangC_Lexer* ctx, LangC_Macro* macro, char** buf)
{
	LangC_PushMacroContext(ctx, NULL, macro);
	LangC_LexerMacroContext* macroctx = ctx->macroctx_last;
	
	while (LangC_ConsumeEofState(ctx), LangC_IsCurrentMacroContext(ctx, macroctx))
	{
		LangC_NextToken(ctx);
		String str = ctx->token.as_string;
		
		SB_PushArray(*buf, str.size, str.data);
		SB_Push(*buf, ' ');
	}
}

// NOTE(ljre): returned string is null-terminated as well
internal String
LangC_ExpandCurrentMacroParameters(LangC_Lexer* ctx, const char* begin, const char* end)
{
	char* buf = NULL;
	SB_ReserveAtLeast(buf, (uintsize)(end - begin) + 1);
	
	if (!ctx->macroctx_last)
	{
		SB_PushArray(buf, (uintsize)(end - begin), begin);
	}
	else
	{
		LangC_LexerMacroContext* macroctx = ctx->macroctx_last;
		
		while (begin < end)
		{
			if (LangC_IsIdentChar(*begin))
			{
				String ident = LangC_TokenizeIdentRanged(&begin, end);
				
				if (LangC_IsIdentMacroParameter(ident, macroctx->expanding_macro))
				{
					LangC_Macro* macro = LangC_FindMacro(ctx, ident, false);
					LangC_ExpandMacroToStretchyBuffer(ctx, macro, &buf);
				}
				else
				{
					SB_PushArray(buf, ident.size, ident.data);
				}
			}
			else
			{
				SB_Push(buf, *begin);
				++begin;
			}
		}
	}
	
	SB_Push(buf, 0);
	String result = {
		.size = SB_Len(buf),
		.data = buf,
	};
	
	return result;
}

// NOTE(ljre): 'phead' may be NULL if 'macro->is_func' is false.
internal void
LangC_PushMacroContext(LangC_Lexer* ctx, const char** phead, LangC_Macro* macro)
{
	LangC_Macro* saved_last_macro = ctx->last_macro;
	const char* macro_head = macro->def;
	
	// ignore macro name
	LangC_TokenizeIdent(&macro_head);
	
	if (macro->is_func)
	{
		assert(*macro_head == '(');
		++macro_head;
		
		// implementation shouldn't call this function if there's "macro type" mismatch
		assert(**phead == '(');
		++*phead;
		
		// NOTE(ljre): Expand current expanding macro parameters
		const char* head = *phead;
		{
			LangC_IgnoreUntilPairOfNestedOr(phead, '(', ')', 0);
			const char* end = *phead;
			
			String expanded_args = LangC_ExpandCurrentMacroParameters(ctx, head, end);
			head = expanded_args.data; // guaranteed to be null-terminated
		}
		
		if (*macro_head != ')')
		{
			do
			{
				LangC_IgnoreWhitespaces(&macro_head, false);
				LangC_IgnoreWhitespaces(&head, false);
				
				if (*macro_head == '.')
				{
					if (macro_head[1] != '.' || macro_head[2] != '.')
					{
						LangC_LexerError(ctx, "expected '...' for VA_ARGS.\n");
						continue;
					}
					
					macro_head += 3;
					const char name[] = "__VA_ARGS__";
					
					const char* begin = head;
					LangC_IgnoreUntilPairOfNestedOr(&head, '(', ')', 0);
					const char* end = head;
					
					uintsize len = (uintsize)(end - begin);
					
					uintsize def_len = sizeof(name) + len + 1;
					char* def_str = PushMemory(def_len);
					
					memcpy(def_str, name, sizeof(name)-1);
					def_str[sizeof(name)] = ' ';
					memcpy(def_str + sizeof(name), begin, len);
					def_str[def_len-1] = 0;
					
					String def = {
						.size = def_len,
						.data = def_str,
					};
					
					LangC_DefineMacro(ctx, def);
					break;
				}
				else
				{
					String ident = LangC_TokenizeIdent(&macro_head);
					
					const char* begin = head;
					LangC_IgnoreUntilPairOfNestedOr(&head, '(', ')', ',');
					
					const char* end = head;
					uintsize len = (uintsize)(end - begin);
					
					uintsize def_len = ident.size + 1 + len + 1;
					char* def_str = PushMemory(def_len);
					
					memcpy(def_str, ident.data, ident.size);
					def_str[ident.size] = ' ';
					memcpy(def_str + ident.size + 1, begin, len);
					def_str[def_len-1] = 0;
					
					String def = {
						.size = def_len,
						.data = def_str,
					};
					
					LangC_DefineMacro(ctx, def);
				}
				
				LangC_IgnoreWhitespaces(&macro_head, false);
				LangC_IgnoreWhitespaces(&head, false);
			}
			while (*macro_head == ',' && *head == ',' && (++macro_head, ++head));
			
			if (*macro_head == ',' && *head != ',')
			{
				LangC_LexerError(ctx, "expected more arguments in function-like macro.");
			}
		}
		
		if (*macro_head != ')')
		{
			LangC_LexerError(ctx, "expected ')' at the end of function-like macro arguments.");
		}
		
		assert(*macro_head == ')');
		++macro_head;
		
		assert(**phead == ')');
		++*phead;
		
		LangC_IgnoreWhitespaces(&macro_head, false);
	}
	
	if (!ctx->macroctx_first)
		ctx->macroctx_first = PushMemory(sizeof *ctx->macroctx_first);
	
	if (!ctx->macroctx_last)
	{
		ctx->macroctx_last = ctx->macroctx_first;
	}
	else
	{
		if (!ctx->macroctx_last->next)
			ctx->macroctx_last->next = PushMemory(sizeof *ctx->macroctx_last->next);
		
		ctx->macroctx_last = ctx->macroctx_last->next;
	}
	
	ctx->macroctx_last->saved_last_macro = saved_last_macro;
	ctx->macroctx_last->expanding_macro = macro;
	ctx->macroctx_last->head = macro_head;
}

internal void
LangC_ConsumeEofState(LangC_Lexer* ctx)
{
	for (;;)
	{
		if (ctx->macroctx_last != NULL)
		{
			LangC_LexerMacroContext* macroctx = ctx->macroctx_last;
			LangC_IgnoreWhitespaces(&macroctx->head, true);
			
			if (!*macroctx->head)
			{
				LangC_PopMacroContext(ctx);
			}
			else
			{
				break;
			}
		}
		else
		{
			LangC_LexerFile* lexfile = &ctx->file;
			if (!lexfile->active)
				break;
			
			while (lexfile->next_included && lexfile->next_included->active)
			{
				lexfile = lexfile->next_included;
			}
			
			LangC_IgnoreWhitespaces(&lexfile->head, true);
			
			if (!*lexfile->head)
			{
				lexfile->active = false;
			}
			else
			{
				break;
			}
		}
	}
}

internal int32
LangC_ValueOfEscaped(const char** ptr)
{
	int32 value = 0;
	int32 chars_in_hex = 0;
	
	switch (**ptr)
	{
		case '\\': value = '\\'; ++*ptr; break;
		case '\'': value = '\''; ++*ptr; break;
		case '"': value = '"'; ++*ptr; break;
		case 'a': value = 0x07; ++*ptr; break;
		case 'b': value = 0x08; ++*ptr; break;
		case 'e': value = 0x1B; ++*ptr; break;
		case 'f': value = 0x0C; ++*ptr; break;
		case 'n': value = 0x0A; ++*ptr; break;
		case 'r': value = 0x0D; ++*ptr; break;
		case 't': value = 0x09; ++*ptr; break;
		case 'v': value = 0x0B; ++*ptr; break;
		case '?': value = '?'; ++*ptr; break;
		
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
		{
			value = **ptr;
			++*ptr;
			
			int32 chars_in_octal = 2; // limit of 3 chars. one is already parsed
			
			while (chars_in_octal > 0 && **ptr >= '0' && **ptr < '8')
			{
				value *= 8;
				value += **ptr;
				++*ptr;
				--chars_in_octal;
			}
		} break;
		
		/**/ if (0) case 'x': chars_in_hex = 2;
		else if (0) case 'u': chars_in_hex = 4;
		else        case 'U': chars_in_hex = 8;
		
		{
			++*ptr;
			
			while (chars_in_hex > 0 &&
				   ((**ptr >= '0' && **ptr <= '9') ||
					(**ptr >= 'a' && **ptr <= 'f') ||
					(**ptr >= 'A' && **ptr <= 'F')))
			{
				value *= 16;
				
				if (**ptr >= 'a')
					value += 10 + (**ptr - 'a');
				else if (**ptr >= 'A')
					value += 10 + (**ptr - 'A');
				else
					value += **ptr - '0';
				
				++*ptr;
			}
		} break;
		
		default: value = **ptr; ++*ptr; break;
	}
	
	return value;
}

internal int32
LangC_TryToIncludeFile(LangC_LexerFile* lexfile, String file, const char* include_dir)
{
	char path[MAX_PATH_SIZE];
	int32 len;
	
	len = strlen(include_dir);
	memcpy(path, include_dir, len);
	memcpy(path + len, file.data, file.size);
	path[len + file.size] = 0;
	
	return LangC_InitLexerFile(lexfile, path);
}

internal void
LangC_IncludeFile(LangC_Lexer* ctx, String file, bool32 relative)
{
	LangC_LexerFile* lexfile = &ctx->file;
	
	while (lexfile->next_included && lexfile->next_included->active)
		lexfile = lexfile->next_included;
	
	lexfile->next_included = PushMemory(sizeof *lexfile->next_included);
	lexfile->next_included->previous_included = lexfile;
	
	if (relative)
	{
		char relative_path[MAX_PATH_SIZE];
		int32 path_len = strlen(lexfile->path);
		
		int32 last_slash_index = -1;
		for (int32 i = 0; i < path_len; ++i)
		{
			if (lexfile->path[i] == '/')
				last_slash_index = i;
		}
		
		if (last_slash_index != -1)
		{
			memcpy(relative_path, lexfile->path, last_slash_index+1);
			relative_path[last_slash_index+1] = 0;
			
			if (LangC_TryToIncludeFile(lexfile->next_included, file, relative_path) >= 0)
				return; // success
		}
	}
	
	lexfile = lexfile->next_included;
	
	for (int32 i = 0; i < LangC_global_include_dirs_count; ++i)
	{
		if (LangC_TryToIncludeFile(lexfile, file, LangC_global_include_dirs[i]) >= 0)
			return; // success
	}
	
	LangC_LexerError(ctx, "cannot locate/open included file '%.*s'.", file.size, file.data);
}

internal void
LangC_PreProcessorDefine(LangC_Lexer* ctx, LangC_LexerFile* lexfile)
{
	const char* begin = lexfile->head;
	LangC_IgnoreUntilEndOfLine(lexfile);
	const char* end = lexfile->head;
	
	String def = {
		.size = (uintsize)(end - begin),
		.data = begin
	};
	
	LangC_DefineMacro(ctx, def);
	
	lexfile->head = end;
}

internal void
LangC_PreProcessorUndef(LangC_Lexer* ctx, LangC_LexerFile* lexfile)
{
	String name = LangC_TokenizeIdent(&lexfile->head);
	
	LangC_UndefineMacro(ctx, name);
	LangC_IgnoreUntilEndOfLine(lexfile);
}

internal void
LangC_PreProcessorIfdef(LangC_Lexer* ctx, LangC_LexerFile* lexfile, bool32 not)
{
	String name = LangC_TokenizeIdent(&lexfile->head);
	
	bool32 result = not ^ (LangC_FindMacro(ctx, name, 2) != NULL);
	if (result)
	{
		++ctx->ifdef_nesting;
	}
	else
	{
		++ctx->ifdef_failed_nesting;
	}
	
	LangC_IgnoreUntilEndOfLine(lexfile);
}

internal void
LangC_PreProcessorIf(LangC_Lexer* ctx, LangC_LexerFile* lexfile)
{
	if (LangC_EvalPreProcessorExpr(ctx))
	{
		++ctx->ifdef_nesting;
	}
	else
	{
		++ctx->ifdef_failed_nesting;
	}
	
	ctx->newline_as_token = false;
}

internal void
LangC_PreProcessorInclude(LangC_Lexer* ctx, LangC_LexerFile* lexfile)
{
	const char* begin = lexfile->head;
	const char* end = begin;
	while (*end && (*end != '\n' || end[-1] == '\\')) ++end;
	
	lexfile->head = end;
	
	if (begin + 1 >= end)
	{
		LangC_LexerError(ctx, "missing file to include.");
		return;
	}
	
	bool32 relative;
	char pair;
	
	if (*begin == '<')
	{
		relative = false;
		pair = '>';
	}
	else if (*begin == '"')
	{
		relative = true;
		pair = '"';
	}
	else
	{
		LangC_LexerError(ctx, "invalid syntax for include directive.");
		return;
	}
	
	const char* path_begin = begin+1;
	const char* path_end = path_begin;
	
	while (path_end < end && *path_end != pair) ++path_end;
	
	if (*path_end != pair)
	{
		LangC_LexerError(ctx, "missing '%c' at the end of path.", pair);
		return;
	}
	
	String file = {
		.size = (uintsize)(path_end - path_begin),
		.data = path_begin,
	};
	
	LangC_IncludeFile(ctx, file, relative);
}

internal void
LangC_TokenizeSimpleTokens(LangC_Lexer* ctx, const char** phead)
{
	ctx->token.as_string.data = *phead;
	
	switch (**phead)
	{
		case '.':
		{
			if (!LangC_IsNumeric((*phead)[1], 10))
			{
				if ((*phead)[1] == '.' && (*phead)[2] == '.')
				{
					ctx->token.kind = LangC_TokenKind_VarArgs;
					*phead += 3;
				}
				else
				{
					ctx->token.kind = LangC_TokenKind_Dot;
					++(*phead);
				}
				
				break;
			}
		} /* fallthrough */
		
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		{
			int32 base = 10;
			if ((*phead)[0] == '0')
			{
				if ((*phead)[1] == 'x' || (*phead)[1] == 'X')
				{
					(*phead) += 2;
					base = 16;
				}
				else if ((*phead)[1] == 'b')
				{
					(*phead) += 2;
					base = 2;
				}
				else
				{
					(*phead) += 1;
					base = 8;
				}
			}
			
			const char* begin = (*phead);
			const char* end = begin;
			
			while (LangC_IsNumeric(*end, base)) ++end;
			
			if (*end == 'e' || *end == 'E')
				goto parse_exponent;
			
			if (*end == '.')
			{
				++end;
				
				while (LangC_IsNumeric(*end, base)) ++end;
				
				if (base == 2)
					LangC_LexerError(ctx, "error: floats cannot begin with '0b'.");
				
				if (*end == 'e' || *end == 'E' || (base == 16 && (*end == 'p' || *end == 'P')))
				{
					parse_exponent:;
					++end;
					
					if (*end == '+' || *end == '-')
						++end;
					
					while (LangC_IsNumeric(*end, 10)) ++end;
				}
				
				if (*end == 'f' || *end == 'F')
				{
					++end;
					ctx->token.kind = LangC_TokenKind_FloatLiteral;
					ctx->token.value_float = strtof(begin, NULL);
				}
				else
				{
					if (*end == 'l' || *end == 'L')
						++end;
					
					ctx->token.kind = LangC_TokenKind_DoubleLiteral;
					ctx->token.value_double = strtod(begin, NULL);
				}
			}
			else
			{
				bool32 is_unsig = false;
				ctx->token.kind = LangC_TokenKind_IntLiteral;
				
				if ((is_unsig = *end == 'u' || *end == 'U')) ++end, ctx->token.kind = LangC_TokenKind_UintLiteral;
				if (*end == 'l' || *end == 'L') ++end, ctx->token.kind += 1;
				if (*end == 'l' || *end == 'L') ++end, ctx->token.kind += 1;
				
				if (is_unsig)
					ctx->token.value_uint = strtoull(begin, NULL, base);
				else
					ctx->token.value_int = strtoll(begin, NULL, base);
			}
			
			int32 eaten = (int32)(end - begin);
			ctx->token.as_string.size += eaten;
			*phead += eaten;
		} break;
		
		case '"':
		{
			const char* begin = (*phead);
			const char* end = begin + 1;
			
			while (end[0] && end[0] != '"')
			{
				if (end[0] == '\n')
					break;
				
				if (end[0] == '\\' && (end[1] == '\n' || end[1] == '\\' || end[1] == '"'))
				{
					end += 2;
				}
				else
				{
					++end;
				}
			}
			
			if (*end != '"')
			{
				LangC_LexerError(ctx, "missing closing quote.");
				break;
			}
			
			int32 len = (int32)(end - begin);
			ctx->token.kind = LangC_TokenKind_StringLiteral;
			ctx->token.value_str = (String) {
				.size = (uintsize)(len - 1),
				.data = begin + 1,
			};
			
			(*phead) += len + 1;
		} break;
		
		case '\'':
		{
			int64 value = 0;
			int32 max_chars = 4;
			
			++(*phead);
			
			while (max_chars --> 0)
			{
				if ((*phead)[0] == '\'')
					break;
				
				value <<= 8;
				
				if ((*phead)[0] == '\\')
				{
					const char* end = ++(*phead);
					value |= LangC_ValueOfEscaped(&end);
					(*phead) = end;
				}
				else
				{
					value |= (*phead)[0];
					
					++(*phead);
				}
			}
			
			ctx->token.kind = LangC_TokenKind_IntLiteral;
			ctx->token.value_int = value;
			
			if ((*phead)[0] != '\'')
			{
				LangC_LexerError(ctx, "missing pair of character literal.");
				break;
			}
			
			++(*phead);
		} break;
		
		case '(': ctx->token.kind = LangC_TokenKind_LeftParen; ++(*phead); break;
		case ')': ctx->token.kind = LangC_TokenKind_RightParen; ++(*phead); break;
		case '[': ctx->token.kind = LangC_TokenKind_LeftBrkt; ++(*phead); break;
		case ']': ctx->token.kind = LangC_TokenKind_RightBrkt; ++(*phead); break;
		case '{': ctx->token.kind = LangC_TokenKind_LeftCurl; ++(*phead); break;
		case '}': ctx->token.kind = LangC_TokenKind_RightCurl; ++(*phead); break;
		case ',': ctx->token.kind = LangC_TokenKind_Comma; ++(*phead); break;
		case ':': ctx->token.kind = LangC_TokenKind_Colon; ++(*phead); break;
		case ';': ctx->token.kind = LangC_TokenKind_Semicolon; ++(*phead); break;
		case '?': ctx->token.kind = LangC_TokenKind_QuestionMark; ++(*phead); break;
		
		case '/':
		{
			if ((*phead)[1] == '=')
			{
				ctx->token.kind = LangC_TokenKind_DivAssign;
				(*phead) += 2;
			}
			else
			{
				ctx->token.kind = LangC_TokenKind_Div;
				++(*phead);
			}
		} break;
		
		case '-':
		{
			ctx->token.kind = LangC_TokenKind_Minus;
			++(*phead);
			
			if ((*phead)[0] == '>')
			{
				ctx->token.kind = LangC_TokenKind_Arrow;
				++(*phead);
			}
			else if ((*phead)[0] == '-')
			{
				ctx->token.kind = LangC_TokenKind_Dec;
				++(*phead);
			}
			else if ((*phead)[0] == '=')
			{
				ctx->token.kind = LangC_TokenKind_MinusAssign;
				++(*phead);
			}
		} break;
		
		case '+':
		{
			ctx->token.kind = LangC_TokenKind_Plus;
			++(*phead);
			
			if ((*phead)[0] == '+')
			{
				ctx->token.kind = LangC_TokenKind_Inc;
				++(*phead);
			}
			else if ((*phead)[0] == '=')
			{
				ctx->token.kind = LangC_TokenKind_PlusAssign;
				++(*phead);
			}
		} break;
		
		case '*':
		{
			ctx->token.kind = LangC_TokenKind_Mul;
			++(*phead);
			
			if ((*phead)[0] == '=')
			{
				ctx->token.kind = LangC_TokenKind_MulAssign;
				++(*phead);
			}
		} break;
		
		case '%':
		{
			ctx->token.kind = LangC_TokenKind_Mod;
			++(*phead);
			
			if ((*phead)[0] == '=')
			{
				ctx->token.kind = LangC_TokenKind_ModAssign;
				++(*phead);
			}
		} break;
		
		case '<':
		{
			ctx->token.kind = LangC_TokenKind_LThan;
			++(*phead);
			
			if ((*phead)[0] == '<')
			{
				ctx->token.kind = LangC_TokenKind_LeftShift;
				++(*phead);
				
				if ((*phead)[0] == '=')
				{
					ctx->token.kind = LangC_TokenKind_LeftShiftAssign;
					++(*phead);
				}
			}
			else if ((*phead)[0] == '=')
			{
				ctx->token.kind = LangC_TokenKind_LEqual;
				++(*phead);
			}
		} break;
		
		case '>':
		{
			ctx->token.kind = LangC_TokenKind_GThan;
			++(*phead);
			
			if ((*phead)[0] == '>')
			{
				ctx->token.kind = LangC_TokenKind_RightShift;
				++(*phead);
				
				if ((*phead)[0] == '=')
				{
					ctx->token.kind = LangC_TokenKind_RightShiftAssign;
					++(*phead);
				}
			}
			else if ((*phead)[0] == '=')
			{
				ctx->token.kind = LangC_TokenKind_GEqual;
				++(*phead);
			}
		} break;
		
		case '=':
		{
			ctx->token.kind = LangC_TokenKind_Assign;
			++(*phead);
			
			if ((*phead)[0] == '=')
			{
				ctx->token.kind = LangC_TokenKind_Equals;
				++(*phead);
			}
		} break;
		
		case '!':
		{
			ctx->token.kind = LangC_TokenKind_LNot;
			++(*phead);
			
			if ((*phead)[0] == '=')
			{
				ctx->token.kind = LangC_TokenKind_NotEquals;
				++(*phead);
			}
		} break;
		
		case '&':
		{
			ctx->token.kind = LangC_TokenKind_And;
			++(*phead);
			
			if ((*phead)[0] == '&')
			{
				ctx->token.kind = LangC_TokenKind_LAnd;
				++(*phead);
			}
			else if ((*phead)[0] == '=')
			{
				ctx->token.kind = LangC_TokenKind_AndAssign;
				++(*phead);
			}
		} break;
		
		case '|':
		{
			ctx->token.kind = LangC_TokenKind_Or;
			++(*phead);
			
			if ((*phead)[0] == '|')
			{
				ctx->token.kind = LangC_TokenKind_LOr;
				++(*phead);
			}
			else if ((*phead)[0] == '=')
			{
				ctx->token.kind = LangC_TokenKind_OrAssign;
				++(*phead);
			}
		} break;
		
		case '^':
		{
			ctx->token.kind = LangC_TokenKind_Xor;
			++(*phead);
			
			if ((*phead)[0] == '=')
			{
				ctx->token.kind = LangC_TokenKind_XorAssign;
				++(*phead);
			}
		} break;
		
		default:
		{
			LangC_LexerError(ctx, "unexpected token '%c'.", (*phead)[0]);
			++(*phead);
		} break;
	}
	
	ctx->token.as_string.size = (uintsize)(*phead - ctx->token.as_string.data);
}

internal void
LangC_NextToken(LangC_Lexer* ctx)
{
	String ident;
	LangC_LexerFile* lexfile;
	
	beginning:;
	
	if (ctx->macroctx_last != NULL)
	{
		//~ Macro State
		LangC_LexerMacroContext* macroctx = ctx->macroctx_last;
		LangC_IgnoreWhitespaces(&macroctx->head, !ctx->newline_as_token);
		
		switch (*macroctx->head)
		{
			case 0: LangC_PopMacroContext(ctx); goto beginning;
			
			case '#': // Stringify
			{
				++macroctx->head;
				
				LangC_IgnoreWhitespaces(&macroctx->head, false);
				ident = LangC_TokenizeIdent(&macroctx->head);
				
				if (!LangC_IsIdentMacroParameter(ident, macroctx->expanding_macro))
				{
					LangC_LexerError(ctx, "'%.*s' is not a parameter.", StrFmt(ident));
					
					// fallback
					ctx->token.kind = LangC_TokenKind_Identifier;
					ctx->token.value_ident = ident;
					break;
				}
				
				LangC_Macro* macro = LangC_FindMacro(ctx, ident, false);
				assert(macro);
				
				const char* head = macro->def;
				LangC_TokenizeIdent(&head);
				LangC_IgnoreWhitespaces(&head, false);
				
				String value = {
					.size = strlen(head),
					.data = head,
				};
				
				ctx->token.kind = LangC_TokenKind_StringLiteral;
				ctx->token.value_str = value;
				ctx->token.as_string = value;
			} break;
			
			case 'L':
			{
				if (macroctx->head[1] == '"')
				{
					++macroctx->head;
					LangC_TokenizeSimpleTokens(ctx, &macroctx->head);
					ctx->token.kind = LangC_TokenKind_WideStringLiteral;
					++ctx->token.as_string.size;
					--ctx->token.as_string.data;
					break;
				}
				//else
				
			} /* fallthrough */
			
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
			case 'K': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
			case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': case 'a': case 'b': case 'c': case 'd':
			case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
			case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x':
			case 'y': case 'z': case '_':
			{
				ident = LangC_TokenizeIdent(&macroctx->head);
				
				LangC_ConsumeEofState(ctx);
				macroctx = ctx->macroctx_last;
				if (!macroctx)
				{
					lexfile = LangC_CurrentLexerFile(ctx);
					goto ident_lexing_outside_macroctx;
				}
				
				before_macro_expansion:;
				LangC_Macro* macro = LangC_FindMacro(ctx, ident, *macroctx->head == '(' ? 3 : 0);
				
				if (!(macro && LangC_IsIdentMacroParameter(ident, macroctx->expanding_macro)) &&
					macroctx->head[0] == '#' && macroctx->head[1] == '#')
				{
					macroctx->head += 2;
					LangC_IgnoreWhitespaces(&macroctx->head, true);
					
					if (!macroctx->head[0])
					{
						LangC_LexerError(ctx, "'##' should never appear at the end of a macro.");
					}
					else
					{
						String next_ident = LangC_TokenizeIdent(&macroctx->head);
						
						if (LangC_IsIdentMacroParameter(next_ident, macroctx->expanding_macro))
						{
							LangC_NextToken(ctx);
							LangC_AssertToken(ctx, LangC_TokenKind_Identifier);
							next_ident = ctx->token.value_ident;
						}
						
						ident = LangC_ConcatStrings(ident, next_ident);
						
						goto before_macro_expansion;
					}
				}
				
				if (macro)
				{
					LangC_PushMacroContext(ctx, &macroctx->head, macro);
					goto beginning;
				}
				
				ctx->token.kind = LangC_TokenKind_Identifier;
				ctx->token.value_ident = ident;
				
				for (int32 keyword = LangC_TokenKind__FirstKeyword; keyword <= LangC_TokenKind__LastKeyword; ++keyword)
				{
					if (MatchCString(LangC_token_str_table[keyword], ident.data, ident.size))
					{
						ctx->token.kind = keyword;
						break;
					}
				}
				
				ctx->token.as_string = ident;
			} break;
			
			default: LangC_TokenizeSimpleTokens(ctx, &macroctx->head); break;
		}
	}
	else
	{
		lexfile = LangC_CurrentLexerFile(ctx);
		if (!lexfile)
		{
			ctx->token.kind = LangC_TokenKind_Eof;
			return;
		}
		
		LangC_IgnoreWhitespaces(&lexfile->head, !ctx->newline_as_token || ctx->ifdef_failed_nesting > 0);
		LangC_UpdateFileLineAndCol(lexfile);
		
		ctx->token.line = lexfile->line;
		ctx->token.col = lexfile->col;
		
		if (ctx->ifdef_failed_nesting > 0)
		{
			//~ "Don't Compile" State
			switch (*lexfile->head)
			{
				case 0: lexfile->active = false; goto beginning;
				
				case '#':
				{
					++lexfile->head;
					LangC_IgnoreWhitespaces(&lexfile->head, false);
					
					const char* begin = lexfile->head;
					
					while (LangC_IsAlpha(*lexfile->head)) ++lexfile->head;
					int32 dirlen = (int32)(lexfile->head - begin);
					
					LangC_IgnoreWhitespaces(&lexfile->head, false);
					
					bool32 not = false;
					if (MatchCString("if", begin, dirlen) ||
						MatchCString("ifdef", begin, dirlen) ||
						MatchCString("ifndef", begin, dirlen))
					{
						++ctx->ifdef_failed_nesting;
					}
					else if (MatchCString("else", begin, dirlen))
					{
						if (ctx->ifdef_failed_nesting == 1 && !ctx->ifdef_already_matched)
						{
							ctx->ifdef_failed_nesting = 0;
							++ctx->ifdef_nesting;
						}
					}
					else if (MatchCString("elif", begin, dirlen))
					{
						if (ctx->ifdef_failed_nesting == 1 && !ctx->ifdef_already_matched)
						{
							ctx->ifdef_failed_nesting = 0;
							
							LangC_PreProcessorIf(ctx, lexfile);
							goto beginning;
						}
					}
					else if (MatchCString("elifdef", begin, dirlen) ||
							 (MatchCString("elifndef", begin, dirlen) && (not = true)))
					{
						if (ctx->ifdef_failed_nesting == 1 && !ctx->ifdef_already_matched)
						{
							ctx->ifdef_failed_nesting = 0;
							
							LangC_PreProcessorIfdef(ctx, lexfile, not);
							goto beginning;
						}
					}
					else if (MatchCString("endif", begin, dirlen))
					{
						if (--ctx->ifdef_failed_nesting == 0)
						{
							ctx->ifdef_already_matched = false;
						}
					}
				} /* fallthrough */
				
				default:
				{
					LangC_IgnoreUntilEndOfLine(lexfile);
				} goto beginning;
			}
		}
		else
		{
			//~ Normal State
			switch (*lexfile->head)
			{
				case 0: lexfile->active = false; ctx->newline_as_token = false; goto beginning;
				case '\n':
				{
					ctx->token.as_string = Str("(EOL)");
					ctx->token.kind = LangC_TokenKind_NewLine;
					ctx->newline_as_token = false;
					++lexfile->head;
				} break;
				
				case '#':
				{
					++lexfile->head;
					LangC_IgnoreWhitespaces(&lexfile->head, false);
					
					const char* begin = lexfile->head;
					
					while (LangC_IsAlpha(*lexfile->head)) ++lexfile->head;
					int32 dirlen = (int32)(lexfile->head - begin);
					
					LangC_IgnoreWhitespaces(&lexfile->head, false);
					
					if (MatchCString("if", begin, dirlen))
					{
						LangC_PreProcessorIf(ctx, lexfile);
					}
					else if (MatchCString("ifdef", begin, dirlen))
					{
						LangC_PreProcessorIfdef(ctx, lexfile, false);
					}
					else if (MatchCString("ifndef", begin, dirlen))
					{
						LangC_PreProcessorIfdef(ctx, lexfile, true);
					}
					else if (MatchCString("else", begin, dirlen) ||
							 MatchCString("elif", begin, dirlen) ||
							 MatchCString("elifdef", begin, dirlen) ||
							 MatchCString("elifndef", begin, dirlen))
					{
						ctx->ifdef_already_matched = true;
						++ctx->ifdef_failed_nesting;
						--ctx->ifdef_nesting;
						
						LangC_IgnoreUntilEndOfLine(lexfile);
					}
					else if (MatchCString("endif", begin, dirlen))
					{
						if (--ctx->ifdef_nesting < 0)
						{
							LangC_LexerError(ctx, "unmatched '#endif'.");
						}
						
						LangC_IgnoreUntilEndOfLine(lexfile);
					}
					else if (MatchCString("define", begin, dirlen))
					{
						LangC_PreProcessorDefine(ctx, lexfile);
					}
					else if (MatchCString("undef", begin, dirlen))
					{
						LangC_PreProcessorUndef(ctx, lexfile);
					}
					else if (MatchCString("error", begin, dirlen))
					{
						begin = lexfile->head;
						LangC_IgnoreUntilEndOfLine(lexfile);
						
						int32 len = (int32)(lexfile->head - begin);
						
						LangC_LexerError(ctx, "%.*s", len, begin);
					}
					else if (MatchCString("include", begin, dirlen))
					{
						LangC_PreProcessorInclude(ctx, lexfile);
					}
					else if (MatchCString("pragma", begin, dirlen))
					{
						// TODO
						LangC_IgnoreUntilEndOfLine(lexfile);
					}
					else
					{
						LangC_LexerError(ctx, "unknown pre-processor directive '%.*s'.", dirlen, begin);
						LangC_IgnoreUntilEndOfLine(lexfile);
					}
				} goto beginning;
				
				case 'L':
				{
					if (lexfile->head[1] == '"')
					{
						++lexfile->head;
						LangC_TokenizeSimpleTokens(ctx, &lexfile->head);
						ctx->token.kind = LangC_TokenKind_WideStringLiteral;
						
						++ctx->token.as_string.size;
						--ctx->token.as_string.data;
					}
					//else
					
				} /* fallthrough */
				
				case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I':
				case 'J': case 'K': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
				case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': case 'a':
				case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
				case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's':
				case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': case '_':
				{
					ident = LangC_TokenizeIdent(&lexfile->head);
					LangC_IgnoreWhitespaces(&lexfile->head, true);
					
					if (!ctx->dont_expand_macros)
					{
						ident_lexing_outside_macroctx:;
						LangC_Macro* macro = LangC_FindMacro(ctx, ident,  *lexfile->head == '(' ? 3 : 0);
						
						if (macro)
						{
							LangC_UpdateFileLineAndCol(lexfile);
							LangC_PushMacroContext(ctx, &lexfile->head, macro);
							goto beginning;
						}
					}
					
					ctx->token.kind = LangC_TokenKind_Identifier;
					ctx->token.value_ident = ident;
					
					for (int32 keyword = LangC_TokenKind__FirstKeyword; keyword <= LangC_TokenKind__LastKeyword; ++keyword)
					{
						if (MatchCString(LangC_token_str_table[keyword], ident.data, ident.size))
						{
							ctx->token.kind = keyword;
							break;
						}
					}
					
					ctx->token.as_string = ident;
				} break;
				
				default: LangC_TokenizeSimpleTokens(ctx, &lexfile->head); break;
			}
		}
	}
}

internal bool32
LangC_EatToken(LangC_Lexer* ctx, LangC_TokenKind kind)
{
	bool32 result = true;
	
	if (ctx->token.kind != kind)
	{
		String got = ctx->token.as_string;
		LangC_LexerError(ctx, "expected '%s', but got '%.*s'.", LangC_token_str_table[kind], StrFmt(got));
		result = false;
	}
	
	LangC_NextToken(ctx);
	return result;
}

internal bool32
LangC_TryToEatToken(LangC_Lexer* ctx, LangC_TokenKind kind)
{
	if (ctx->token.kind == kind)
	{
		LangC_NextToken(ctx);
		return true;
	}
	
	return false;
}

internal bool32
LangC_AssertToken(LangC_Lexer* ctx, LangC_TokenKind kind)
{
	if (ctx->token.kind != kind)
	{
		String got = ctx->token.as_string;
		LangC_LexerError(ctx, "expected '%s', but got '%.*s'.", LangC_token_str_table[kind], StrFmt(got));
		
		return false;
	}
	
	return true;
}

//~ NOTE(ljre): this is awful
internal int32 LangC_EvalPreProcessorExpr_Binary(LangC_Lexer* ctx, int32 level);

internal int32
LangC_EvalPreProcessorExpr_Factor(LangC_Lexer* ctx)
{
	beginning:;
	
	switch (ctx->token.kind)
	{
		case LangC_TokenKind_Minus:
		{
			LangC_NextToken(ctx);
			return -LangC_EvalPreProcessorExpr_Factor(ctx);
		} break;
		
		case LangC_TokenKind_Plus:
		{
			LangC_NextToken(ctx);
			goto beginning;
		} break;
		
		case LangC_TokenKind_Not:
		{
			LangC_NextToken(ctx);
			return ~LangC_EvalPreProcessorExpr_Factor(ctx);
		} break;
		
		case LangC_TokenKind_LNot:
		{
			LangC_NextToken(ctx);
			return !LangC_EvalPreProcessorExpr_Factor(ctx);
		} break;
	}
	
	int32 result = 0;
	
	switch (ctx->token.kind)
	{
		case LangC_TokenKind_IntLiteral:
		case LangC_TokenKind_LIntLiteral:
		case LangC_TokenKind_LLIntLiteral:
		{
			result = (int32)ctx->token.value_int;
			LangC_NextToken(ctx);
		} break;
		
		case LangC_TokenKind_UintLiteral:
		case LangC_TokenKind_LUintLiteral:
		case LangC_TokenKind_LLUintLiteral:
		{
			result = (int32)ctx->token.value_uint;
			LangC_NextToken(ctx);
		} break;
		
		case LangC_TokenKind_Identifier:
		{
			String name = ctx->token.value_ident;
			
			if (MatchCString("defined", name.data, name.size))
			{
				ctx->dont_expand_macros = true;
				
				LangC_NextToken(ctx);
				if (LangC_TryToEatToken(ctx, LangC_TokenKind_LeftParen))
				{
					if (!LangC_AssertToken(ctx, LangC_TokenKind_Identifier))
						break;
					name = ctx->token.value_ident;
					
					LangC_NextToken(ctx);
					LangC_EatToken(ctx, LangC_TokenKind_RightParen);
				}
				else
				{
					if (!LangC_AssertToken(ctx, LangC_TokenKind_Identifier))
						break;
					name = ctx->token.value_ident;
					
					LangC_NextToken(ctx);
				}
				
				result = (LangC_FindMacro(ctx, name, 2) != NULL);
			}
			else
			{
				result = 0;
				LangC_NextToken(ctx);
				
				if (ctx->token.kind == LangC_TokenKind_LeftParen)
				{
					LangC_LexerError(ctx, "unknown macro function '%.*s'.", StrFmt(name));
					ctx->dont_expand_macros = true;
					LangC_NextToken(ctx);
					
					int32 nested = 1;
					while (ctx->token.kind && ctx->token.kind != LangC_TokenKind_NewLine)
					{
						if (ctx->token.kind == LangC_TokenKind_LeftParen)
							++nested;
						else if (ctx->token.kind == LangC_TokenKind_RightParen && --nested <= 0)
							break;
						
						LangC_NextToken(ctx);
					}
					
					if (LangC_AssertToken(ctx, LangC_TokenKind_RightParen))
					{
						LangC_NextToken(ctx);
					}
				}
			}
		} break;
		
		case LangC_TokenKind_LeftParen:
		{
			LangC_NextToken(ctx);
			result = LangC_EvalPreProcessorExpr_Binary(ctx, 0);
			LangC_EatToken(ctx, LangC_TokenKind_RightParen);
		} break;
	}
	
	ctx->dont_expand_macros = false;
	
	return result;
}

internal int32
LangC_EvalPreProcessorExpr_Op(LangC_TokenKind op, int32 left, int32 right)
{
	switch (op)
	{
		case LangC_TokenKind_Comma: return right;
		case LangC_TokenKind_Assign: return right;
		case LangC_TokenKind_PlusAssign:
		case LangC_TokenKind_Plus: return left + right;
		case LangC_TokenKind_MinusAssign:
		case LangC_TokenKind_Minus: return left - right;
		case LangC_TokenKind_MulAssign:
		case LangC_TokenKind_Mul: return left * right;
		case LangC_TokenKind_DivAssign:
		case LangC_TokenKind_Div:
		{
			if (right == 0)
				return 0;
			return left / right;
		};
		case LangC_TokenKind_ModAssign:
		case LangC_TokenKind_Mod:
		{
			if (right == 0)
				return 0;
			return left % right;
		};
		case LangC_TokenKind_LeftShiftAssign:
		case LangC_TokenKind_LeftShift: return left << right;
		case LangC_TokenKind_RightShiftAssign:
		case LangC_TokenKind_RightShift: return left >> right;
		case LangC_TokenKind_AndAssign:
		case LangC_TokenKind_And: return left & right;
		case LangC_TokenKind_OrAssign:
		case LangC_TokenKind_Or: return left | right;
		case LangC_TokenKind_XorAssign:
		case LangC_TokenKind_Xor: return left ^ right;
		
		case LangC_TokenKind_LOr: return left || right;
		case LangC_TokenKind_LAnd: return left && right;
		case LangC_TokenKind_Equals: return left == right;
		case LangC_TokenKind_NotEquals: return left != right;
		case LangC_TokenKind_LThan: return left < right;
		case LangC_TokenKind_LEqual: return left <= right;
		case LangC_TokenKind_GThan: return left > right;
		case LangC_TokenKind_GEqual: return left >= right;
	}
	
	return 0;
}

internal int32
LangC_EvalPreProcessorExpr_Binary(LangC_Lexer* ctx, int32 level)
{
	int32 result = LangC_EvalPreProcessorExpr_Factor(ctx);
	
	LangC_OperatorPrecedence prec;
	while (prec = LangC_operators_precedence[ctx->token.kind],
		   prec.level > level)
	{
		LangC_TokenKind op = ctx->token.kind;
		LangC_NextToken(ctx);
		int32 right = LangC_EvalPreProcessorExpr_Factor(ctx);
		
		LangC_TokenKind lookahead = ctx->token.kind;
		LangC_OperatorPrecedence lookahead_prec = LangC_operators_precedence[lookahead];
		
		while (lookahead_prec.level > 0 &&
			   (lookahead_prec.level > prec.level ||
				(lookahead_prec.level == prec.level && lookahead_prec.right2left))) {
			LangC_NextToken(ctx);
			
			if (lookahead == LangC_TokenKind_QuestionMark) {
				int32 if_true = LangC_EvalPreProcessorExpr_Binary(ctx, 1);
				
				LangC_EatToken(ctx, LangC_TokenKind_Colon);
				int32 if_false = LangC_EvalPreProcessorExpr_Binary(ctx, level + 1);
				
				right = right ? if_true : if_false;
			} else {
				int32 other = LangC_EvalPreProcessorExpr_Binary(ctx, level + 1);
				
				right = LangC_EvalPreProcessorExpr_Op(lookahead, right, other);
			}
			
			lookahead = ctx->token.kind;
			lookahead_prec = LangC_operators_precedence[lookahead];
		}
		
		result = LangC_EvalPreProcessorExpr_Op(op, result, right);
	}
	
	return result;
}

internal int32
LangC_EvalPreProcessorExpr(LangC_Lexer* ctx)
{
	ctx->newline_as_token = true;
	
	LangC_NextToken(ctx);
	int32 result = LangC_EvalPreProcessorExpr_Binary(ctx, 0);
	
	return result;
}
