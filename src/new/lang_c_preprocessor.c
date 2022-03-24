//~ NOTE(ljre): Outputing tokens.
internal void
C_PreprocessWriteToken(C_Context* ctx, C_Preprocessor* pp, const C_Token* token, C_SourceFileTrace* file_trace)
{
	//Assert(token->kind != C_TokenKind_Eof);
	
	*pp->out = *token;
	
	if (file_trace)
		pp->out->trace.file = file_trace;
	
	if (pp->out->kind == C_TokenKind_Identifier)
	{
		C_TokenKind kw = C_FindKeywordByString(pp->out->as_string);
		
		if (kw)
			pp->out->kind = kw;
	}
	
	if (pp->out->kind == C_TokenKind_UnclosedQuote)
		C_TraceError(ctx, &pp->out->trace, "unclosed quote.");
	
	pp->out = Arena_Push(ctx->array_arena, sizeof(*pp->out));
}

//~ NOTE(ljre): Token reading & queueing.
internal inline C_TokenQueue**
C_PushTokenToQueue(Arena* arena, const C_Token* token, C_TokenQueue** queue)
{
	Assert(token->kind != C_TokenKind_Eof);
	
	C_TokenQueue* q = Arena_Push(arena, sizeof(*q));
	q->token = *token;
	
	q->next = *queue;
	*queue = q;
	queue = &q->next;
	
	return queue;
}

internal inline C_TokenQueue**
C_PushTokenRangeToQueue(Arena* arena, const C_Token* first, const C_Token* end, C_TokenQueue** queue)
{
	for (; first < end; ++first)
		queue = C_PushTokenToQueue(arena, first, queue);
	
	return queue;
}

internal inline C_TokenQueue**
C_PushTokenFromMacroToQueue(C_Context* ctx, const C_Token* first, const C_Token* end, C_TokenQueue** queue, C_SourceTrace* invocation, C_Macro* macro_def)
{
	for (; first < end; ++first)
	{
		C_Token tok = *first;
		
		tok.trace.file = macro_def->file;
		tok.trace.invocation = invocation;
		tok.trace.macro_def = macro_def;
		
		queue = C_PushTokenToQueue(ctx->scratch_arena, &tok, queue);
	}
	
	return queue;
}

internal String
C_StringifyTokensToArena(Arena* arena, const C_Token* first, const C_Token* end)
{
	char* buf = Arena_End(arena);
	
	for (; first < end; ++first)
	{
		if (first->leading_spaces)
		{
			char* tmp = Arena_PushDirtyAligned(arena, first->leading_spaces, 1);
			MemSet(tmp, ' ', first->leading_spaces);
		}
		
		Arena_PushMemory(arena, StrFmt(first->as_string));
		
		if (first + 1 < end && first[0].trace.line != first[1].trace.line)
		{
			Arena_PushMemory(arena, 1, " ");
		}
	}
	
	uintsize length = (char*)Arena_End(arena) - buf;
	return StrMake(buf, length);
}

//~ NOTE(ljre): File searching & loading.
internal C_PpLoadedFile*
C_TryToLoadFile(C_Context* ctx, C_Preprocessor* pp, String path, bool relative, String including_from)
{
	TraceName(path);
	
	char fullpath[MAX_PATH_SIZE];
	
	if (relative)
	{
		// NOTE(ljre): Write 'fullpath' string.
		{
			uintsize len = 0;
			
			if (including_from.size > 0)
			{
				MemCopy(fullpath, including_from.data, including_from.size);
				
				int32 last_slash = -1;
				for (int32 i = 0; i < including_from.size; ++i)
				{
					if (fullpath[i] == '/')
						last_slash = i;
				}
				
				if (last_slash == -1)
				{
					last_slash = including_from.size;
					fullpath[last_slash] = '/';
				}
				
				len = last_slash + 1;
			}
			
			MemCopy(fullpath + len, path.data, path.size);
			len += path.size;
			OS_ResolveFullPath(StrMake(fullpath, len), fullpath, ctx->scratch_arena);
		}
		
		String fullpath_str = StrFrom(fullpath);
		uint64 hash = SimpleHash(fullpath_str);
		
		// NOTE(ljre): Try to load from cache.
		{
			C_PpLoadedFile* file = Map_FetchWithHash(pp->loaded_files, fullpath_str, hash);
			
			if (file)
				return file;
		}
		
		// NOTE(ljre): Try to load from disk.
		char* file_data = Arena_End(ctx->tree_arena);
		uintsize file_len = OS_ReadWholeFile(fullpath_str, ctx->tree_arena);
		
		if (file_len > 0)
		{
			C_PpLoadedFile* file = Arena_Push(ctx->tree_arena, sizeof(*file));
			
			file->fullpath = Arena_PushString(ctx->tree_arena, fullpath_str);
			file->contents = file_data;
			file->relative = true;
			file->pragma_onced = false;
			
			Map_InsertWithHash(pp->loaded_files, fullpath_str, file, hash);
			
			return file;
		}
	}
	
	for (int32 i = ctx->options->include_dirs_count - 1; i >= 0 ; --i)
	{
		String include_dir = ctx->options->include_dirs[i];
		
		MemCopy(fullpath,                    include_dir.data, include_dir.size);
		MemCopy(fullpath + include_dir.size, path.data, path.size);
		
		OS_ResolveFullPath(StrMake(fullpath, include_dir.size + path.size), fullpath, ctx->scratch_arena);
		
		String fullpath_str = StrFrom(fullpath);
		uint64 hash = SimpleHash(fullpath_str);
		
		// NOTE(ljre): Try to load from cache.
		{
			C_PpLoadedFile* file = Map_FetchWithHash(pp->loaded_files, fullpath_str, hash);
			
			if (file)
				return file;
		}
		
		// NOTE(ljre): Try to load from disk.
		char* file_data = Arena_End(ctx->tree_arena);
		uintsize file_len = OS_ReadWholeFile(fullpath_str, ctx->tree_arena);
		
		if (file_len > 0)
		{
			C_PpLoadedFile* file = Arena_Push(ctx->tree_arena, sizeof(*file));
			
			file->fullpath = Arena_PushString(ctx->tree_arena, fullpath_str);
			file->contents = file_data;
			file->relative = true;
			file->pragma_onced = false;
			
			Map_InsertWithHash(pp->loaded_files, fullpath_str, file, hash);
			
			return file;
		}
	}
	
	return NULL;
}

