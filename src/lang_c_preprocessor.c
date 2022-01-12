/*
* NOTE(ljre): How does this work
* There are 2 output modes: text and tokens.
* To check which one we are working with, just check 'ctx->tokens' for NULL.
*
* In text mode:
*     The 'ctx->persistent_arena' is where the preprocessed source output will be, so all random
*     objects (O_Macro, O_SourceTrace, etc.) need to be allocated in 'ctx->stage_arena'.
*     At the end, the output should be in the member 'ctx->pre_source' as a null-terminated string.
*     It will use GCC's preprocessor metadata for files, line numbers, etc.
*
* In token mode:
*     Now we are outputing 'C_TokenStream's. They are just simple linked lists of arrays of tokens.
*     Those tokens will have full source-code tracing (including macro expansion).
*
*     There is an edge-case that, when reading the token stream, needs to be handled specially if the
*         preprocessor can't: #pragma
*     It will be written to the stream as { C_TokenKind_HashtagPragma, ..., C_TokenKind_NewLine }.
*
*/

internal C_TokenStream*
C_PushTokenToStream(C_TokenStream* stream, const C_Token* token, Arena* arena)
{
	if (stream->len >= ArrayLength(stream->tokens))
		stream = stream->next = Arena_Push(arena, sizeof(*stream));
	
	stream->tokens[stream->len++] = *token;
	
	return stream;
}

internal void C_PreprocessFile(C_Context* ctx, String path, const char* source, C_Lexer* from);
internal void C_PreprocessIfDef(C_Context* ctx, C_Lexer* lex, bool32 not);
internal void C_PreprocessIf(C_Context* ctx, C_Lexer* lex);

internal void
C_PreprocessWriteToken(C_Context* ctx, const C_Token* token)
{
	if (!ctx->tokens)
	{
		Arena_PushMemory(ctx->persistent_arena, StrFmt(token->as_string));
		Arena_PushMemory(ctx->persistent_arena, StrFmt(token->leading_spaces));
	}
	else if (token->kind != C_TokenKind_NewLine)
	{
		C_Token copy = *token;
		
		if (copy.kind == C_TokenKind_Identifier)
			copy.kind = C_FindKeywordByString(copy.as_string);
		
		ctx->pp.stream = C_PushTokenToStream(ctx->pp.stream, &copy, ctx->persistent_arena);
	}
}

internal void
C_PreprocessWriteNewlineToken(C_Context* ctx, const C_SourceTrace* trace)
{
	C_Token token = {
		.kind = C_TokenKind_NewLine,
		.trace = *trace,
		.as_string = { .size = 1, .data = "\n" },
		.leading_spaces = { 0 },
	};
	
	ctx->pp.stream = C_PushTokenToStream(ctx->pp.stream, &token, ctx->persistent_arena);
}

internal String
C_PreprocessSPrintf(C_Context* ctx, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	
	String result;
	if (ctx->pp.stream)
		result = Arena_VSPrintf(ctx->persistent_arena, fmt, args);
	else
		result = Arena_VSPrintf(ctx->stage_arena, fmt, args);
	
	return result;
}

internal C_PPLoadedFile*
C_LoadFileFromDisk(C_Context* ctx, char* path, uint64 calculated_hash, bool32 relative)
{
	const char* contents = Arena_End(ctx->persistent_arena);
	uintsize len = OS_ReadWholeFile(path, ctx->persistent_arena);
	C_Preprocessor* const pp = &ctx->pp;
	
	if (len)
	{
		uintsize path_len = strlen(path);
		Arena_Pop(ctx->stage_arena, path + path_len + 1);
		
		C_PPLoadedFile* file;
		
		if (pp->loaded_files)
		{
			file = pp->loaded_files;
			while (file->next)
				file = file->next;
			
			file = file->next = Arena_Push(ctx->stage_arena, sizeof *file);
		}
		else
		{
			file = pp->loaded_files = Arena_Push(ctx->stage_arena, sizeof *file);
		}
		
		file->hash = calculated_hash;
		file->contents = contents;
		file->relative = relative;
		file->path = StrMake(path, path_len);
		
		return file;
	}
	
	return NULL;
}

