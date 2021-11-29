internal void LangC_Preprocess2(LangC_Context* ctx, String path, const char* source, LangC_Lexer* from);
internal void LangC_PreprocessIfDef(LangC_Context* ctx, LangC_Lexer* lex, bool32 not);
internal void LangC_PreprocessIf(LangC_Context* ctx, LangC_Lexer* lex);

internal LangC_PPLoadedFile*
LangC_LoadFileFromDisk(LangC_Context* ctx, const char path[MAX_PATH_SIZE], uint64 calculated_hash, bool32 relative)
{
	const char* contents = OS_ReadWholeFile(path, NULL);
	LangC_Preprocessor* const pp = &ctx->pp;
	
	if (contents)
	{
		LangC_PPLoadedFile* file;
		
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
		
		uintsize len = strlen(path) + 1;
		char* mem = Arena_Push(ctx->stage_arena, len);
		OurMemCopy(mem, path, len);
		
		file->path = StrMake(mem, len);
		return file;
	}
	
	return NULL;
}

internal const char*
LangC_TryToLoadFile(LangC_Context* ctx, String path, bool32 relative, String including_from, String* out_fullpath)
{
	// TODO(ljre): Trim the working directory from 'out_fullpath' when needed.
	char fullpath[MAX_PATH_SIZE];
	LangC_Preprocessor* const pp = &ctx->pp;
	
	if (relative)
	{
		LangC_PPLoadedFile* file = pp->loaded_files;
		
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
		
		OS_ResolveFullPath(StrMake(fullpath, len), fullpath);
		uint64 search_hash = SimpleHashNullTerminated(fullpath);
		
		while (file)
		{
			if (file->hash == search_hash && file->relative)
			{
				if (file->pragma_onced)
					return NULL;
				
				*out_fullpath = file->path;
				return file->contents;
			}
			
			file = file->next;
		}
		
		file = LangC_LoadFileFromDisk(ctx, fullpath, search_hash, true);
		if (file)
		{
			*out_fullpath = file->path;
			return file->contents;
		}
	}
	
	for (int32 i = ctx->options->include_dirs_count - 1; i >= 0 ; --i)
	{
		String include_dir = ctx->options->include_dirs[i];
		include_dir = IgnoreNullTerminator(include_dir);
		
		OurMemCopy(fullpath, include_dir.data, include_dir.size);
		OurMemCopy(fullpath + include_dir.size, path.data, path.size);
		
		String p = {
			.size = include_dir.size + path.size,
			.data = fullpath,
		};
		
		OS_ResolveFullPath(p, fullpath);
		
		LangC_PPLoadedFile* file = pp->loaded_files;
		uint64 search_hash = SimpleHashNullTerminated(fullpath);
		
		while (file)
		{
			if (file->hash == search_hash && !file->relative)
			{
				if (file->pragma_onced)
					return NULL;
				
				*out_fullpath = file->path;
				return file->contents;
			}
			
			file = file->next;
		}
		
		file = LangC_LoadFileFromDisk(ctx, fullpath, search_hash, true);
		if (file)
		{
			*out_fullpath = file->path;
			return file->contents;
		}
	}
	
	return NULL;
}

internal void
LangC_PragmaOncePPFile(LangC_Context* ctx, String fullpath)
{
	LangC_Preprocessor* const pp = &ctx->pp;
	LangC_PPLoadedFile* file = pp->loaded_files;
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
LangC_IgnoreUntilNewline(LangC_Lexer* lex)
{
	//while (lex->token.kind && lex->token.kind != LangC_TokenKind_NewLine)
	//LangC_NextToken(lex);
	
	if (lex->token.kind && lex->token.kind != LangC_TokenKind_NewLine)
	{
		while (lex->head[0] && (lex->head[0] != '\n' || lex->head[-1] == '\\'))
			++lex->head;
		
		LangC_NextToken(lex);
	}
}

internal LangC_Macro*
LangC_DefineMacro(LangC_Context* ctx, String definition)
{
	LangC_Preprocessor* const pp = &ctx->pp;
	const char* def = Arena_NullTerminateString(ctx->stage_arena, definition);
	
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
			pp->first_macro = Arena_Push(ctx->stage_arena, sizeof *pp->first_macro);
		
		pp->last_macro = pp->first_macro;
	}
	else
	{
		if (!pp->last_macro->next)
			pp->last_macro->next = Arena_Push(ctx->stage_arena, sizeof *pp->last_macro->next);
		
		pp->last_macro->next->previous = pp->last_macro;
		pp->last_macro = pp->last_macro->next;
	}
	
	pp->last_macro->def = def;
	pp->last_macro->name = name;
	pp->last_macro->is_func_like = is_func_like;
	
	return pp->last_macro;
}