internal void
C_PragmaOnceFile(C_Context* ctx, C_Preprocessor* pp, String fullpath)
{
	C_PpLoadedFile* file = Map_Fetch(pp->loaded_files, fullpath);
	Assert(file);
	
	file->pragma_onced = true;
	// TODO(ljre): Maybe return error if already set?
}

//~ NOTE(ljre): Macro management.
internal C_Macro*
C_ParseMacroDefinition(C_Context* ctx, C_Preprocessor* pp, C_TokenReader* rd, uint64* out_hash, bool* out_is_func_like)
{
	Assert(rd->head->kind == C_TokenKind_Identifier);
	Assert(!rd->queue);
	
	C_Macro* macro = Arena_Push(ctx->tree_arena, sizeof(*macro));
	macro->name = rd->head->as_string;
	macro->tokens.data = rd->head;
	macro->line = rd->head->trace.line;
	macro->col = rd->head->trace.col;
	macro->file = rd->head->trace.file;
	macro->param_count = -1;
	
	bool is_func_like = false;
	uint64 hash = SimpleHash(macro->name);
	
	// NOTE(ljre): Parse function-like macro.
	C_Token* peeking = C_PeekToken(rd);
	
	if (peeking && peeking->leading_spaces == 0 && peeking->kind == C_TokenKind_LeftParen)
	{
		C_NextToken(rd);
		C_NextToken(rd);
		
		macro->param_count = 0;
		is_func_like = true;
		
		while (rd->head && rd->head->kind != C_TokenKind_NewLine && rd->head->kind != C_TokenKind_RightParen)
		{
			if (rd->head->kind == C_TokenKind_Identifier || rd->head->kind == C_TokenKind_VarArgs)
			{
				C_NextToken(rd);
				++macro->param_count;
				
				if (rd->head->kind != C_TokenKind_Comma)
					break;
			}
			else
			{
				C_TraceErrorRd(ctx, rd, "expected identifier or '...' for macro parameter.");
				break;
			}
			
			C_NextToken(rd);
		}
		
		if (rd->head->kind != C_TokenKind_RightParen)
			C_TraceErrorRd(ctx, rd, "expected closing ')' after macro parameters.");
	}
	
	while (rd->head && rd->head->kind != C_TokenKind_NewLine)
		C_NextToken(rd);
	
	if (rd->head)
		macro->tokens.size = rd->head - macro->tokens.data;
	else
		macro->tokens.size = rd->slice_end - macro->tokens.data;
	
#ifndef NDEBUG
	{
		C_Token* h = macro->tokens.data;
		C_Token* e = macro->tokens.data + macro->tokens.size;
		
		for (; h < e; ++h)
		{
			if (!h->kind || h->kind == C_TokenKind_NewLine)
				Assert(false);
		}
	}
#endif
	
	*out_hash = hash;
	*out_is_func_like = is_func_like;
	
	return macro;
}

internal C_Macro*
C_DefineMacro(C_Context* ctx, C_Preprocessor* pp, C_TokenReader* rd)
{
	Assert(rd->head->kind == C_TokenKind_Identifier);
	Assert(!rd->queue);
	
	if (pp->predefined_macros && Map_Fetch(pp->predefined_macros, rd->head->as_string))
		return NULL;
	
	uint64 hash;
	bool is_func_like;
	C_Macro* macro = C_ParseMacroDefinition(ctx, pp, rd, &hash, &is_func_like);
	
	// NOTE(ljre): If this is a redefinition, check if both are exactly the same.
	C_Macro* other_macro = Map_FetchWithHash(pp->func_macros, macro->name, hash);
	if (!other_macro)
		other_macro = Map_FetchWithHash(pp->obj_macros, macro->name, hash);
	
	if (other_macro)
	{
		bool equal = (other_macro->param_count == macro->param_count ||
					  other_macro->tokens.size == macro->tokens.size);
		
		if (equal)
		{
			C_Token* otok = other_macro->tokens.data;
			C_Token* stok = macro->tokens.data;
			uintsize count = macro->tokens.size;
			
			while (count --> 0)
			{
				if (otok->kind != stok->kind ||
					CompareString(otok->as_string, stok->as_string) != 0 ||
					otok->leading_spaces != stok->leading_spaces)
				{
					equal = false;
					break;
				}
				
				++otok;
				++stok;
			}
		}
		
		if (!equal)
			C_TraceErrorRd(ctx, rd, "incompatible redefinition of macro.");
		
		if (other_macro->param_count >= 0)
			Map_RemoveWithHash(pp->func_macros, macro->name, hash);
		else
			Map_RemoveWithHash(pp->obj_macros, macro->name, hash);
	}
	
	// NOTE(ljre): Add macro to map.
	if (is_func_like)
		Map_InsertWithHash(pp->func_macros, macro->name, macro, hash);
	else
		Map_InsertWithHash(pp->obj_macros, macro->name, macro, hash);
	
	return macro;
}

