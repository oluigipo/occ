struct LangC_Macro typedef LangC_Macro;
struct LangC_Macro
{
	LangC_Macro* next;
	LangC_Macro* previous;
	
	const char* def;
	String name;
	bool16 is_func_like;
	bool16 expanding;
};

struct LangC_PPLoadedFile typedef LangC_PPLoadedFile;
struct LangC_PPLoadedFile
{
	LangC_PPLoadedFile* next;
	uint64 hash;
	const char* contents;
	bool32 relative;
};

struct LangC_Preprocessor
{
	char* buf;
	
	LangC_Macro* first_macro;
	LangC_Macro* last_macro;
	
	LangC_PPLoadedFile* loaded_files;
}
typedef LangC_Preprocessor;

internal void LangC_Preprocess2(LangC_Preprocessor* pp, String path, const char* source, LangC_LexerFile* from);

internal LangC_PPLoadedFile*
LangC_LoadFileFromDisk(const char path[MAX_PATH_SIZE], uint64 calculated_hash, bool32 relative)
{
	uintsize path_len = strlen(path);
	const char* contents = NULL;
	
	if (relative)
	{
		contents = OS_ReadWholeFile(path);
		if (contents)
			goto success;
	}
	
	relative = false;
	for (int32 i = 0; i < LangC_include_dirs_count; ++i)
	{
		char fullpath[MAX_PATH_SIZE];
		
		const char* dir = LangC_include_dirs[i];
		uintsize len = strlen(dir);
		
		memcpy(fullpath, dir, len);
		memcpy(fullpath + len, path, path_len);
		fullpath[len + path_len] = 0;
		
		contents = OS_ReadWholeFile(path);
		if (contents)
			goto success;
	}
	
	return NULL;
	
	success:
	{
		LangC_PPLoadedFile* result = PushMemory(sizeof *result);
		
		result->hash = calculated_hash;
		result->contents = contents;
		result->relative = relative;
		
		return result;
	}
}

internal const char*
LangC_TryToLoadFile(LangC_Preprocessor* pp, String path, bool32 relative)
{
	// TODO(ljre): Resolve full file paths before hashing!
	
	uint64 search_hash = SimpleHash(path);
	
	// NOTE(ljre): Search for already loaded files.
	if (pp->loaded_files)
	{
		LangC_PPLoadedFile* file = pp->loaded_files;
		
		if (file->hash == search_hash && file->relative > relative)
			return file->contents;
		
		while (file->next)
		{
			if (file->next->hash == search_hash && file->relative > relative)
			{
				return file->next->contents;
			}
			
			file = file->next;
		}
		
		file->next = LangC_LoadFileFromDisk(NullTerminateString(path), search_hash, relative);
		
		if (!file->next)
			return NULL;
		
		return file->next->contents;
	}
	else
	{
		pp->loaded_files = LangC_LoadFileFromDisk(NullTerminateString(path), search_hash, relative);
		
		if (!pp->loaded_files)
			return NULL;
		
		return pp->loaded_files->contents;
	}
}

internal void
LangC_IgnoreUntilNewline(LangC_Lexer* lex)
{
	while (lex->token.kind && lex->token.kind != LangC_TokenKind_NewLine)
		LangC_NextToken(lex);
}

internal void
LangC_DefineMacro(LangC_Preprocessor* pp, String definition)
{
	const char* def = NullTerminateString(definition);
	
	const char* ident_begin = def;
	const char* head = def;
	
	while (LangC_IsIdentChar(head[0]))
		++head;
	
	String name = {
		.data = ident_begin,
		.size = (uintsize)(head - ident_begin),
	};
	
	bool32 is_func_like = (head[0] == '(');
	
	if (!pp->last_macro)
	{
		if (!pp->first_macro)
			pp->first_macro = PushMemory(sizeof *pp->first_macro);
		
		pp->last_macro = pp->first_macro;
	}
	else
	{
		if (!pp->last_macro->next)
			pp->last_macro->next = PushMemory(sizeof *pp->last_macro->next);
		
		pp->last_macro->next->previous = pp->last_macro;
		pp->last_macro = pp->last_macro->next;
	}
	
	pp->last_macro->def = def;
	pp->last_macro->name = name;
	pp->last_macro->is_func_like = is_func_like;
}