internal void
LangC_UndefineMacro(LangC_Context* ctx, String name)
{
	LangC_Preprocessor* const pp = &ctx->pp;
	
	if (!pp->first_macro)
		return;
	
	if (CompareString(pp->first_macro->name, name) == 0)
	{
		if (!pp->first_macro->persistent)
			pp->first_macro = pp->first_macro->next;
		
		return;
	}
	
	LangC_Macro* current = pp->first_macro;
	while (current->next && current->next != pp->last_macro)
	{
		if (CompareString(current->name, name) == 0)
		{
			if (!current->persistent)
				current->next = current->next->next;
			
			break;
		}
		
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
LangC_FindMacro(LangC_Context* ctx, String name, int32 type)
{
	Assert(type >= 0 && type <= 3);
	LangC_Preprocessor* pp = &ctx->pp;
	
	if (type == 3)
	{
		LangC_Macro* result = LangC_FindMacro(ctx, name, 1);
		
		if (!result)
			result = LangC_FindMacro(ctx, name, 0);
		
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

internal LangC_MacroParameter*
LangC_FindMacroParameter(LangC_MacroParameter* param_array, int32 param_count, String name)
{
	for (int32 i = 0; i < param_count; ++i)
	{
		if (CompareString(param_array[i].name, name) == 0)
			return &param_array[i];
	}
	
	return NULL;
}

internal void
LangC_TracePreprocessor(LangC_Context* ctx, LangC_Lexer* lex, uint32 flags)
{
	Assert(flags < 16);
	
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
		Arena_Printf(ctx->persistent_arena, "# %i \"%S\" %s\n", lex->line, StrFmt(lex->file->path), flag_table[flags]);
	else
		Arena_Printf(ctx->persistent_arena, "# %i \"%S\"\n", lex->line, StrFmt(lex->file->path));
}

internal void
LangC_ExpandMacro(LangC_Context* ctx, LangC_Macro* macro, LangC_Lexer* parent_lex, String leading_spaces)
{
	TraceName(macro->name);
	
	// NOTE(ljre): Handle special macros
	if (MatchCString("__LINE__", macro->name.data, macro->name.size))
	{
		int32 line = parent_lex->line;
		char* mem = Arena_End(ctx->stage_arena);
		uintsize needed = Arena_Printf(ctx->stage_arena, "%i", line);
		
		LangC_Token tok = {
			.kind = LangC_TokenKind_IntLiteral,
			.as_string = StrMake(mem, needed),
			.leading_spaces = StrNull,
			.value_int = line,
		};
		
		LangC_PushTokenToFront(parent_lex, &tok);
		LangC_NextToken(parent_lex);
		return;
	}
	else if (MatchCString("__FILE__", macro->name.data, macro->name.size))
	{
		String file = parent_lex->file->path;
		char* mem = Arena_End(ctx->stage_arena);
		uintsize needed = Arena_Printf(ctx->stage_arena, "\"%S\"", StrFmt(file));
		
		LangC_Token tok = {
			.kind = LangC_TokenKind_StringLiteral,
			.as_string = StrMake(mem, needed),
			.leading_spaces = StrNull,
			.value_str = file,
		};
		
		LangC_PushTokenToFront(parent_lex, &tok);
		LangC_NextToken(parent_lex);
		return;
	}
	
	// NOTE(ljre): Normal macro expansion.
	macro->expanding = true;
	const char* def_head = macro->def;
	
	LangC_MacroParameter params[128];
	int32 param_count = 0;
	
	// NOTE(ljre): Ignore macro name
	//while (LangC_IsIdentChar(*def_head))
	//++def_head;
	def_head += macro->name.size;
	
	// NOTE(ljre): Define parameters as macros
	if (macro->is_func_like)
	{
		LangC_NextToken(parent_lex); // eat macro name
		LangC_EatToken(parent_lex, LangC_TokenKind_LeftParen);
		
		while (*def_head && *def_head++ != '(');
		
		if (*def_head != ')')
		{
			do
			{
				LangC_IgnoreWhitespaces(&def_head, false);
				LangC_MacroParameter* param = &params[param_count++];
				Assert(param_count <= ArrayLength(params));
				
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
						if (parent_lex->token.kind == LangC_TokenKind_RightParen)
						{
							if (--nesting <= 0)
								break;
						}
						else if (parent_lex->token.kind == LangC_TokenKind_LeftParen)
							++nesting;
						
						Arena_PushMemory(ctx->stage_arena, StrFmt(parent_lex->token.as_string));
						Arena_PushMemory(ctx->stage_arena, StrFmt(parent_lex->token.leading_spaces));
						
						LangC_NextToken(parent_lex);
					}
				}
				else
				{
					const char* name_begin = def_head;
					
					while (LangC_IsIdentChar(*def_head))
						++def_head;
					
					name = StrMake(name_begin, (uintsize)(def_head - name_begin));
					
					int32 nesting = 1;
					while (parent_lex->token.kind && (parent_lex->token.kind != LangC_TokenKind_Comma || nesting > 1))
					{
						if (parent_lex->token.kind == LangC_TokenKind_RightParen)
						{
							if (--nesting <= 0)
								break;
						}
						else if (parent_lex->token.kind == LangC_TokenKind_LeftParen)
							++nesting;
						
						Arena_PushMemory(ctx->stage_arena, StrFmt(parent_lex->token.as_string));
						Arena_PushMemory(ctx->stage_arena, StrFmt(parent_lex->token.leading_spaces));
						
						LangC_NextToken(parent_lex);
					}
				}
				
				Arena_PushMemory(ctx->stage_arena, 1, "");
				
				param->name = name;
				param->expands_to = buf;
				
				LangC_IgnoreWhitespaces(&def_head, false);
			}
			while (def_head[0] == ',' && ++def_head && LangC_EatToken(parent_lex, LangC_TokenKind_Comma));
		}
		
		while (parent_lex->token.kind && parent_lex->token.kind != LangC_TokenKind_RightParen)
		{
			LangC_NextToken(parent_lex);
		}
		
		LangC_AssertToken(parent_lex, LangC_TokenKind_RightParen);
		leading_spaces = parent_lex->token.leading_spaces;
		
		while (*def_head && *def_head++ != ')');
	}
	
	// NOTE(ljre): Expand macro
	LangC_Lexer* lex = &(LangC_Lexer) {
		.preprocessor = true,
		.file = parent_lex->file,
		.line = parent_lex->line,
		.ctx = ctx,
	};
	
	// NOTE(ljre): If the lexer already has waiting tokens, we want to insert right before them.
	//             Save and append them to the end after expanding.
	LangC_TokenList* saved_first_token = parent_lex->waiting_token;
	LangC_TokenList* saved_last_token = parent_lex->last_waiting_token;
	
	parent_lex->waiting_token = NULL;
	
	LangC_SetupLexer(lex, def_head, parent_lex->arena);
	LangC_NextToken(lex);
	
	// NOTE(ljre): Expand macro.
	while (lex->token.kind != LangC_TokenKind_Eof)
	{
		switch (lex->token.kind)
		{
			case LangC_TokenKind_Hashtag:
			{
				LangC_NextToken(lex);
				
				LangC_MacroParameter* param;
				if (!macro->is_func_like ||
					!LangC_AssertToken(lex, LangC_TokenKind_Identifier) ||
					!(param = LangC_FindMacroParameter(params, param_count, lex->token.value_ident)))
				{
					LangC_LexerError(parent_lex, "expected a macro parameter for stringification.");
				}
				else
				{
					uintsize len = strlen(param->expands_to);
					char* buf = Arena_PushAligned(ctx->stage_arena, len+2, 1);
					
					buf[0] = '"';
					OurMemCopy(buf + 1, param->expands_to, len);
					buf[len + 1] = '"';
					
					LangC_Token tok = {
						.kind = LangC_TokenKind_StringLiteral,
						.leading_spaces = lex->token.leading_spaces,
						.as_string = {
							.size = len + 2,
							.data = buf,
						},
						.value_str = {
							.size = len,
							.data = buf + 1,
						},
					};
					
					LangC_PushToken(parent_lex, &tok);
					LangC_NextToken(lex);
				}
			} break;
			
			case LangC_TokenKind_Identifier:
			{
				LangC_MacroParameter* param;
				if (lex->token.kind == LangC_TokenKind_Identifier &&
					!lex->token_was_pushed &&
					(param = LangC_FindMacroParameter(params, param_count, lex->token.value_ident)))
				{
					LangC_PushStringOfTokens(lex, param->expands_to);
					LangC_NextToken(lex);
					break;
				}
			} /* fallthrough */
			
			default:
			{
				LangC_Token tok = lex->token;
				LangC_Token incoming = LangC_PeekIncomingToken(lex);
				bool32 from_macro = lex->token_was_pushed;
				LangC_Macro* to_expand;
				
				if (incoming.kind == LangC_TokenKind_DoubleHashtag)
				{
					LangC_NextToken(lex); // NOTE(ljre): Eat left side
					LangC_NextToken(lex); // NOTE(ljre): Eat ##
					LangC_MacroParameter* param;
					
					if (lex->token.kind == LangC_TokenKind_Identifier &&
						(param = LangC_FindMacroParameter(params, param_count, lex->token.value_ident)))
					{
						LangC_PushStringOfTokens(lex, param->expands_to);
						LangC_NextToken(lex);
					}
					
					LangC_Token other_tok = lex->token;
					LangC_NextToken(lex);
					
					uintsize len = tok.as_string.size + other_tok.as_string.size + 1;
					char* buf = Arena_Push(ctx->stage_arena, len);
					
					OurMemCopy(buf, tok.as_string.data, tok.as_string.size);
					OurMemCopy(buf + tok.as_string.size, other_tok.as_string.data, other_tok.as_string.size);
					buf[tok.as_string.size + other_tok.as_string.size] = 0;
					
					// TODO(ljre): Check if result of concatenation should be expanded on the 4th step.
					LangC_PushStringOfTokens(parent_lex, buf);
				}
				else if (from_macro &&
						 tok.kind == LangC_TokenKind_Identifier &&
						 (to_expand = LangC_FindMacro(ctx, tok.value_ident,
													  (incoming.kind == LangC_TokenKind_LeftParen) ? 3 : 0)))
				{
					// NOTE(ljre): Tokens originated from parameters shall be expanded first.
					LangC_ExpandMacro(ctx, to_expand, lex, tok.leading_spaces);
				}
				else
				{
					LangC_NextToken(lex);
					LangC_PushToken(parent_lex, &tok);
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
	LangC_NextToken(parent_lex);
	
	macro->expanding = false;
}

internal int32 LangC_EvalPreprocessorExprBinary(LangC_Context* ctx, LangC_Lexer* lex, int32 level);

internal int32
LangC_EvalPreprocessorExprFactor(LangC_Context* ctx, LangC_Lexer* lex)
{
	beginning:;
	int32 result = 0;
	
	switch (lex->token.kind)
	{
		// NOTE(ljre): Unary operators
		case LangC_TokenKind_Minus:
		{
			LangC_NextToken(lex);
			return -LangC_EvalPreprocessorExprFactor(ctx, lex);
		} break;
		
		case LangC_TokenKind_Plus:
		{
			LangC_NextToken(lex);
			goto beginning;
		} break;
		
		case LangC_TokenKind_Not:
		{
			LangC_NextToken(lex);
			return -LangC_EvalPreprocessorExprFactor(ctx, lex);
		} break;
		
		case LangC_TokenKind_LNot:
		{
			LangC_NextToken(lex);
			return !LangC_EvalPreprocessorExprFactor(ctx, lex);
		} break;
		
		// NOTE(ljre): Factors
		case LangC_TokenKind_IntLiteral:
		case LangC_TokenKind_LIntLiteral:
		case LangC_TokenKind_LLIntLiteral:
		{
			result = (int32)lex->token.value_int;
			LangC_NextToken(lex);
		} break;
		
		case LangC_TokenKind_UintLiteral:
		case LangC_TokenKind_LUintLiteral:
		case LangC_TokenKind_LLUintLiteral:
		{
			result = (int32)lex->token.value_uint;
			LangC_NextToken(lex);
		} break;
		
		case LangC_TokenKind_FloatLiteral:
		{
			result = (int32)lex->token.value_float;
			LangC_NextToken(lex);
		} break;
		
		case LangC_TokenKind_DoubleLiteral:
		{
			result = (int32)lex->token.value_double;
			LangC_NextToken(lex);
		} break;
		
		case LangC_TokenKind_WideStringLiteral:
		case LangC_TokenKind_StringLiteral:
		{
			// TODO(ljre): What should happen if it's a string literal?
			result = (lex->token.value_str.size > 0);
			LangC_NextToken(lex);
		} break;
		
		case LangC_TokenKind_Identifier:
		{
			String name = lex->token.value_ident;
			
			if (MatchCString("defined", name.data, name.size))
			{
				LangC_NextToken(lex);
				bool32 has_paren = LangC_TryToEatToken(lex, LangC_TokenKind_LeftParen);
				
				if (LangC_AssertToken(lex, LangC_TokenKind_Identifier))
				{
					result = (LangC_FindMacro(ctx, lex->token.value_ident, 2) != NULL);
					LangC_NextToken(lex);
				}
				
				if (has_paren)
					LangC_EatToken(lex, LangC_TokenKind_RightParen);
			}
			else
			{
				bool32 is_func = (LangC_PeekIncomingToken(lex).kind == LangC_TokenKind_LeftParen);
				LangC_Macro* macro = LangC_FindMacro(ctx, name, is_func);
				
				if (macro)
				{
					LangC_ExpandMacro(ctx, macro, lex, lex->token.leading_spaces);
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
LangC_EvalPreprocessorExprOp(LangC_TokenKind op, int32 left, int32 right)
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
LangC_EvalPreprocessorExprBinary(LangC_Context* ctx, LangC_Lexer* lex, int32 level)
{
	int32 result = LangC_EvalPreprocessorExprFactor(ctx, lex);
	
	LangC_OperatorPrecedence prec;
	while (prec = LangC_operators_precedence[lex->token.kind],
		   prec.level > level)
	{
		LangC_TokenKind op = lex->token.kind;
		LangC_NextToken(lex);
		int32 right = LangC_EvalPreprocessorExprFactor(ctx, lex);
		
		LangC_TokenKind lookahead = lex->token.kind;
		LangC_OperatorPrecedence lookahead_prec = LangC_operators_precedence[lookahead];
		
		while (lookahead_prec.level > 0 &&
			   (lookahead_prec.level > prec.level ||
				(lookahead_prec.level == prec.level && lookahead_prec.right2left))) {
			LangC_NextToken(lex);
			
			if (lookahead == LangC_TokenKind_QuestionMark) {
				int32 if_true = LangC_EvalPreprocessorExprBinary(ctx, lex, 0);
				
				LangC_EatToken(lex, LangC_TokenKind_Colon);
				int32 if_false = LangC_EvalPreprocessorExprBinary(ctx, lex, level - 1);
				
				right = right ? if_true : if_false;
			} else {
				int32 other = LangC_EvalPreprocessorExprBinary(ctx, lex, level + 1);
				
				right = LangC_EvalPreprocessorExprOp(lookahead, right, other);
			}
			
			lookahead = lex->token.kind;
			lookahead_prec = LangC_operators_precedence[lookahead];
		}
		
		result = LangC_EvalPreprocessorExprOp(op, result, right);
	}
	
	return result;
}

internal int32
LangC_EvalPreprocessorExpr(LangC_Context* ctx, LangC_Lexer* lex)
{
	return LangC_EvalPreprocessorExprBinary(ctx, lex, 0);
}

internal void
LangC_IgnoreUntilEndOfIf(LangC_Context* ctx, LangC_Lexer* lex, bool32 already_matched)
{
	int32 nesting = 1;
	
	while (lex->token.kind && nesting > 0)
	{
		switch (lex->token.kind)
		{
			case LangC_TokenKind_Hashtag:
			{
				LangC_NextToken(lex);
				
				if (LangC_AssertToken(lex, LangC_TokenKind_Identifier))
				{
					String directive = lex->token.value_ident;
					bool32 not = false;
					
					if (!already_matched && nesting == 1)
					{
						if (MatchCString("elifdef", directive.data, directive.size) ||
							(MatchCString("elifndef", directive.data, directive.size) && (not = true)))
						{
							LangC_NextToken(lex);
							LangC_PreprocessIfDef(ctx, lex, not);
							
							goto out_of_the_loop;
						}
						else if (MatchCString("elif", directive.data, directive.size))
						{
							LangC_NextToken(lex);
							LangC_PreprocessIf(ctx, lex);
							
							goto out_of_the_loop;
						}
						else if (MatchCString("else", directive.data, directive.size))
							nesting = 0;
					}
					
					if (MatchCString("ifdef", directive.data, directive.size) ||
						MatchCString("ifndef", directive.data, directive.size) ||
						MatchCString("if", directive.data, directive.size))
					{
						++nesting;
					}
					else if (MatchCString("endif", directive.data, directive.size))
					{
						--nesting;
						
						if (nesting <= 0)
						{
							LangC_IgnoreUntilNewline(lex);
							break;
						}
					}
				}
			} /* fallthrough */
			
			default:
			{
				LangC_IgnoreUntilNewline(lex);
				LangC_TryToEatToken(lex, LangC_TokenKind_NewLine);
			} break;
		}
	}
	
	out_of_the_loop:;
	
	LangC_TracePreprocessor(ctx, lex, 0);
}

internal void
LangC_PreprocessIfDef(LangC_Context* ctx, LangC_Lexer* lex, bool32 not)
{
	bool32 result = false;
	
	if (LangC_AssertToken(lex, LangC_TokenKind_Identifier))
	{
		String name = lex->token.value_ident;
		result = (LangC_FindMacro(ctx, name, 2) != NULL) ^ not;
	}
	
	if (!result)
	{
		LangC_IgnoreUntilNewline(lex);
		LangC_IgnoreUntilEndOfIf(ctx, lex, false);
	}
}

internal void
LangC_PreprocessIf(LangC_Context* ctx, LangC_Lexer* lex)
{
	int32 result = LangC_EvalPreprocessorExpr(ctx, lex);
	
	if (result == 0)
		LangC_IgnoreUntilEndOfIf(ctx, lex, false);
}

internal void
LangC_PreprocessInclude(LangC_Context* ctx, LangC_Lexer* lex)
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
	
	String fullpath;
	const char* contents = LangC_TryToLoadFile(ctx, path, relative, lex->file->path, &fullpath);
	if (contents)
	{
		LangC_Preprocess2(ctx, fullpath, contents, lex);
		LangC_TracePreprocessor(ctx, lex, 2);
	}
	else
		LangC_LexerError(lex, "could not find file '%S' when including.", StrFmt(path));
}

internal void
LangC_GeneratePlainPragma(LangC_Context* ctx, const char* begin, const char* end)
{
	Arena_Printf(ctx->persistent_arena, "_Pragma(\"");
	const char* p = begin;
	
	for (; p != end; ++p)
	{
		if (*p == '"')
		{
			Arena_Printf(ctx->persistent_arena, "%S\\\"", p - begin, begin);
			begin = p + 1;
		}
		else if (*p == '\\')
		{
			Arena_Printf(ctx->persistent_arena, "%S\\\\", p - begin, begin);
			begin = p + 1;
		}
	}
	
	Arena_Printf(ctx->persistent_arena, "%S\")", p - begin, begin);
}

internal void
LangC_Preprocess2(LangC_Context* ctx, String path, const char* source, LangC_Lexer* from)
{
	TraceName(path);
	
	LangC_Lexer* lex = &(LangC_Lexer) {
		.preprocessor = true,
		.file = from ? from->file : NULL,
		.ctx = ctx,
	};
	
	LangC_SetupLexer(lex, source, ctx->stage_arena);
	LangC_PushLexerFile(lex, path, from);
	
	LangC_TracePreprocessor(ctx, lex, 1);
	
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
				
				if (lex->token.kind != LangC_TokenKind_Identifier)
				{
					LangC_LexerError(lex, "invalid preprocessor directive '%S'.", StrFmt(directive));
					LangC_IgnoreUntilNewline(lex);
					break;
				}
				
				bool32 not = false;
				if (MatchCString("include", directive.data, directive.size))
				{
					LangC_NextToken(lex);
					LangC_PreprocessInclude(ctx, lex);
				}
				else if (MatchCString("ifdef", directive.data, directive.size) ||
						 (not = MatchCString("ifndef", directive.data, directive.size)))
				{
					LangC_NextToken(lex);
					LangC_PreprocessIfDef(ctx, lex, not);
				}
				else if (MatchCString("if", directive.data, directive.size))
				{
					LangC_NextToken(lex);
					LangC_PreprocessIf(ctx, lex);
				}
				else if (MatchCString("elif", directive.data, directive.size) ||
						 MatchCString("elifdef", directive.data, directive.size) ||
						 MatchCString("elifndef", directive.data, directive.size) ||
						 MatchCString("else", directive.data, directive.size))
				{
					LangC_NextToken(lex);
					LangC_IgnoreUntilEndOfIf(ctx, lex, true);
				}
				else if (MatchCString("endif", directive.data, directive.size))
				{
					LangC_NextToken(lex);
					// NOTE(ljre): :P
				}
				else if (MatchCString("line", directive.data, directive.size))
				{
					LangC_NextToken(lex);
					
					int32 line = lex->line;
					if (LangC_AssertToken(lex, LangC_TokenKind_IntLiteral))
						line = lex->token.value_int;
					
					lex->line = line-1; // NOTE(ljre): :P
					LangC_TracePreprocessor(ctx, lex, 0);
				}
				else if (MatchCString("define", directive.data, directive.size))
				{
					const char* def = lex->head;
					
					LangC_NextToken(lex);
					if (LangC_AssertToken(lex, LangC_TokenKind_Identifier))
					{
						while (lex->head[0] && (lex->head[0] != '\n' || lex->head[-1] == '\\'))
							++lex->head;
						
						const char* end = lex->head;
						
						String macro_def = {
							.size = (uintsize)(end - def),
							.data = def,
						};
						
						LangC_DefineMacro(ctx, macro_def);
					}
				}
				else if (MatchCString("undef", directive.data, directive.size))
				{
					LangC_NextToken(lex);
					
					if (LangC_AssertToken(lex, LangC_TokenKind_Identifier))
					{
						LangC_UndefineMacro(ctx, lex->token.value_ident);
					}
				}
				else if (MatchCString("error", directive.data, directive.size))
				{
					const char* begin = lex->head;
					const char* end = lex->head;
					
					while (*end && *end != '\n')
						++end;
					
					uintsize len = end - begin;
					LangC_LexerError(lex, "\"%S\"", len, begin);
					
					lex->head = end;
				}
				else if (MatchCString("warning", directive.data, directive.size))
				{
					const char* begin = lex->head;
					const char* end = lex->head;
					
					while (*end && *end != '\n')
						++end;
					
					uintsize len = end - begin;
					LangC_LexerWarning(lex, LangC_Warning_UserWarning, "\"%S\"", len, begin);
					
					lex->head = end;
				}
				else if (MatchCString("pragma", directive.data, directive.size))
				{
					const char* begin = lex->head;
					const char* end = lex->head;
					
					while (*end && *end != '\n')
						++end;
					
					LangC_NextToken(lex);
					
					if (lex->token.kind == LangC_TokenKind_Identifier &&
						MatchCString("once", lex->token.value_ident.data, lex->token.value_ident.size))
					{
						LangC_PragmaOncePPFile(ctx, path);
					}
					else
					{
						lex->head = end;
						LangC_GeneratePlainPragma(ctx, begin, end);
					}
				}
				else
					LangC_LexerError(lex, "unknown pre-processor directive '%S'.", StrFmt(directive));
				
				LangC_IgnoreUntilNewline(lex);
				LangC_NextToken(lex);
				Arena_PushMemory(ctx->persistent_arena, 1, "\n");
			} break;
			
			case LangC_TokenKind_Identifier:
			{
				String ident = lex->token.value_ident;
				String leading_spaces = lex->token.leading_spaces;
				LangC_Macro* macro;
				
				if (LangC_PeekIncomingToken(lex).kind == LangC_TokenKind_LeftParen)
					macro = LangC_FindMacro(ctx, ident, 3);
				else
					macro = LangC_FindMacro(ctx, ident, 0);
				
				if (macro)
					LangC_ExpandMacro(ctx, macro, lex, leading_spaces);
				else
				{
					Arena_PushMemory(ctx->persistent_arena, ident.size, ident.data);
					Arena_PushMemory(ctx->persistent_arena, leading_spaces.size, leading_spaces.data);
					LangC_NextToken(lex);
				}
				
				previous_was_newline = false;
			} break;
			
			default:
			{
				previous_was_newline = false;
				
				if (false)
				{
					case LangC_TokenKind_NewLine:
					previous_was_newline = true;
				}
				
				Arena_PushMemory(ctx->persistent_arena, StrFmt(lex->token.as_string));
				Arena_PushMemory(ctx->persistent_arena, StrFmt(lex->token.leading_spaces));
				LangC_NextToken(lex);
			} break;
		}
	}
	
	// NOTE(ljre): Done preprocessing this file!
}

internal bool32
LangC_Preprocess(LangC_Context* ctx, String path)
{
	Trace();
	
	// NOTE(ljre): Those macros are handled internally, but a definition is still needed.
	LangC_DefineMacro(ctx, Str("__LINE__"))->persistent = true;
	LangC_DefineMacro(ctx, Str("__FILE__"))->persistent = true;
	
	String fullpath;
	ctx->source = LangC_TryToLoadFile(ctx, path, true, StrNull, &fullpath);
	ctx->pre_source = Arena_End(ctx->persistent_arena);
	ctx->use_stage_arena_for_warnings = true;
	
	if (ctx->source)
		LangC_Preprocess2(ctx, fullpath, ctx->source, NULL);
	else
	{
		Print("error: could not open input file '%S'.\n", StrFmt(path));
		++LangC_error_count;
	}
	
	ctx->use_stage_arena_for_warnings = false;
	
	return LangC_error_count == 0;
}