internal void
C_PredefineBuiltinMacros(C_Context* ctx, C_Preprocessor* pp)
{
	const char* defs[] = {
		"__LINE__",
		"__FILE__",
	};
	
	for (int32 i = 0; i < ArrayLength(defs); ++i)
	{
		C_TokenSlice slice = C_Tokenize(ctx, ctx->array_arena, defs[i], &(C_SourceTrace) { 0 });
		C_TokenReader rd = {
			.slice_head = slice.data,
			.slice_end = slice.data + slice.size,
		};
		
		C_NextToken(&rd);
		
		bool is_func_like;
		uint64 hash;
		C_Macro* macro = C_ParseMacroDefinition(ctx, pp, &rd, &hash, &is_func_like);
		
		Map_InsertWithHash(pp->predefined_macros, macro->name, macro, hash);
	}
}

internal void
C_UndefineMacro(C_Context* ctx, C_Preprocessor* pp, String name)
{
	uint64 hash = SimpleHash(name);
	
	if (!Map_RemoveWithHash(pp->obj_macros, name, hash))
		Map_RemoveWithHash(pp->func_macros, name, hash);
}

internal C_Macro*
C_FindMacro(C_Context* ctx, C_Preprocessor* pp, String name, bool obj_like_only)
{
	TraceName(name);
	
	uint64 hash = SimpleHash(name);
	C_Macro* result = Map_FetchWithHash(pp->predefined_macros, name, hash);
	
	if (result && result->param_count >= 0 && obj_like_only)
		result = NULL;
	if (!result && !obj_like_only)
		result = Map_FetchWithHash(pp->func_macros, name, hash);
	if (!result)
		result = Map_FetchWithHash(pp->obj_macros, name, hash);
	
	return result;
}

internal inline bool
C_WasExpandedByMacro(C_Macro* macro, C_SourceTrace* trace)
{
	for (; trace->invocation; trace = trace->invocation)
	{
		if (trace->macro_def == macro)
			return true;
	}
	
	return false;
}

struct C_MacroParameter
{
	String name;
	
	C_Token* tokens;
	C_Token* end;
}
typedef C_MacroParameter;