internal const char*
C_TryToLoadFile(C_Context* ctx, String path, bool32 relative, String including_from, String* out_fullpath)
{
	TraceName(path);
	
	// TODO(ljre): Trim the working directory from 'out_fullpath' when needed.
	char* fullpath = Arena_PushDirtyAligned(ctx->stage_arena, MAX_PATH_SIZE, 1);
	C_Preprocessor* const pp = &ctx->pp;
	
	if (relative)
	{
		C_PPLoadedFile* file = pp->loaded_files;
		
		uintsize len = 0;
		if (including_from.size > 0)
		{
			OurMemCopy(fullpath, including_from.data, including_from.size);
			
			int32 last_slash_index = -1;
			for (int32 i = 0; i < including_from.size; ++i)
			{
				if (fullpath[i] == '/')
					last_slash_index = i;
			}
			
			if (last_slash_index == -1)
			{
				last_slash_index = including_from.size;
				fullpath[last_slash_index] = '/';
			}
			
			len = last_slash_index + 1;
		}
		
		OurMemCopy(fullpath + len, path.data, path.size);
		len += path.size;
		
		OS_ResolveFullPath(StrMake(fullpath, len), fullpath, ctx->stage_arena);
		uint64 search_hash = SimpleHashNullTerminated(fullpath);
		
		while (file)
		{
			if (file->hash == search_hash && file->relative)
			{
				if (file->pragma_onced)
					return NULL;
				
				*out_fullpath = file->path;
				Arena_Pop(ctx->stage_arena, fullpath);
				return file->contents;
			}
			
			file = file->next;
		}
		
		file = C_LoadFileFromDisk(ctx, fullpath, search_hash, true);
		if (file)
		{
			*out_fullpath = file->path;
			return file->contents;
		}
	}
	
	for (StringList* it = ctx->options->include_dirs; it; it = it->next)
	{
		String include_dir = it->value;
		include_dir = IgnoreNullTerminator(include_dir);
		
		OurMemCopy(fullpath, include_dir.data, include_dir.size);
		OurMemCopy(fullpath + include_dir.size, path.data, path.size);
		
		String p = {
			.size = include_dir.size + path.size,
			.data = fullpath,
		};
		
		OS_ResolveFullPath(p, fullpath, ctx->stage_arena);
		
		C_PPLoadedFile* file = pp->loaded_files;
		uint64 search_hash = SimpleHashNullTerminated(fullpath);
		
		while (file)
		{
			if (file->hash == search_hash && !file->relative)
			{
				if (file->pragma_onced)
					return NULL;
				
				*out_fullpath = file->path;
				Arena_Pop(ctx->stage_arena, fullpath);
				return file->contents;
			}
			
			file = file->next;
		}
		
		file = C_LoadFileFromDisk(ctx, fullpath, search_hash, true);
		if (file)
		{
			*out_fullpath = file->path;
			return file->contents;
		}
	}
	
	Arena_Pop(ctx->stage_arena, fullpath);
	return NULL;
}

internal void
C_PragmaOncePPFile(C_Context* ctx, String fullpath)
{
	C_Preprocessor* const pp = &ctx->pp;
	C_PPLoadedFile* file = pp->loaded_files;
	uint64 search_hash = SimpleHash(fullpath);
	
	while (file)
	{
		if (file->hash == search_hash)
		{
			file->pragma_onced = true;
			break;
		}
		
		file = file->next;
	}
}

internal void
C_IgnoreUntilNewline(C_Lexer* lex)
{
	//while (lex->token.kind && lex->token.kind != C_TokenKind_NewLine)
	//C_NextToken(lex);
	
	if (lex->token.kind && lex->token.kind != C_TokenKind_NewLine)
	{
		while (lex->head[0] && (lex->head[0] != '\n' || lex->head[-1] == '\\'))
			++lex->head;
		
		C_NextToken(lex);
	}
}

internal C_Macro*
C_DefineMacro(C_Context* ctx, String definition, const C_SourceTrace* from)
{
	// TODO(ljre): Macros redefinitions are OK if the previous definition has the exact same tokens and spaces.
	// NOTE(ljre): Possible optimization: tokenize macro definition ahead of time.
	//             More than half of time spent in C_ExpandMacro is calling C_NextToken!
	
	C_Preprocessor* const pp = &ctx->pp;
	const char* def = Arena_NullTerminateString(ctx->stage_arena, definition);
	
	const char* ident_begin = def;
	const char* head = def;
	
	while (C_IsIdentChar(head[0]))
		++head;
	
	String name = {
		.data = ident_begin,
		.size = (uintsize)(head - ident_begin),
	};
	
	if (!from && (StringListHas(ctx->options->predefined_macros, name) || StringListHas(ctx->options->preundefined_macros, name)))
		return NULL;
	
	bool32 is_func_like = (head[0] == '(');
	uint32 param_count;
	
	if (is_func_like)
	{
		// TODO(ljre): Check if parameter names are valid identifiers.
		C_IgnoreWhitespaces(&head, false);
		
		param_count = 0;
		if (*++head != ')')
		{
			++param_count;
			
			for (; head[0] && head[0] != ')'; ++head)
			{
				if (head[0] == ',')
					++param_count;
			}
		}
	}
	else
		param_count = UINT32_MAX;
	
	C_Macro* result = Arena_Push((ctx->tokens) ? ctx->persistent_arena : ctx->stage_arena, sizeof(*result));
	
	result->def = def;
	result->name = name;
	result->param_count = param_count;
	
	if (from)
	{
		result->file = from->file;
		result->line = from->line;
		result->col = from->col;
	}
	else
	{
		static C_SourceFileTrace command_line = {
			.path = StrInit("<command_line>"),
			.is_system_file = true,
		};
		
		result->file = &command_line;
		result->line = result->col = 1;
	}
	
	uint64 hash = SimpleHash(name);
	
	if (Map_RemoveWithHash(pp->func_macros, name, hash) || Map_RemoveWithHash(pp->obj_macros, name, hash))
	{
		// TODO(ljre): Warn macro redefinition
	}
	
	if (is_func_like)
		Map_InsertWithHash(pp->func_macros, name, result, hash);
	else
		Map_InsertWithHash(pp->obj_macros, name, result, hash);
	
	return result;
}