internal void
LangC_UndefineMacro(LangC_Preprocessor* pp, String name)
{
	if (!pp->first_macro)
		return;
	
	if (CompareString(pp->first_macro->name, name) == 0)
	{
		pp->first_macro = pp->first_macro->next;
		return;
	}
	
	LangC_Macro* current = pp->first_macro;
	while (current->next && current->next != pp->last_macro)
	{
		if (CompareString(current->name, name) == 0)
			current->next = current->next->next;
		
		current = current->next;
	}
}

// NOTE(ljre): first of all, this function will never return a macro being expanded.
//             returns NULL if it couldn't find it.
//
//             if type is 0, then it will return an object-like macro.
//             if type is 1, then it will return a function-like macro.
//             if type is 2, then it will return any macro.
//             if type is 3, then it will return a function-like macro with this name, otherwise
//                                a object-like macro is still ok.
internal LangC_Macro*
LangC_FindMacro(LangC_Preprocessor* pp, String name, int32 type)
{
	if (type == 3)
	{
		LangC_Macro* result = LangC_FindMacro(pp, name, 1);
		
		if (!result)
			result = LangC_FindMacro(pp, name, 0);
		
		return result;
	}
	
	LangC_Macro* current = pp->last_macro;
	if (current)
	{
		do
		{
			if (!current->expanding &&
				(type == 2 || current->is_func_like == type) &&
				CompareString(current->name, name) == 0)
			{
				return current;
			}
		}
		while (current != pp->first_macro && (current = current->previous));
	}
	
	return NULL;
}

internal LangC_Macro*
LangC_FindMacroParameter(LangC_Macro* first_param, String name)
{
	LangC_Macro* it = first_param;
	
	while (it)
	{
		if (CompareString(it->name, name) == 0)
			break;
		
		it = it->next;
	}
	
	return it;
}

internal void
LangC_TracePreprocessor(LangC_Preprocessor* pp, LangC_Lexer* lex, uint32 flags)
{
	char str[2048];
	int32 len;
	assert(flags < 16);
	
	static const char* flag_table[] = {
		"1",
		"2",
		"1 2",
		"3",
		"1 3",
		"2 3",
		"1 2 3",
		"4",
		"1 4",
		"2 4",
		"1 2 4",
		"3 4",
		"1 3 4",
		"2 3 4",
		"1 2 3 4",
	};
	
	if (flags)
		len = snprintf(str, sizeof str, "# %i \"%.*s\" %s\n", lex->line, StrFmt(lex->file->path), flag_table[flags]);
	else
		len = snprintf(str, sizeof str, "# %i \"%.*s\"\n", lex->line, StrFmt(lex->file->path));
	
	SB_PushArray(pp->buf, len, str);
}