internal void
C_ExpandMacro(C_Context* ctx, C_Preprocessor* pp, C_Macro* macro, C_TokenReader* up_rd)
{
	TraceName(macro->name);
	C_SourceTrace* invocation_trace = Arena_Push(ctx->tree_arena, sizeof(*invocation_trace));
	*invocation_trace = up_rd->head->trace;
	invocation_trace->file = up_rd->file_trace;
	
	uint32 invocation_leading_spaces = up_rd->head->leading_spaces;
	C_TokenQueue* saved_up_rd_queue = up_rd->queue;
	
	if (MatchCString("__LINE__", macro->name))
	{
		C_Token tmp = {
			.kind = C_TokenKind_IntLiteral,
			.as_string = Arena_SPrintf(ctx->tree_arena, "%u", invocation_trace->line + 1),
			.trace = *invocation_trace,
		};
		
		C_PushTokenToQueue(ctx->tree_arena, &tmp, &up_rd->queue);
	}
	else if (MatchCString("__FILE__", macro->name))
	{
		C_Token tmp = {
			.kind = C_TokenKind_StringLiteral,
			.as_string = Arena_SPrintf(ctx->tree_arena, "\"%S\"", invocation_trace->file->path),
			.trace = *invocation_trace,
		};
		
		C_PushTokenToQueue(ctx->tree_arena, &tmp, &up_rd->queue);
	}
	else if (macro->param_count < 0)
	{
		//- NOTE(ljre): This is an object-like macro.
		C_Token* mhead = macro->tokens.data + 1; // NOTE(ljre): Ignore macro name
		C_Token* mend = macro->tokens.data + macro->tokens.size;
		
		C_TokenQueue** queue = &up_rd->queue;
		for (; mhead < mend; ++mhead)
		{
			C_Token tmp = *mhead;
			
			tmp.trace.file = macro->file;
			tmp.trace.invocation = invocation_trace;
			tmp.trace.macro_def = macro;
			
			queue = C_PushTokenToQueue(ctx->scratch_arena, &tmp, queue);
		}
	}
	else
	{
		//- NOTE(ljre): This is a function-like macro.
		C_MacroParameter* params = (macro->param_count > 0) ? Arena_Push(ctx->scratch_arena, macro->param_count * sizeof(*params)) : NULL;
		
		C_Token* mhead = macro->tokens.data;
		C_Token* mend = macro->tokens.data + macro->tokens.size;
		
		C_EatToken(ctx, up_rd, C_TokenKind_Identifier); // NOTE(ljre): Consume macro name
		C_EatToken(ctx, up_rd, C_TokenKind_LeftParen); // NOTE(ljre): Consume '('
		
		mhead += 2; // same as above
		
		//- NOTE(ljre): Read parameters
		for (int32 param_index = 0; param_index < macro->param_count; ++param_index)
		{
			Assert(mhead->kind != C_TokenKind_RightParen);
			bool is_last_arg = (param_index + 1 >= macro->param_count);
			
			if (up_rd->head->kind == C_TokenKind_RightParen)
			{
				C_TraceWarningRd(ctx, up_rd, C_Warning_MissingMacroArguments, "missing macro arguments.");
				
				// NOTE(ljre): Read at least the parameters name and leave them expanding to nothing.
				while (param_index < macro->param_count)
				{
					if (mhead->kind == C_TokenKind_VarArgs)
						params[param_index].name = Str("__VA_ARGS__");
					else
						params[param_index].name = mhead->as_string;
					
					mhead += 1 + (param_index+1 < macro->param_count);
					
					++param_index;
				}
				
				break;
			}
			
			if (mhead->kind == C_TokenKind_VarArgs)
			{
				params[param_index].name = Str("__VA_ARGS__");
				
				C_Token* arg = Arena_Push(ctx->scratch_arena, sizeof(*arg));
				C_Token* tok = arg;
				
				int32 nesting = 1;
				while (up_rd->head->kind)
				{
					if (up_rd->head->kind == C_TokenKind_RightParen)
					{
						if (--nesting <= 0)
							break;
					}
					else if (up_rd->head->kind == C_TokenKind_LeftParen)
						++nesting;
					
					*tok = *up_rd->head;
					tok = Arena_Push(ctx->scratch_arena, sizeof(*tok));
					
					C_NextToken(up_rd);
				}
				
				params[param_index].tokens = arg;
				params[param_index].end = tok;
			}
			else
			{
				Assert(mhead->kind == C_TokenKind_Identifier);
				
				params[param_index].name = mhead->as_string;
				
				C_Token* arg = Arena_Push(ctx->scratch_arena, sizeof(*arg));
				C_Token* tok = arg;
				
				int32 nesting = 1;
				while (up_rd->head->kind && (up_rd->head->kind != C_TokenKind_Comma || nesting > 1))
				{
					if (up_rd->head->kind == C_TokenKind_RightParen)
					{
						if (--nesting <= 0)
							break;
					}
					else if (up_rd->head->kind == C_TokenKind_LeftParen)
						++nesting;
					
					*tok = *up_rd->head;
					tok = Arena_Push(ctx->scratch_arena, sizeof(*tok));
					
					C_NextToken(up_rd);
				}
				
				params[param_index].tokens = arg;
				params[param_index].end = tok;
			}
			
			mhead += 1 + !is_last_arg;
			
			if (!is_last_arg)
				C_TryToEatToken(up_rd, C_TokenKind_Comma);
		}
		
		//- NOTE(ljre): Check for end of arguments list.
		if (up_rd->head->kind == C_TokenKind_Comma)
		{
			C_TraceWarningRd(ctx, up_rd, C_Warning_ExcessiveMacroArguments, "excessive macro arguments (expected only %i).", macro->param_count);
			
			int32 nesting = 1;
			while (up_rd->head->kind)
			{
				if (up_rd->head->kind == C_TokenKind_RightParen)
				{
					if (--nesting <= 0)
						break;
				}
				else if (up_rd->head->kind == C_TokenKind_LeftParen)
					++nesting;
				
				C_NextToken(up_rd);
			}
		}
		
		++mhead; // NOTE(ljre): Eat ')'
		
		if (up_rd->head->kind != C_TokenKind_RightParen)
		{
			C_TraceErrorRd(ctx, up_rd, "expected ')' after arguments for macro.");
		}
		else
		{
			// NOTE(ljre): We *DO NOT* read the next token yet, because we still have to queue the tokens to
			//             be expanded. So we read the next token at the end of this procedure.
			
			//C_NextToken(up_rd);
		}
		
		//- NOTE(ljre): Expand tokens.
		C_TokenReader rd = {
			.queue = NULL,
			.slice_head = mhead,
			.slice_end = mend,
			.file_trace = mhead[-1].trace.file,
		};
		
		C_NextToken(&rd);
		
		C_TokenQueue** parent_queue = &up_rd->queue;
		
		while (rd.head)
		{
			switch (rd.head->kind)
			{
				case C_TokenKind_Hashtag:
				{
					uint32 leading_spaces = rd.head->leading_spaces;
					
					C_NextToken(&rd);
					if (!rd.head || rd.head->kind != C_TokenKind_Identifier)
						C_TraceErrorRd(ctx, &rd, "expected macro parameter for stringification.");
					
					C_MacroParameter* param = NULL;
					
					for (int32 i = 0; i < macro->param_count; ++i)
					{
						if (CompareString(rd.head->as_string, params[i].name) == 0)
						{
							param = &params[i];
							break;
						}
					}
					
					if (!param)
						C_TraceErrorRd(ctx, &rd, "'%S' is not a macro paremeter.", rd.head->as_string);
					else
					{
						C_Token token = {
							.kind = C_TokenKind_StringLiteral,
							.leading_spaces = leading_spaces,
							.trace = rd.head->trace,
						};
						
						{
							char* buf = Arena_PushMemory(ctx->tree_arena, 1, "\"");
							C_StringifyTokensToArena(ctx->tree_arena, param->tokens, param->end);
							Arena_PushMemory(ctx->tree_arena, 1, "\"");
							
							token.as_string = StrMake(buf, (char*)Arena_End(ctx->tree_arena) - buf);
						}
						
						if (param->tokens < param->end && param->tokens[0].trace.invocation)
						{
							C_SourceTrace* trace = Arena_Push(ctx->tree_arena, sizeof(*trace));
							*trace = token.trace;
							
							trace->file = param->tokens[0].trace.file;
							trace->invocation = param->tokens[0].trace.invocation;
							trace->macro_def = param->tokens[0].trace.macro_def;
							
							token.trace.file = macro->file;
							token.trace.invocation = trace;
							token.trace.macro_def = macro;
						}
						else
						{
							token.trace.file = macro->file;
							token.trace.invocation = invocation_trace;
							token.trace.macro_def = macro;
						}
						
						parent_queue = C_PushTokenToQueue(ctx->scratch_arena, &token, parent_queue);
						C_NextToken(&rd);
					}
				} break;
				
				case C_TokenKind_Identifier:
				{
					// NOTE(ljre): Was the 'head' added by a parameter in the queue or is it from the macro we're expanding?
					bool from_parameter = (rd.head->trace.macro_def == macro);
					
					if (!from_parameter)
					{
						C_MacroParameter* param = NULL;
						
						for (int32 i = 0; i < macro->param_count; ++i)
						{
							if (CompareString(rd.head->as_string, params[i].name) == 0)
							{
								param = &params[i];
								break;
							}
						}
						
						if (param)
						{
							C_PushTokenFromMacroToQueue(ctx, param->tokens, param->end, &rd.queue, invocation_trace, macro);
							C_NextToken(&rd);
							break;
						}
					}
					
					/* fallthrough */
					default:;
					
					C_Token* tok = rd.head;
					C_Token* incoming = C_PeekToken(&rd);
					
					if (incoming->kind == C_TokenKind_DoubleHashtag)
					{
						// TODO(ljre): Tokens being generated from ## concatenation don't seem to have
						//             correct invocation trace. Feeling like this come from parameter
						//             expansion though.
						
						C_NextToken(&rd); // Eat left side
						C_SourceTrace hh_trace = rd.head->trace;
						C_NextToken(&rd); // Eat ##
						
						if (!rd.head)
						{
							// TODO(ljre): handle case where ## is followed by no token :clown:
						}
						
						if (rd.head->kind == C_TokenKind_Identifier)
						{
							C_MacroParameter* param = NULL;
							
							for (int32 i = 0; i < macro->param_count; ++i)
							{
								if (CompareString(rd.head->as_string, params[i].name) == 0)
								{
									param = &params[i];
									break;
								}
							}
							
							if (param)
							{
								C_PushTokenFromMacroToQueue(ctx, param->tokens, param->end, &rd.queue, invocation_trace, macro);
								C_NextToken(&rd);
							}
						}
						
						C_Token* other_tok = rd.head;
						
						// NOTE(ljre): This is a dumb way :kekw
						uintsize total_size = tok->as_string.size + other_tok->as_string.size + 1;
						char* buf = Arena_PushDirtyAligned(ctx->tree_arena, total_size, 1);
						
						MemCopy(buf,                       tok->as_string.data, tok->as_string.size);
						MemCopy(buf + tok->as_string.size, other_tok->as_string.data, other_tok->as_string.size);
						buf[total_size-1] = 0;
						
						C_SourceTrace* inv_trace = Arena_Push(ctx->tree_arena, sizeof(*inv_trace));
						*inv_trace = tok->trace;
						
						hh_trace.file = macro->file;
						hh_trace.invocation = inv_trace;
						hh_trace.macro_def = macro;
						
						C_TokenSlice slice = C_Tokenize(ctx, ctx->scratch_arena, buf, &hh_trace);
						
						parent_queue = C_PushTokenFromMacroToQueue(ctx, slice.data, slice.data + slice.size, parent_queue, invocation_trace, macro);
						
						C_NextToken(&rd);
						
						if (slice.size != 1)
						{
							// TODO(ljre): Error out?
						}
					}
					else
					{
						if (from_parameter && tok->kind == C_TokenKind_Identifier)
						{
							C_Macro* to_expand = C_FindMacro(ctx, pp, tok->as_string, incoming->kind != C_TokenKind_LeftParen);
							
							if (to_expand && to_expand != macro && !C_WasExpandedByMacro(macro, tok->trace.invocation))
							{
								C_ExpandMacro(ctx, pp, to_expand, &rd);
								break;
							}
						}
						
						C_NextToken(&rd);
						
						if (tok->kind != C_TokenKind_NewLine)
						{
							C_Token tmp = *tok;
							
							tmp.trace.file = macro->file;
							tmp.trace.invocation = invocation_trace;
							tmp.trace.macro_def = macro;
							
							parent_queue = C_PushTokenToQueue(ctx->scratch_arena, tok, parent_queue);
						}
					}
				} break;
			}
		}
		
		// NOTE(ljre): End of function-like macro :)
	}
	
	if (up_rd->queue && saved_up_rd_queue != up_rd->queue)
		up_rd->queue->token.leading_spaces = invocation_leading_spaces;
	
	// NOTE(ljre): Done!
	C_NextToken(up_rd);
}