internal void
C_UndefineMacro(C_Context* ctx, String name)
{
	C_Preprocessor* const pp = &ctx->pp;
	if (StringListHas(ctx->options->predefined_macros, name) || StringListHas(ctx->options->preundefined_macros, name))
		return;
	
	uint64 hash = SimpleHash(name);
	
	if (!Map_RemoveWithHash(pp->func_macros, name, hash))
		Map_RemoveWithHash(pp->obj_macros, name, hash);
}

// NOTE(ljre): returns NULL if it couldn't find it.
//
//             if type is 0, then it will return an object-like macro.
//             if type is 1, then it will return a function-like macro.
//             if type is 2, then it will return any macro.
//             if type is 3, then it will return a function-like macro with this name, otherwise
//                                a object-like macro is still ok.
internal C_Macro*
C_FindMacro(C_Context* ctx, String name, int32 type)
{
	TraceName(name);
	Assert(type >= 0 && type <= 3);
	C_Preprocessor* pp = &ctx->pp;
	uint64 hash = SimpleHash(name);
	
	C_Macro* result = NULL;
	
	switch (type)
	{
		case 0: result = Map_FetchWithHash(pp->obj_macros, name, hash); break;
		case 1: result = Map_FetchWithHash(pp->func_macros, name, hash); break;
		
		case 2: case 3:
		{
			result = Map_FetchWithHash(pp->func_macros, name, hash);
			if (!result)
				result = Map_FetchWithHash(pp->obj_macros, name, hash);
		} break;
	}
	
	return result;
}

internal bool32
C_TokenWasGeneratedByMacro(const C_Token* token, const C_Macro* macro)
{
	const C_SourceTrace* trace = &token->trace;
	while (trace->invocation)
	{
		if (trace->macro_def == macro)
			return true;
		
		trace = trace->invocation;
	}
	
	return false;
}