internal void
LangC_ExpandMacro(LangC_Preprocessor* pp, LangC_Macro* macro, LangC_Lexer* parent_lex)
{
	macro->expanding = true;
	LangC_Macro* saved_last_macro = pp->last_macro;
	
	// NOTE(ljre): Define parameters as macros
	if (macro->is_func_like)
	{
		LangC_NextToken(parent_lex); // eat macro name
		LangC_EatToken(parent_lex, LangC_TokenKind_LeftParen);
		
		const char* def_head = macro->def;
		while (*def_head && *def_head++ != '(');
		
		if (*def_head != ')')
		{
			do
			{
				LangC_IgnoreWhitespaces(&def_head, false);
				
				// NOTE(ljre): __VA_ARGS__
				if (def_head[0] == '.' && def_head[1] == '.' && def_head[2] == '.')
				{
					def_head += 3;
					
					// TODO
				}
			}
			while (def_head[0] == ',');
		}
		
		// TODO
		
		LangC_AssertToken(parent_lex, LangC_TokenKind_RightParen);
	}
	
	// NOTE(ljre): Expand macro
	LangC_Lexer* lex = &(LangC_Lexer) {
		.preprocessor = true,
		.file = parent_lex->file,
	};
	
	LangC_SetupLexer(lex, macro->def);
	LangC_NextToken(lex);
	
	while (lex->token.kind != LangC_TokenKind_Eof)
	{
		switch (lex->token.kind)
		{
			case LangC_TokenKind_Hashtag:
			{
				LangC_NextToken(lex);
				
				LangC_Macro* param;
				if (!macro->is_func_like ||
					!LangC_AssertToken(lex, LangC_TokenKind_Identifier) ||
					!(param = LangC_FindMacroParameter(saved_last_macro->next, lex->token.value_ident)))
				{
					LangC_LexerError(parent_lex, "expected a macro parameter for stringification.");
				}
				else
				{
					const char* def = param->def;
					while (LangC_IsIdentChar(*def))
						++def;
					
					LangC_IgnoreWhitespaces(&def, false);
					
					char* buf = NULL;
					uintsize len = strlen(def);
					SB_ReserveAtLeast(buf, len+2);
					
					SB_Push(buf, '"');
					SB_PushArray(buf, len, def);
					SB_Push(buf, '"');
					
					LangC_Token tok = {
						.kind = LangC_TokenKind_StringLiteral,
						.as_string = {
							.size = SB_Len(buf),
							.data = buf,
						},
						.value_str = {
							.size = len,
							.data = buf + 1,
						},
					};
					
					LangC_PushToken(parent_lex, &tok);
				}
			} break;
			
			case LangC_TokenKind_Identifier:
			{
				LangC_Macro* param;
				if (lex->token.kind == LangC_TokenKind_Identifier &&
					(param = LangC_FindMacroParameter(saved_last_macro->next, lex->token.value_ident)))
				{
					LangC_ExpandMacro(pp, param, lex);
					break;
				}
			} /* fallthrough */
			
			default:
			{
				LangC_Token tok = lex->token;
				LangC_NextToken(lex);
				
				if (lex->token.kind == LangC_TokenKind_DoubleHashtag)
				{
					LangC_NextToken(lex);
					LangC_Macro* param;
					
					if (lex->token.kind == LangC_TokenKind_Identifier &&
						(param = LangC_FindMacroParameter(saved_last_macro->next, lex->token.value_ident)))
					{
						LangC_ExpandMacro(pp, param, lex);
					}
					
					LangC_Token other_tok = lex->token;
					
					uintsize len = tok.as_string.size + other_tok.as_string.size;
					char* buf = PushMemory(len);
					memcpy(buf, tok.as_string.data, tok.as_string.size);
					memcpy(buf + tok.as_string.size, other_tok.as_string.data, other_tok.as_string.size);
					
					String final_tok_str = {
						.data = buf,
						.size = len,
					};
					
					LangC_Token final_token = {
						.kind = LangC_TokenKind_Identifier, // NOTE(ljre): This shouldn't matter!
						.as_string = final_tok_str,
						.value_ident = final_tok_str,
					};
					
					LangC_PushToken(parent_lex, &final_token);
				}
				else
				{
					LangC_PushToken(parent_lex, &tok);
				}
			} break;
		}
	}
	
	// NOTE(ljre): At this point, the parent_lex is going to have the macro name *or*
	//             the ending ')' of the parameter list. We can eat a token safely.
	LangC_NextToken(parent_lex);
	
	macro->expanding = false;
	pp->last_macro = saved_last_macro;
}

internal void
LangC_PreprocessIfDef(LangC_Preprocessor* pp, LangC_Lexer* lex)
{
	// TODO(ljre):
}

internal void
LangC_PreprocessIf(LangC_Preprocessor* pp, LangC_Lexer* lex)
{
	// TODO(ljre): dear lord
}