internal bool32
C_TryToExpandMacro(C_Context* ctx, C_Preprocessor* pp, C_TokenReader* up_rd)
{
	Assert(up_rd->head->kind == C_TokenKind_Identifier);
	
	String name = up_rd->head->as_string;
	C_Macro* macro;
	
	macro = C_FindMacro(ctx, pp, name, (C_PeekToken(up_rd)->kind != C_TokenKind_LeftParen));
	
	if (macro)
	{
		// NOTE(ljre): Check if this token was expanded from this macro.
		C_SourceTrace* trace = &up_rd->head->trace;
		bool should_expand = true;
		
		while (trace->invocation)
		{
			if (trace->macro_def == macro)
			{
				should_expand = false;
				break;
			}
			
			trace = trace->invocation;
		}
		
		if (should_expand)
		{
			C_ExpandMacro(ctx, pp, macro, up_rd);
			return true;
		}
	}
	
	return false;
}

//~ NOTE(ljre): Evaluate constant expression.
internal int32 C_EvalPreprocessorExprBinary(C_Context* ctx, C_Preprocessor* pp, C_TokenReader* rd, int32 level);

internal int32
C_EvalPreprocessorExprFactor(C_Context* ctx, C_Preprocessor* pp, C_TokenReader* rd)
{
	beginning:;
	int32 result = 0;
	
	switch (rd->head->kind)
	{
		// NOTE(ljre): Unary operators
		case C_TokenKind_Minus:
		{
			C_NextToken(rd);
			return -C_EvalPreprocessorExprFactor(ctx, pp, rd);
		} break;
		
		case C_TokenKind_Plus:
		{
			C_NextToken(rd);
			goto beginning;
		} break;
		
		case C_TokenKind_Not:
		{
			C_NextToken(rd);
			return ~C_EvalPreprocessorExprFactor(ctx, pp, rd);
		} break;
		
		case C_TokenKind_LNot:
		{
			C_NextToken(rd);
			return !C_EvalPreprocessorExprFactor(ctx, pp, rd);
		} break;
		
		// NOTE(ljre): Factors
		case C_TokenKind_IntLiteral:
		case C_TokenKind_LIntLiteral:
		case C_TokenKind_LLIntLiteral:
		case C_TokenKind_UintLiteral:
		case C_TokenKind_LUintLiteral:
		case C_TokenKind_LLUintLiteral:
		{
			result = (int32)C_TokenizeIntLiteral(ctx, rd->head->as_string);
			
			C_NextToken(rd);
		} break;
		
		case C_TokenKind_DoubleLiteral:
		case C_TokenKind_FloatLiteral:
		{
			result = (int32)C_TokenizeDoubleLiteral(ctx, rd->head->as_string);
			
			C_NextToken(rd);
		} break;
		
		case C_TokenKind_CharLiteral:
		{
			result = C_TokenizeCharLiteral(ctx, rd->head->as_string);
			
			C_NextToken(rd);
		} break;
		
		case C_TokenKind_WideStringLiteral:
		case C_TokenKind_StringLiteral:
		{
			// TODO(ljre): What should happen if it's a string literal?
			result = (rd->head->as_string.size > 2 + (rd->head->kind == C_TokenKind_WideStringLiteral));
			
			C_NextToken(rd);
		} break;
		
		case C_TokenKind_Identifier:
		{
			String name = rd->head->as_string;
			
			if (MatchCString("defined", name))
			{
				C_NextToken(rd);
				bool32 has_paren = C_TryToEatToken(rd, C_TokenKind_LeftParen);
				
				if (C_AssertToken(ctx, rd, C_TokenKind_Identifier))
				{
					result = (C_FindMacro(ctx, pp, rd->head->as_string, false) != NULL);
					C_NextToken(rd);
				}
				
				if (has_paren)
					C_EatToken(ctx, rd, C_TokenKind_RightParen);
			}
			else
			{
				bool is_func = (C_PeekToken(rd)->kind == C_TokenKind_LeftParen);
				C_Macro* macro = C_FindMacro(ctx, pp, name, !is_func);
				
				if (macro)
				{
					C_ExpandMacro(ctx, pp, macro, rd);
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
	// TODO(ljre): Error checking for invalid operations. (0 div, etc.)
	
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

// NOTE(ljre): Operator Precedence Parser (https://en.wikipedia.org/wiki/Operator-precedence_parser)
internal int32
C_EvalPreprocessorExpr(C_Context* ctx, C_Preprocessor* pp, C_TokenReader* rd, int32 level)
{
	int32 result = C_EvalPreprocessorExprFactor(ctx, pp, rd);
	
	C_OperatorPrecedence prec;
	while (prec = C_operators_precedence[rd->head->kind],
		   prec.level > level)
	{
		C_TokenKind op = rd->head->kind;
		C_NextToken(rd);
		int32 right = C_EvalPreprocessorExprFactor(ctx, pp, rd);
		
		C_TokenKind lookahead = rd->head->kind;
		C_OperatorPrecedence lookahead_prec = C_operators_precedence[lookahead];
		
		while (lookahead_prec.level > 0 &&
			   (lookahead_prec.level > prec.level ||
				(lookahead_prec.level == prec.level && lookahead_prec.right2left))) {
			C_NextToken(rd);
			
			if (lookahead == C_TokenKind_QuestionMark) {
				int32 if_true = C_EvalPreprocessorExpr(ctx, pp, rd, 0);
				
				if (rd->head->kind != C_TokenKind_Colon)
					C_TraceErrorRd(ctx, rd, "expected ':' for ternary conditional operator, but got '%S'.", StrFmt(rd->head->as_string));
				
				C_NextToken(rd);
				
				int32 if_false = C_EvalPreprocessorExpr(ctx, pp, rd, level - 1);
				
				right = right ? if_true : if_false;
			} else {
				int32 other = C_EvalPreprocessorExpr(ctx, pp, rd, level + 1);
				
				right = C_EvalPreprocessorExprOp(lookahead, right, other);
			}
			
			lookahead = rd->head->kind;
			lookahead_prec = C_operators_precedence[lookahead];
		}
		
		result = C_EvalPreprocessorExprOp(op, result, right);
	}
	
	return result;
}

//~ NOTE(ljre): File Preprocessing & Directives
internal void C_PreprocessFile(C_Context* ctx, C_Preprocessor* pp, C_TokenSlice input, C_SourceFileTrace* file_trace);

internal void
C_PreprocessInclude(C_Context* ctx, C_Preprocessor* pp, C_TokenReader* rd, C_SourceFileTrace* including_from)
{
	C_PpLoadedFile* file = NULL;
	String path;
	
	if (rd->head->kind == C_TokenKind_LThan)
	{
		C_Token* tokens = Arena_Push(ctx->scratch_arena, sizeof(*tokens));
		C_Token* tok = tokens;
		
		C_NextToken(rd);
		while (rd->head && rd->head->kind != C_TokenKind_GThan)
		{
			if (rd->head->kind == C_TokenKind_NewLine)
			{
				C_TraceErrorRd(ctx, rd, "expected closing '>' when reading path for #include'ing.");
				break;
			}
			
			*tok = *rd->head;
			tok = Arena_Push(ctx->scratch_arena, sizeof(*tok));
			
			C_NextToken(rd);
		}
		
		path = C_StringifyTokensToArena(ctx->scratch_arena, tokens, tok);
		file = C_TryToLoadFile(ctx, pp, path, false, including_from->path);
	}
	else if (rd->head->kind == C_TokenKind_StringLiteral && C_PeekToken(rd)->kind == C_TokenKind_NewLine)
	{
		path = SliceString(rd->head->as_string, 1, rd->head->as_string.size - 2);
		file = C_TryToLoadFile(ctx, pp, path, true, including_from->path);
		
		C_NextToken(rd);
	}
	else
	{
		// NOTE(ljre): SLOW PATH!!
		//             We have to expand macros and then read them as string.
		
		// TODO
		Unreachable();
	}
	
	if (!file)
	{
		C_TraceErrorRd(ctx, rd, "could not find file '%S' when including.", path);
	}
	else if (!file->pragma_onced)
	{
		C_SourceFileTrace* file_trace = Arena_Push(ctx->tree_arena, sizeof(*file_trace));
		
		file_trace->path = file->fullpath;
		file_trace->included_from = including_from;
		file_trace->included_line = rd->head->trace.line;
		file_trace->is_system_file = file->is_system_file;
		
		if (!file->tokens.data)
			file->tokens = C_Tokenize(ctx, ctx->tree_arena, file->contents, &(C_SourceTrace) {
										  .file = file_trace,
									  });
		
		C_PreprocessFile(ctx, pp, file->tokens, file_trace);
	}
}

internal void C_IgnoreUntilEndIf(C_Context* ctx, C_Preprocessor* pp, C_TokenReader* rd, bool already_matched);

internal void
C_PreprocessIfDef(C_Context* ctx, C_Preprocessor* pp, C_TokenReader* rd, bool not)
{
	if (!C_AssertToken(ctx, rd, C_TokenKind_Identifier))
		return;
	
	String name = rd->head->as_string;
	while (rd->head->kind && rd->head->kind != C_TokenKind_NewLine)
		C_NextToken(rd);
	
	bool result = not ^ (C_FindMacro(ctx, pp, name, false) != NULL);
	if (!result)
		C_IgnoreUntilEndIf(ctx, pp, rd, false);
}

internal void
C_PreprocessIf(C_Context* ctx, C_Preprocessor* pp, C_TokenReader* rd)
{
	if (rd->head->kind == C_TokenKind_NewLine)
	{
		C_TraceErrorRd(ctx, rd, "expected constant expression for condition.");
		return;
	}
	
	int32 result = C_EvalPreprocessorExpr(ctx, pp, rd, 0);
	if (result == 0)
		C_IgnoreUntilEndIf(ctx, pp, rd, false);
}

internal void
C_IgnoreUntilEndIf(C_Context* ctx, C_Preprocessor* pp, C_TokenReader* rd, bool already_matched)
{
	int32 nesting = 1;
	
	while (rd->head->kind && nesting > 0)
	{
		switch (rd->head->kind)
		{
			case C_TokenKind_Eof:
			{
				C_TraceErrorRd(ctx, rd, "unclosed conditional pre-processor block.");
				goto out_of_the_loop;
			} break;
			
			case C_TokenKind_Hashtag:
			{
				C_NextToken(rd);
				
				if (rd->head->kind == C_TokenKind_Identifier)
				{
					String directive = rd->head->as_string;
					bool not = false;
					
					if (!already_matched && nesting == 1)
					{
						if (MatchCString("elifdef", directive) ||
							(not = MatchCString("elifndef", directive)))
						{
							C_NextToken(rd);
							C_PreprocessIfDef(ctx, pp, rd, not);
							
							goto out_of_the_loop;
						}
						else if (MatchCString("elif", directive))
						{
							C_NextToken(rd);
							C_PreprocessIf(ctx, pp, rd);
							
							goto out_of_the_loop;
						}
						else if (MatchCString("else", directive))
						{
							C_NextToken(rd);
							
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
							while (rd->head->kind && rd->head->kind != C_TokenKind_NewLine)
								C_NextToken(rd);
							break;
						}
					}
				}
			} /* fallthrough */
			
			default:
			{
				while (rd->head->kind && rd->head->kind != C_TokenKind_NewLine)
					C_NextToken(rd);
				if (rd->head->kind)
					C_NextToken(rd);
			} break;
		}
	}
	
	out_of_the_loop:;
}

internal void
C_PreprocessFile(C_Context* ctx, C_Preprocessor* pp, C_TokenSlice input, C_SourceFileTrace* file_trace)
{
	TraceName(file_trace->path);
	
	C_TokenReader rd = {
		.queue = NULL,
		.slice_head = input.data,
		.slice_end = input.data + input.size,
		.file_trace = file_trace,
	};
	
	C_NextToken(&rd);
	
	bool previous_was_newline = true;
	while (rd.head)
	{
		Assert(rd.head);
		
		if (rd.head->kind == C_TokenKind_NewLine)
		{
			Assert(rd.queue == NULL);
			Arena_Clear(ctx->scratch_arena);
			
			previous_was_newline = true;
			
			do
				C_NextToken(&rd);
			while (rd.head && rd.head->kind == C_TokenKind_NewLine);
			
			if (!rd.head)
				break;
		}
		
		if (rd.head->kind == C_TokenKind_Hashtag)
		{
			if (!previous_was_newline)
				C_TraceErrorRd(ctx, &rd, "'#' should be the first token in a line.");
			else
			{
				C_NextToken(&rd);
				
				if (rd.head && rd.head->kind == C_TokenKind_Identifier)
				{
					String ident = rd.head->as_string;
					bool not = false;
					
					if (MatchCString("define", ident))
					{
						C_NextToken(&rd);
						C_DefineMacro(ctx, pp, &rd);
					}
					else if (MatchCString("include", ident))
					{
						C_NextToken(&rd);
						C_PreprocessInclude(ctx, pp, &rd, file_trace);
					}
					else if (MatchCString("undef", ident))
					{
						C_NextToken(&rd);
						
						if (C_AssertToken(ctx, &rd, C_TokenKind_Identifier))
							C_UndefineMacro(ctx, pp, rd.head->as_string);
					}
					else if (MatchCString("ifdef", ident) || (not = MatchCString("ifndef", ident)))
					{
						C_NextToken(&rd);
						C_PreprocessIfDef(ctx, pp, &rd, not);
					}
					else if (MatchCString("if", ident))
					{
						C_NextToken(&rd);
						C_PreprocessIf(ctx, pp, &rd);
					}
					else if (MatchCString("elif", ident) || MatchCString("elifdef", ident) ||
							 MatchCString("elifndef", ident) || MatchCString("else", ident))
					{
						C_NextToken(&rd);
						C_IgnoreUntilEndIf(ctx, pp, &rd, true);
					}
					else if (MatchCString("endif", ident))
					{
						C_NextToken(&rd);
						// TODO(ljre): This should raise an error. Check for if/endif balance.
					}
					else if (MatchCString("warning", ident))
					{
						
					}
					else if (MatchCString("error", ident))
					{
						
					}
					else if (MatchCString("pragma", ident))
					{
						C_NextToken(&rd);
						
						if (rd.head && MatchCString("once", rd.head->as_string) && file_trace->included_from)
							C_PragmaOnceFile(ctx, pp, file_trace->path);
					}
					else
					{
						C_TraceErrorRd(ctx, &rd, "unknown preprocessor directive '%S'.", StrFmt(ident));
					}
				}
			}
			
			while (rd.head && rd.head->kind != C_TokenKind_NewLine)
				C_NextToken(&rd);
			
			continue;
		}
		
		while (rd.head && rd.head->kind != C_TokenKind_Hashtag && rd.head->kind != C_TokenKind_NewLine)
		{
			previous_was_newline = false;
			
			if (rd.head->kind != C_TokenKind_Identifier || !C_TryToExpandMacro(ctx, pp, &rd))
			{
				C_PreprocessWriteToken(ctx, pp, rd.head, file_trace);
				C_NextToken(&rd);
			}
		}
	}
}

//~ NOTE(ljre): Main function API.
internal C_TokenSlice
C_Preprocess(C_Context* ctx, String file)
{
	TraceName(file);
	
	//- NOTE(ljre): Read & Tokenize translation-unit.
	C_SourceFileTrace* file_trace = Arena_Push(ctx->tree_arena, sizeof(*file_trace));
	file_trace->path = file;
	
	const char* source = Arena_End(ctx->tree_arena);
	OS_ReadWholeFile(file, ctx->tree_arena);
	C_TokenSlice tokens = C_Tokenize(ctx, ctx->array_arena, source, &(C_SourceTrace) { .file = file_trace });
	
	//- NOTE(ljre): Preprocess it.
	C_Preprocessor pp = {
		.func_macros = Map_Create(ctx->tree_arena, 4096),
		.obj_macros = Map_Create(ctx->tree_arena, 4096),
		.loaded_files = Map_Create(ctx->tree_arena, 256),
		.predefined_macros = Map_Create(ctx->tree_arena, 256),
	};
	
	C_PredefineBuiltinMacros(ctx, &pp);
	
	for (int32 i = 0; i < ctx->options->predefined_macros_count; ++i)
	{
		const char* cstr = ctx->options->predefined_macros[i];
		
		C_TokenSlice slice = C_Tokenize(ctx, ctx->array_arena, cstr, &(C_SourceTrace) { 0 });
		C_TokenReader rd = {
			.slice_head = slice.data,
			.slice_end = slice.data + slice.size,
		};
		
		C_NextToken(&rd);
		
		uint64 hash;
		bool is_func_like;
		C_Macro* macro = C_ParseMacroDefinition(ctx, &pp, &rd, &hash, &is_func_like);
		
		Map_InsertWithHash(pp.predefined_macros, macro->name, macro, hash);
	}
	
	C_Token* result = Arena_Push(ctx->array_arena, sizeof(*result));
	pp.out = result;
	
	C_PreprocessFile(ctx, &pp, tokens, file_trace);
	C_PreprocessWriteToken(ctx, &pp, &(C_Token) { 0 }, NULL);
	
	Arena_Clear(ctx->scratch_arena);
	return (C_TokenSlice) { .size = pp.out - result, .data = result };
}

internal String
C_PrintTokensGnuStyle(C_Context* ctx, Arena* arena, C_TokenSlice tokens)
{
	char* result = Arena_End(arena);
	
	{
		C_Token* head = tokens.data;
		C_Token* end = tokens.data + tokens.size;
		
		if (tokens.size > 0 && end[-1].kind == C_TokenKind_Eof)
			--end;
		
		// NOTE(ljre): Print at least one token up-front.
		if (head->leading_spaces)
		{
			char* buf = Arena_PushDirtyAligned(arena, head->leading_spaces, 1);
			MemSet(buf, ' ', head->leading_spaces);
		}
		
		Arena_Printf(arena, "# %u \"%S\"\n", head->trace.line + 1, StrFmt(head->trace.file->path));
		Arena_PushMemory(arena, head->as_string.size, head->as_string.data);
		
		// NOTE(ljre): Now 'previous' is always a valid token. No need to check for NULL.
		C_Token* previous = head++;
		
		for (; head < end; previous = head++)
		{
			C_SourceTrace* head_trace = &head->trace;
			C_SourceTrace* prev_trace = &previous->trace;
			
			while (head_trace->invocation)
				head_trace = head_trace->invocation;
			
			while (prev_trace->invocation)
				prev_trace = prev_trace->invocation;
			
			if (head_trace->file == prev_trace->file)
			{
				int32 line_diff = head_trace->line - prev_trace->line;
				
				if (line_diff > 4)
				{
					Arena_Printf(arena, "\n\n# %u \"%S\"\n", head_trace->line + 1, StrFmt(head_trace->file->path));
				}
				else if (line_diff > 0)
				{
					char* buf = Arena_PushDirtyAligned(arena, line_diff, 1);
					MemSet(buf, '\n', line_diff);
				}
			}
			else
			{
				Arena_Printf(arena, "\n\n# %u \"%S\"\n", head_trace->line + 1, StrFmt(head_trace->file->path));
			}
			
			if (head->leading_spaces)
			{
				char* buf = Arena_PushDirtyAligned(arena, head->leading_spaces, 1);
				MemSet(buf, ' ', head->leading_spaces);
			}
			Arena_PushMemory(arena, head->as_string.size, head->as_string.data);
		}
	}
	
	uintsize length = (char*)Arena_End(arena) - result;
	return StrMake(result, length);
}