internal void
C_TracePreprocessor(C_Context* ctx, C_Lexer* lex, uint32 flags)
{
	Assert(flags < 16);
	
	if (!ctx->tokens)
	{
		static const char* const flag_table[] = {
			NULL,
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
			Arena_Printf(ctx->persistent_arena, "# %i \"%S\" %s\n", lex->trace.line, StrFmt(lex->trace.file->path), flag_table[flags]);
		else
			Arena_Printf(ctx->persistent_arena, "# %i \"%S\"\n", lex->trace.line, StrFmt(lex->trace.file->path));
	}
}

internal void
C_ExpandMacro(C_Context* ctx, C_Macro* macro, C_Lexer* parent_lex, String leading_spaces)
{
	TraceName(macro->name);
	C_SourceTrace invocation_trace = parent_lex->token.trace;
	
	// NOTE(ljre): Handle special macros
	if (MatchCString("__LINE__", macro->name))
	{
		int32 line = parent_lex->trace.line;
		
		C_Token tok = {
			.kind = C_TokenKind_IntLiteral,
			.as_string = C_PreprocessSPrintf(ctx, "%i", line),
			.leading_spaces = StrNull,
			.value_int = line,
			.trace = parent_lex->token.trace,
		};
		
		C_PushTokenToFront(parent_lex, &tok);
		C_NextToken(parent_lex);
		return;
	}
	else if (MatchCString("__FILE__", macro->name))
	{
		String file = parent_lex->trace.file->path;
		
		C_Token tok = {
			.kind = C_TokenKind_StringLiteral,
			.as_string = C_PreprocessSPrintf(ctx, "\"%S\"", StrFmt(file)),
			.leading_spaces = StrNull,
			.value_str = file,
			.trace = parent_lex->token.trace,
		};
		
		C_PushTokenToFront(parent_lex, &tok);
		C_NextToken(parent_lex);
		return;
	}
	
	// NOTE(ljre): Normal macro expansion.
	const char* def_head = macro->def;
	
	Map* params = NULL;
	uint32 param_count = 0;
	
	bool8 is_func_like = (macro->param_count != UINT32_MAX);
	
	// NOTE(ljre): Ignore macro name
	//while (C_IsIdentChar(*def_head))
	//++def_head;
	def_head += macro->name.size;
	
	// NOTE(ljre): Define parameters as macros
	if (is_func_like)
	{
		params = Map_Create(ctx->stage_arena, 256);
		
		C_NextToken(parent_lex); // eat macro name
		C_EatToken(parent_lex, C_TokenKind_LeftParen);
		
		while (*def_head && *def_head++ != '(');
		
		if (*def_head != ')')
		{
			do
			{
				C_IgnoreWhitespaces(&def_head, false);
				++param_count;
				Assert(param_count <= macro->param_count);
				
				String name;
				char* buf = Arena_End(ctx->stage_arena);
				
				// NOTE(ljre): __VA_ARGS__
				if (def_head[0] == '.' && def_head[1] == '.' && def_head[2] == '.')
				{
					def_head += 3;
					name = Str("__VA_ARGS__");
					
					int32 nesting = 1;
					while (parent_lex->token.kind)
					{
						if (parent_lex->token.kind == C_TokenKind_RightParen)
						{
							if (--nesting <= 0)
								break;
						}
						else if (parent_lex->token.kind == C_TokenKind_LeftParen)
							++nesting;
						
						Arena_PushMemory(ctx->stage_arena, StrFmt(parent_lex->token.as_string));
						Arena_PushMemory(ctx->stage_arena, StrFmt(parent_lex->token.leading_spaces));
						
						C_NextToken(parent_lex);
					}
				}
				else
				{
					const char* name_begin = def_head;
					
					while (C_IsIdentChar(*def_head))
						++def_head;
					
					name = StrMake(name_begin, (uintsize)(def_head - name_begin));
					
					int32 nesting = 1;
					while (parent_lex->token.kind && (parent_lex->token.kind != C_TokenKind_Comma || nesting > 1))
					{
						if (parent_lex->token.kind == C_TokenKind_RightParen)
						{
							if (--nesting <= 0)
								break;
						}
						else if (parent_lex->token.kind == C_TokenKind_LeftParen)
							++nesting;
						
						Arena_PushMemory(ctx->stage_arena, StrFmt(parent_lex->token.as_string));
						Arena_PushMemory(ctx->stage_arena, StrFmt(parent_lex->token.leading_spaces));
						
						C_NextToken(parent_lex);
					}
				}
				
				Arena_PushMemory(ctx->stage_arena, 1, "");
				
				Map_Insert(params, name, buf);
				
				C_IgnoreWhitespaces(&def_head, false);
			}
			while (def_head[0] == ',' && ++def_head && C_TryToEatToken(parent_lex, C_TokenKind_Comma));
		}
		
		while (def_head[0] == ',')
		{
			++def_head;
			
			C_IgnoreWhitespaces(&def_head, false);
			++param_count;
			String name;
			
			if (def_head[0] == '.' && def_head[1] == '.' && def_head[2] == '.')
			{
				def_head += 3;
				name = Str("__VA_ARGS__");
			}
			else
			{
				const char* name_begin = def_head;
				
				while (C_IsIdentChar(*def_head))
					++def_head;
				
				name = StrMake(name_begin, (uintsize)(def_head - name_begin));
			}
			
			Map_Insert(params, name, "");
		}
		
		while (parent_lex->token.kind && parent_lex->token.kind != C_TokenKind_RightParen)
		{
			C_NextToken(parent_lex);
		}
		
		C_AssertToken(parent_lex, C_TokenKind_RightParen);
		leading_spaces = parent_lex->token.leading_spaces;
		
		while (*def_head && *def_head++ != ')');
	}
	
	// NOTE(ljre): Expand macro
	C_Lexer* lex = &(C_Lexer) {
		.preprocessor = true,
		.trace = {
			.file = macro->file,
			.line = macro->line,
			.col = macro->col,
		},
	};
	
	// NOTE(ljre): If the lexer already has waiting tokens, we want to insert right before them.
	//             Save and append them to the end after expanding.
	C_TokenList* saved_first_token = parent_lex->waiting_token;
	C_TokenList* saved_last_token = parent_lex->last_waiting_token;
	
	parent_lex->waiting_token = NULL;
	
	C_SetupLexer(lex, def_head, ctx, parent_lex->arena);
	
	// NOTE(ljre): Insert Trace
	{
		Arena* arena = (ctx->tokens) ? ctx->persistent_arena : ctx->stage_arena;
		C_SourceTrace* trace = Arena_Push(arena, sizeof(*trace));
		*trace = invocation_trace;
		
		lex->trace.invocation = trace;
		lex->trace.macro_def = macro;
	}
	
	C_NextToken(lex);
	
	// NOTE(ljre): Expand macro.
	while (lex->token.kind != C_TokenKind_Eof)
	{
		switch (lex->token.kind)
		{
			case C_TokenKind_Hashtag:
			{
				C_NextToken(lex);
				
				const char* param;
				if (!is_func_like ||
					!C_AssertToken(lex, C_TokenKind_Identifier) ||
					!(param = Map_Fetch(params, lex->token.value_ident)))
				{
					C_TraceError(ctx, &lex->trace, "expected a macro parameter for stringification.");
				}
				else
				{
					// TODO(ljre): Fix this.
					String str = C_PreprocessSPrintf(ctx, "\"%s\"", param);
					
					C_Token tok = {
						.kind = C_TokenKind_StringLiteral,
						.leading_spaces = lex->token.leading_spaces,
						.as_string = str,
						.value_str = {
							.size = str.size - 2,
							.data = str.data + 1,
						},
						.trace = lex->trace,
					};
					
					C_PushToken(parent_lex, &tok);
					C_NextToken(lex);
				}
			} break;
			
			case C_TokenKind_Identifier:
			{
				const char* param;
				if (lex->token.kind == C_TokenKind_Identifier &&
					!lex->token_was_pushed &&
					(param = Map_Fetch(params, lex->token.value_ident)))
				{
					C_PushStringOfTokens(lex, param);
					C_NextToken(lex);
					break;
				}
			} /* fallthrough */
			
			default:
			{
				C_Token tok = lex->token;
				C_Token incoming = C_PeekIncomingToken(lex);
				bool32 from_macro = lex->token_was_pushed;
				C_Macro* to_expand;
				
				if (incoming.kind == C_TokenKind_DoubleHashtag)
				{
					C_NextToken(lex); // NOTE(ljre): Eat left side
					C_NextToken(lex); // NOTE(ljre): Eat ##
					const char* param;
					
					if (lex->token.kind == C_TokenKind_Identifier &&
						(param = Map_Fetch(params, lex->token.value_ident)))
					{
						C_PushStringOfTokens(lex, param);
						C_NextToken(lex);
					}
					
					C_Token other_tok = lex->token;
					C_NextToken(lex);
					
					uintsize len = tok.as_string.size + other_tok.as_string.size + 1;
					char* buf = Arena_Push(ctx->stage_arena, len);
					
					OurMemCopy(buf, tok.as_string.data, tok.as_string.size);
					OurMemCopy(buf + tok.as_string.size, other_tok.as_string.data, other_tok.as_string.size);
					buf[tok.as_string.size + other_tok.as_string.size] = 0;
					
					// TODO(ljre): Check if result of concatenation should be expanded on the 4th step.
					C_PushStringOfTokens(parent_lex, buf);
				}
				else if (from_macro &&
						 tok.kind == C_TokenKind_Identifier &&
						 (to_expand = C_FindMacro(ctx, tok.value_ident,
												  (incoming.kind == C_TokenKind_LeftParen) ? 3 : 0)))
				{
					// NOTE(ljre): Tokens originated from parameters shall be expanded first.
					C_ExpandMacro(ctx, to_expand, lex, tok.leading_spaces);
				}
				else
				{
					C_NextToken(lex);
					C_PushToken(parent_lex, &tok);
				}
			} break;
		}
	}
	
	if (parent_lex->waiting_token)
	{
		parent_lex->last_waiting_token->next = saved_first_token;
		parent_lex->last_waiting_token->token.leading_spaces = leading_spaces;
	}
	else
	{
		parent_lex->waiting_token = saved_first_token;
	}
	
	parent_lex->last_waiting_token = saved_last_token;
	
	// NOTE(ljre): At this point, the parent_lex is going to have the macro name *or*
	//             the ending ')' of the parameter list. We can eat a token safely.
	C_NextToken(parent_lex);
}

internal bool32
C_TryToExpandIdent(C_Context* ctx, C_Lexer* lex)
{
	String ident = lex->token.value_ident;
	String leading_spaces = lex->token.leading_spaces;
	C_Macro* macro = NULL;
	
	if (C_PeekIncomingToken(lex).kind == C_TokenKind_LeftParen)
		macro = C_FindMacro(ctx, ident, 3);
	else
		macro = C_FindMacro(ctx, ident, 0);
	
	if (macro && !C_TokenWasGeneratedByMacro(&lex->token, macro))
	{
		C_ExpandMacro(ctx, macro, lex, leading_spaces);
		return true;
	}
	
	return false;
}

internal int32 C_EvalPreprocessorExprBinary(C_Context* ctx, C_Lexer* lex, int32 level);

internal int32
C_EvalPreprocessorExprFactor(C_Context* ctx, C_Lexer* lex)
{
	beginning:;
	int32 result = 0;
	
	switch (lex->token.kind)
	{
		// NOTE(ljre): Unary operators
		case C_TokenKind_Minus:
		{
			C_NextToken(lex);
			return -C_EvalPreprocessorExprFactor(ctx, lex);
		} break;
		
		case C_TokenKind_Plus:
		{
			C_NextToken(lex);
			goto beginning;
		} break;
		
		case C_TokenKind_Not:
		{
			C_NextToken(lex);
			return ~C_EvalPreprocessorExprFactor(ctx, lex);
		} break;
		
		case C_TokenKind_LNot:
		{
			C_NextToken(lex);
			return !C_EvalPreprocessorExprFactor(ctx, lex);
		} break;
		
		// NOTE(ljre): Factors
		case C_TokenKind_IntLiteral:
		case C_TokenKind_LIntLiteral:
		case C_TokenKind_LLIntLiteral:
		{
			result = (int32)lex->token.value_int;
			C_NextToken(lex);
		} break;
		
		case C_TokenKind_UintLiteral:
		case C_TokenKind_LUintLiteral:
		case C_TokenKind_LLUintLiteral:
		{
			result = (int32)lex->token.value_uint;
			C_NextToken(lex);
		} break;
		
		case C_TokenKind_FloatLiteral:
		{
			result = (int32)lex->token.value_float;
			C_NextToken(lex);
		} break;
		
		case C_TokenKind_DoubleLiteral:
		{
			result = (int32)lex->token.value_double;
			C_NextToken(lex);
		} break;
		
		case C_TokenKind_WideStringLiteral:
		case C_TokenKind_StringLiteral:
		{
			// TODO(ljre): What should happen if it's a string literal?
			result = (lex->token.value_str.size > 0);
			C_NextToken(lex);
		} break;
		
		case C_TokenKind_Identifier:
		{
			String name = lex->token.value_ident;
			
			if (MatchCString("defined", name))
			{
				C_NextToken(lex);
				bool32 has_paren = C_TryToEatToken(lex, C_TokenKind_LeftParen);
				
				if (C_AssertToken(lex, C_TokenKind_Identifier))
				{
					result = (C_FindMacro(ctx, lex->token.value_ident, 2) != NULL);
					C_NextToken(lex);
				}
				
				if (has_paren)
					C_EatToken(lex, C_TokenKind_RightParen);
			}
			else
			{
				bool32 is_func = (C_PeekIncomingToken(lex).kind == C_TokenKind_LeftParen);
				C_Macro* macro = C_FindMacro(ctx, name, is_func);
				
				if (macro)
				{
					C_ExpandMacro(ctx, macro, lex, lex->token.leading_spaces);
					goto beginning;
				}
				else
				{
					result = 0;
				}
			}
		} break;
	}
	
	return result;
}

internal int32
C_EvalPreprocessorExprOp(C_TokenKind op, int32 left, int32 right)
{
	switch (op)
	{
		case C_TokenKind_Comma: return right;
		case C_TokenKind_Assign: return right;
		case C_TokenKind_PlusAssign:
		case C_TokenKind_Plus: return left + right;
		case C_TokenKind_MinusAssign:
		case C_TokenKind_Minus: return left - right;
		case C_TokenKind_MulAssign:
		case C_TokenKind_Mul: return left * right;
		case C_TokenKind_DivAssign:
		case C_TokenKind_Div:
		{
			if (right == 0)
				return 0;
			return left / right;
		};
		case C_TokenKind_ModAssign:
		case C_TokenKind_Mod:
		{
			if (right == 0)
				return 0;
			return left % right;
		};
		case C_TokenKind_LeftShiftAssign:
		case C_TokenKind_LeftShift: return left << right;
		case C_TokenKind_RightShiftAssign:
		case C_TokenKind_RightShift: return left >> right;
		case C_TokenKind_AndAssign:
		case C_TokenKind_And: return left & right;
		case C_TokenKind_OrAssign:
		case C_TokenKind_Or: return left | right;
		case C_TokenKind_XorAssign:
		case C_TokenKind_Xor: return left ^ right;
		
		case C_TokenKind_LOr: return left || right;
		case C_TokenKind_LAnd: return left && right;
		case C_TokenKind_Equals: return left == right;
		case C_TokenKind_NotEquals: return left != right;
		case C_TokenKind_LThan: return left < right;
		case C_TokenKind_LEqual: return left <= right;
		case C_TokenKind_GThan: return left > right;
		case C_TokenKind_GEqual: return left >= right;
	}
	
	return 0;
}

internal int32
C_EvalPreprocessorExprBinary(C_Context* ctx, C_Lexer* lex, int32 level)
{
	int32 result = C_EvalPreprocessorExprFactor(ctx, lex);
	
	C_OperatorPrecedence prec;
	while (prec = C_operators_precedence[lex->token.kind],
		   prec.level > level)
	{
		C_TokenKind op = lex->token.kind;
		C_NextToken(lex);
		int32 right = C_EvalPreprocessorExprFactor(ctx, lex);
		
		C_TokenKind lookahead = lex->token.kind;
		C_OperatorPrecedence lookahead_prec = C_operators_precedence[lookahead];
		
		while (lookahead_prec.level > 0 &&
			   (lookahead_prec.level > prec.level ||
				(lookahead_prec.level == prec.level && lookahead_prec.right2left))) {
			C_NextToken(lex);
			
			if (lookahead == C_TokenKind_QuestionMark) {
				int32 if_true = C_EvalPreprocessorExprBinary(ctx, lex, 0);
				
				C_EatToken(lex, C_TokenKind_Colon);
				int32 if_false = C_EvalPreprocessorExprBinary(ctx, lex, level - 1);
				
				right = right ? if_true : if_false;
			} else {
				int32 other = C_EvalPreprocessorExprBinary(ctx, lex, level + 1);
				
				right = C_EvalPreprocessorExprOp(lookahead, right, other);
			}
			
			lookahead = lex->token.kind;
			lookahead_prec = C_operators_precedence[lookahead];
		}
		
		result = C_EvalPreprocessorExprOp(op, result, right);
	}
	
	return result;
}

internal int32
C_EvalPreprocessorExpr(C_Context* ctx, C_Lexer* lex)
{
	return C_EvalPreprocessorExprBinary(ctx, lex, 0);
}

internal void
C_IgnoreUntilEndOfIf(C_Context* ctx, C_Lexer* lex, bool32 already_matched)
{
	int32 nesting = 1;
	
	while (lex->token.kind && nesting > 0)
	{
		switch (lex->token.kind)
		{
			case C_TokenKind_Eof:
			{
				C_TraceError(ctx, &lex->token.trace, "unclosed conditional pre-processor block.");
				goto out_of_the_loop;
			}
			
			case C_TokenKind_Hashtag:
			{
				C_NextToken(lex);
				
				if (C_AssertToken(lex, C_TokenKind_Identifier))
				{
					String directive = lex->token.value_ident;
					bool32 not = false;
					
					if (!already_matched && nesting == 1)
					{
						if (MatchCString("elifdef", directive) ||
							(not = MatchCString("elifndef", directive)))
						{
							C_NextToken(lex);
							C_PreprocessIfDef(ctx, lex, not);
							
							goto out_of_the_loop;
						}
						else if (MatchCString("elif", directive))
						{
							C_NextToken(lex);
							C_PreprocessIf(ctx, lex);
							
							goto out_of_the_loop;
						}
						else if (MatchCString("else", directive))
						{
							C_NextToken(lex);
							
							goto out_of_the_loop;
						}
					}
					
					if (MatchCString("ifdef", directive) ||
						MatchCString("ifndef", directive) ||
						MatchCString("if", directive))
					{
						++nesting;
					}
					else if (MatchCString("endif", directive))
					{
						--nesting;
						
						if (nesting <= 0)
						{
							C_IgnoreUntilNewline(lex);
							break;
						}
					}
				}
			} /* fallthrough */
			
			default:
			{
				C_IgnoreUntilNewline(lex);
				C_TryToEatToken(lex, C_TokenKind_NewLine);
			} break;
		}
	}
	
	out_of_the_loop:;
	
	C_TracePreprocessor(ctx, lex, 0);
}

internal void
C_PreprocessIfDef(C_Context* ctx, C_Lexer* lex, bool32 not)
{
	bool32 result = false;
	
	if (C_AssertToken(lex, C_TokenKind_Identifier))
	{
		String name = lex->token.value_ident;
		result = (C_FindMacro(ctx, name, 2) != NULL) ^ not;
		
		C_NextToken(lex);
	}
	
	if (!result)
	{
		C_IgnoreUntilNewline(lex);
		C_IgnoreUntilEndOfIf(ctx, lex, false);
	}
}

internal void
C_PreprocessIf(C_Context* ctx, C_Lexer* lex)
{
	int32 result = C_EvalPreprocessorExpr(ctx, lex);
	
	if (result == 0)
		C_IgnoreUntilEndOfIf(ctx, lex, false);
}

internal void
C_PreprocessInclude(C_Context* ctx, C_Lexer* lex)
{
	String path;
	bool32 relative;
	
	if (lex->token.kind == C_TokenKind_StringLiteral)
	{
		relative = true;
		path = lex->token.value_str;
		
		C_IgnoreUntilNewline(lex);
	}
	else if (lex->token.kind == C_TokenKind_LThan)
	{
		relative = false;
		const char* head = lex->head; // NOTE(ljre): 'lex->head' should be at the beginning of the next token.
		
		while (*head && *head != '>' && *head != '\n')
			++head;
		
		if (*head != '>')
		{
			C_TraceError(ctx, &lex->token.trace, "missing '>' at the end of file path.");
			C_IgnoreUntilNewline(lex);
			return;
		}
		
		path = (String) {
			.data = lex->head,
			.size = (uintsize)(head - lex->head),
		};
		
		lex->head = head+1;
		C_IgnoreUntilNewline(lex);
	}
	else
	{
		C_TraceError(ctx, &lex->token.trace, "'#include' should have a path to a file sorrounded by \"\" or <>.");
		C_IgnoreUntilNewline(lex);
		return;
	}
	
	String fullpath;
	const char* contents = C_TryToLoadFile(ctx, path, relative, lex->trace.file->path, &fullpath);
	if (contents)
	{
		C_PreprocessFile(ctx, fullpath, contents, lex);
		C_TracePreprocessor(ctx, lex, 2);
	}
	else
		C_TraceError(ctx, &lex->token.trace, "could not find file '%S' when including.", StrFmt(path));
}

internal void
C_PreprocessFile(C_Context* ctx, String path, const char* source, C_Lexer* from)
{
	TraceName(path);
	
	C_Lexer* lex = &(C_Lexer) {
		.preprocessor = true,
		.trace.file = from ? from->trace.file : NULL,
	};
	
	C_SetupLexer(lex, source, ctx, ctx->stage_arena);
	C_PushFileTrace(lex, path, from);
	
	C_TracePreprocessor(ctx, lex, 1);
	
	C_NextToken(lex);
	
	bool32 previous_was_newline = true;
	while (lex->token.kind != C_TokenKind_Eof)
	{
		switch (lex->token.kind)
		{
			case C_TokenKind_Hashtag:
			{
				if (!previous_was_newline)
				{
					C_TraceError(ctx, &lex->token.trace, "'#' should be the first token in a line.");
					C_NextToken(lex);
					break;
				}
				
				C_NextToken(lex);
				String directive = lex->token.as_string;
				
				if (lex->token.kind != C_TokenKind_Identifier)
				{
					C_TraceError(ctx, &lex->token.trace, "preprocessor directive has to be identifier.");
					C_IgnoreUntilNewline(lex);
					break;
				}
				
				bool32 not = false;
				if (MatchCString("include", directive))
				{
					C_NextToken(lex);
					C_PreprocessInclude(ctx, lex);
				}
				else if (MatchCString("ifdef", directive) ||
						 (not = MatchCString("ifndef", directive)))
				{
					C_NextToken(lex);
					C_PreprocessIfDef(ctx, lex, not);
				}
				else if (MatchCString("if", directive))
				{
					C_NextToken(lex);
					C_PreprocessIf(ctx, lex);
				}
				else if (MatchCString("elif", directive) ||
						 MatchCString("elifdef", directive) ||
						 MatchCString("elifndef", directive) ||
						 MatchCString("else", directive))
				{
					C_NextToken(lex);
					C_IgnoreUntilEndOfIf(ctx, lex, true);
				}
				else if (MatchCString("endif", directive))
				{
					C_NextToken(lex);
					// NOTE(ljre): :P
				}
				else if (MatchCString("line", directive))
				{
					C_NextToken(lex);
					
					int32 line = lex->trace.line;
					if (C_AssertToken(lex, C_TokenKind_IntLiteral))
						line = lex->token.value_int;
					
					lex->trace.line = line-1; // NOTE(ljre): :P
					C_TracePreprocessor(ctx, lex, 0);
				}
				else if (MatchCString("define", directive))
				{
					const char* def = lex->head;
					
					C_NextToken(lex);
					if (C_AssertToken(lex, C_TokenKind_Identifier))
					{
						while (lex->head[0] && (lex->head[0] != '\n' || lex->head[-1] == '\\'))
							++lex->head;
						
						const char* end = lex->head;
						
						String macro_def = {
							.size = (uintsize)(end - def),
							.data = def,
						};
						
						C_DefineMacro(ctx, macro_def, &lex->token.trace);
					}
				}
				else if (MatchCString("undef", directive))
				{
					C_NextToken(lex);
					
					if (C_AssertToken(lex, C_TokenKind_Identifier))
						C_UndefineMacro(ctx, lex->token.value_ident);
				}
				else if (MatchCString("error", directive))
				{
					const char* begin = lex->head;
					const char* end = lex->head;
					
					while (*end && *end != '\n')
						++end;
					
					uintsize len = end - begin;
					C_TraceError(ctx, &lex->token.trace, "\"%S\"", len, begin);
					
					lex->head = end;
				}
				else if (MatchCString("warning", directive))
				{
					const char* begin = lex->head;
					const char* end = lex->head;
					
					while (*end && *end != '\n')
						++end;
					
					uintsize len = end - begin;
					C_TraceWarning(ctx, &lex->token.trace, C_Warning_UserWarning, "\"%S\"", len, begin);
					
					lex->head = end;
				}
				else if (MatchCString("pragma", directive))
				{
					String leading = lex->token.leading_spaces;
					C_SourceTrace saved_trace = lex->token.trace;
					C_NextToken(lex);
					
					if (lex->token.kind == C_TokenKind_Identifier &&
						MatchCString("once", lex->token.value_ident))
					{
						C_PragmaOncePPFile(ctx, path);
					}
					else
					{
						if (ctx->tokens)
						{
							C_Token pragma = {
								.kind = C_TokenKind_HashtagPragma,
								.trace = saved_trace,
								.as_string = { .data = "#pragma", .size = 7 },
							};
							
							C_PreprocessWriteToken(ctx, &pragma);
							
							do
							{
								if (lex->token.kind != C_TokenKind_Identifier || !C_TryToExpandIdent(ctx, lex))
								{
									C_PreprocessWriteToken(ctx, &lex->token);
									C_NextToken(lex);
								}
							}
							while (lex->token.kind && lex->token.kind != C_TokenKind_NewLine);
							
							C_PreprocessWriteNewlineToken(ctx, &lex->token.trace);
						}
						else
						{
							Arena_Printf(ctx->persistent_arena, "#pragma%S", StrFmt(leading));
						}
						break;
					}
				}
				else
					C_TraceError(ctx, &lex->token.trace, "unknown pre-processor directive '%S'.", StrFmt(directive));
				
				C_IgnoreUntilNewline(lex);
				C_NextToken(lex);
				Arena_PushMemory(ctx->persistent_arena, 1, "\n");
			} break;
			
			case C_TokenKind_Identifier:
			{
				if (!C_TryToExpandIdent(ctx, lex))
				{
					C_PreprocessWriteToken(ctx, &lex->token);
					C_NextToken(lex);
				}
				
				previous_was_newline = false;
			} break;
			
			default:
			{
				previous_was_newline = false;
				
				if (false)
				{
					case C_TokenKind_NewLine:
					previous_was_newline = true;
				}
				
				C_PreprocessWriteToken(ctx, &lex->token);
				C_NextToken(lex);
			} break;
		}
	}
	
	// NOTE(ljre): If we are the main file, write EOF.
	if (!from)
		C_PreprocessWriteToken(ctx, &lex->token);
}

internal bool32
C_Preprocess(C_Context* ctx)
{
	Trace();
	
	String fullpath;
	ctx->source = C_TryToLoadFile(ctx, ctx->input_file, true, StrNull, &fullpath);
	if (ctx->source)
	{
		if (!ctx->tokens)
		{
			ctx->pre_source = Arena_End(ctx->persistent_arena);
			ctx->use_stage_arena_for_warnings = true;
			
			ctx->pp.obj_macros = Map_Create(ctx->stage_arena, 2048);
			ctx->pp.func_macros = Map_Create(ctx->stage_arena, 2048);
		}
		else
		{
			ctx->pp.stream = ctx->tokens;
			
			ctx->pp.obj_macros = Map_Create(ctx->persistent_arena, 2048);
			ctx->pp.func_macros = Map_Create(ctx->persistent_arena, 2048);
		}
		
		for (StringList* it = ctx->options->predefined_macros; it; it = it->next)
			C_DefineMacro(ctx, it->value, NULL);
		
		C_PreprocessFile(ctx, fullpath, ctx->source, NULL);
	}
	else
	{
		Print("error: could not open input file '%S'.\n", StrFmt(ctx->input_file));
		++ctx->error_count;
	}
	
	ctx->use_stage_arena_for_warnings = false;
	
	return ctx->error_count == 0;
}