internal void
LangC_PreprocessInclude(LangC_Preprocessor* pp, LangC_Lexer* lex)
{
	String path;
	bool32 relative;
	
	if (lex->token.kind == LangC_TokenKind_StringLiteral)
	{
		relative = true;
		path = lex->token.value_str;
		
		LangC_IgnoreUntilNewline(lex);
	}
	else if (lex->token.kind == LangC_TokenKind_LThan)
	{
		relative = false;
		const char* head = lex->head; // NOTE(ljre): 'lex->head' should be at the beginning of the next token.
		
		while (*head && *head != '>' && *head != '\n')
			++head;
		
		if (*head != '>')
		{
			LangC_LexerError(lex, "missing '>' at the end of file path.");
			LangC_IgnoreUntilNewline(lex);
			return;
		}
		
		path = (String) {
			.data = lex->head,
			.size = (uintsize)(head - lex->head),
		};
		
		lex->head = head+1;
		LangC_IgnoreUntilNewline(lex);
	}
	else
	{
		LangC_LexerError(lex, "'#include' should have a path to a file sorrounded by \"\" or <>.");
		LangC_IgnoreUntilNewline(lex);
		return;
	}
	
	const char* contents = LangC_TryToLoadFile(pp, path, relative);
	if (contents)
	{
		LangC_Preprocess2(pp, path, contents, lex->file);
		LangC_TracePreprocessor(pp, lex, 2);
	}
	else
	{
		LangC_LexerError(lex, "could not find file '%.*s' when including.", StrFmt(path));
	}
}

internal void
LangC_Preprocess2(LangC_Preprocessor* pp, String path, const char* source, LangC_LexerFile* from)
{
	LangC_Lexer* lex = &(LangC_Lexer) {
		.preprocessor = true,
		.file = from,
	};
	
	LangC_SetupLexer(lex, source);
	LangC_PushLexerFile(lex, path);
	
	SB_ReserveMore(pp->buf, 2048);
	LangC_TracePreprocessor(pp, lex, 1);
	
	LangC_NextToken(lex);
	
	bool32 previous_was_newline = true;
	while (lex->token.kind != LangC_TokenKind_Eof)
	{
		switch (lex->token.kind)
		{
			case LangC_TokenKind_Hashtag:
			{
				if (!previous_was_newline)
				{
					LangC_LexerError(lex, "'#' should be the first token in a line.");
					LangC_NextToken(lex);
					break;
				}
				
				LangC_NextToken(lex);
				String directive = lex->token.as_string;
				
				if (lex->token.kind != LangC_TokenKind_Identifier && !LangC_IsKeyword(lex->token.kind))
				{
					LangC_LexerError(lex, "invalid preprocessor directive '%.*s'.", StrFmt(directive));
					LangC_IgnoreUntilNewline(lex);
					break;
				}
				
				bool32 not = false;
				if (MatchCString("include", directive.data, directive.size))
				{
					LangC_NextToken(lex);
					LangC_PreprocessInclude(pp, lex);
				}
				else if (MatchCString("ifdef", directive.data, directive.size) ||
						 MatchCString("ifndef", directive.data, directive.size) && (not = true))
				{
					LangC_NextToken(lex);
					LangC_PreprocessIfDef(pp, lex);
				}
				else if (MatchCString("if", directive.data, directive.size))
				{
					LangC_NextToken(lex);
					LangC_PreprocessIf(pp, lex);
				}
				else if (MatchCString("line", directive.data, directive.size))
				{
					LangC_NextToken(lex);
					
					int32 line = lex->line;
					if (LangC_AssertToken(lex, LangC_TokenKind_IntLiteral))
						line = lex->token.value_int;
					
					lex->line = line-1; // NOTE(ljre): :P
					LangC_TracePreprocessor(pp, lex, 0);
				}
				else if (MatchCString("define", directive.data, directive.size))
				{
					const char* def = lex->head;
					
					LangC_NextToken(lex);
					if (LangC_AssertToken(lex, LangC_TokenKind_Identifier))
					{
						LangC_IgnoreUntilNewline(lex);
						const char* end = lex->head;
						
						String macro_def = {
							.size = (uintsize)(end - def),
							.data = def,
						};
						
						LangC_DefineMacro(pp, macro_def);
					}
				}
				else if (MatchCString("undef", directive.data, directive.size))
				{
					LangC_NextToken(lex);
					
					if (LangC_AssertToken(lex, LangC_TokenKind_Identifier))
					{
						LangC_UndefineMacro(pp, lex->token.value_ident);
					}
				}
				else if (MatchCString("error", directive.data, directive.size))
				{
					const char* begin = lex->head;
					const char* end = lex->head;
					
					while (*end && *end != '\n')
						++end;
					
					uintsize len = end - begin;
					LangC_LexerError(lex, ": %.*s", len, begin);
					
					lex->head = end;
				}
				else if (MatchCString("warning", directive.data, directive.size))
				{
					const char* begin = lex->head;
					const char* end = lex->head;
					
					while (*end && *end != '\n')
						++end;
					
					uintsize len = end - begin;
					LangC_LexerWarning(lex, "%.*s", len, begin);
					
					lex->head = end;
				}
				else
				{
					LangC_LexerError(lex, "unknown pre-processor directive '%.*s'.", StrFmt(directive));
				}
				
				LangC_IgnoreUntilNewline(lex);
				LangC_NextToken(lex);
			} break;
			
			case LangC_TokenKind_Identifier:
			{
				String ident = lex->token.value_ident;
				LangC_Macro* macro;
				
				if (lex->head[0] == '(')
				{
					macro = LangC_FindMacro(pp, ident, 3);
				}
				else
				{
					macro = LangC_FindMacro(pp, ident, 0);
				}
				
				if (macro)
				{
					if (previous_was_newline)
						LangC_TracePreprocessor(pp, lex, 0);
					
					LangC_ExpandMacro(pp, macro, lex);
				}
				else
				{
					SB_PushArray(pp->buf, ident.size, ident.data);
					SB_Push(pp->buf, ' ');
					LangC_NextToken(lex);
				}
				
				previous_was_newline = false;
			} break;
			
			default:
			{
				if (false)
				{
					case LangC_TokenKind_NewLine:
					previous_was_newline = true;
				}
				else
				{
					previous_was_newline = false;
				}
				
				SB_PushArray(pp->buf, lex->token.as_string.size, lex->token.as_string.data);
				SB_Push(pp->buf, ' ');
				LangC_NextToken(lex);
			} break;
		}
	}
	
	// NOTE(ljre): Done preprocessing this file!
}

internal const char*
LangC_Preprocess(String path)
{
	LangC_Preprocessor pp = { 0 };
	
	LangC_DefineMacro(&pp, Str("__STDC__ 1"));
	LangC_DefineMacro(&pp, Str("__STDC_HOSTED__ 1"));
	LangC_DefineMacro(&pp, Str("__STDC_VERSION__ 199901L"));
	LangC_DefineMacro(&pp, Str("__x86_64 1"));
	LangC_DefineMacro(&pp, Str("_WIN32 1"));
	LangC_DefineMacro(&pp, Str("_WIN64 1"));
	LangC_DefineMacro(&pp, Str("__OCC__ 1"));
	LangC_DefineMacro(&pp, Str("__int64 long long"));
	LangC_DefineMacro(&pp, Str("__int32 int"));
	LangC_DefineMacro(&pp, Str("__int16 short"));
	LangC_DefineMacro(&pp, Str("__int8 char"));
	LangC_DefineMacro(&pp, Str("__inline inline"));
	LangC_DefineMacro(&pp, Str("__inline__ inline"));
	LangC_DefineMacro(&pp, Str("__forceinline inline"));
	LangC_DefineMacro(&pp, Str("__restrict restrict"));
	LangC_DefineMacro(&pp, Str("__restrict__ restrict"));
	LangC_DefineMacro(&pp, Str("__const const"));
	LangC_DefineMacro(&pp, Str("__const__ const"));
	LangC_DefineMacro(&pp, Str("__builtin_va_list void*"));
	LangC_DefineMacro(&pp, Str("__builtin_va_start(l,p) ((l) = &(p)+1)"));
	LangC_DefineMacro(&pp, Str("__builtin_va_end(l) ((l) = NULL)"));
	LangC_DefineMacro(&pp, Str("__attribute(...)"));
	LangC_DefineMacro(&pp, Str("__declspec(...)"));
	LangC_DefineMacro(&pp, Str("__cdecl"));
	LangC_DefineMacro(&pp, Str("__stdcall"));
	LangC_DefineMacro(&pp, Str("__vectorcall"));
	LangC_DefineMacro(&pp, Str("__fastcall"));
	LangC_DefineMacro(&pp, Str("_VA_LIST_DEFINED"));
	LangC_DefineMacro(&pp, Str("va_list void*"));
	
	const char* source = LangC_TryToLoadFile(&pp, path, true);
	
	LangC_Preprocess2(&pp, path, source, NULL);
	
	return pp.buf;
}
