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

struct LangC_MacroParameter
{
	String name;
	const char* expands_to;
}
typedef LangC_MacroParameter;

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

internal void LangC_Preprocess2(LangC_Preprocessor* pp, String path, const char* source, LangC_Lexer* from);
internal void LangC_PreprocessIfDef(LangC_Preprocessor* pp, LangC_Lexer* lex, bool32 not);
internal void LangC_PreprocessIf(LangC_Preprocessor* pp, LangC_Lexer* lex);

internal const char*
LangC_LoadFileFromDisk(LangC_Preprocessor* pp, const char path[MAX_PATH_SIZE], uint64 calculated_hash, bool32 relative)
{
	const char* contents = OS_ReadWholeFile(path);
	
	if (contents)
	{
		LangC_PPLoadedFile* file;
		
		if (pp->loaded_files)
		{
			file = pp->loaded_files;
			while (file->next)
				file = file->next;
			
			file = file->next = PushMemory(sizeof *file);
		}
		else
		{
			file = pp->loaded_files = PushMemory(sizeof *file);
		}
		
		file->hash = calculated_hash;
		file->contents = contents;
		file->relative = relative;
	}
	
	return contents;
}

internal const char*
LangC_TryToLoadFile(LangC_Preprocessor* pp, String path, bool32 relative, String including_from, String* out_fullpath)
{
	char fullpath[MAX_PATH_SIZE];
	
	if (relative)
	{
		LangC_PPLoadedFile* file = pp->loaded_files;
		
		uintsize len = 0;
		if (including_from.size > 0)
		{
			memcpy(fullpath, including_from.data, including_from.size);
			
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
		
		memcpy(fullpath + len, path.data, path.size);
		len += path.size;
		
		OS_ResolveFullPath(StrMake(fullpath, len), fullpath);
		uint64 search_hash = SimpleHashNullTerminated(fullpath);
		
		while (file)
		{
			if (file->hash == search_hash && file->relative)
				return file->contents;
			
			file = file->next;
		}
		
		const char* contents = LangC_LoadFileFromDisk(pp, fullpath, search_hash, true);
		if (contents)
		{
			len = strlen(fullpath) + 1;
			char* mem = PushMemory(len);
			memcpy(mem, fullpath, len);
			
			*out_fullpath = StrMake(mem, len);
			return contents;
		}
	}
	
	for (int32 i = 0; i < LangC_include_dirs_count; ++i)
	{
		const char* include_dir = LangC_include_dirs[i];
		uintsize len = strlen(include_dir);
		
		memcpy(fullpath, include_dir, len);
		memcpy(fullpath + len, path.data, path.size);
		
		String p = {
			.size = len + path.size,
			.data = fullpath,
		};
		
		OS_ResolveFullPath(p, fullpath);
		
		LangC_PPLoadedFile* file = pp->loaded_files;
		uint64 search_hash = SimpleHashNullTerminated(fullpath);
		
		while (file)
		{
			if (file->hash == search_hash && !file->relative)
				return file->contents;
			
			file = file->next;
		}
		
		const char* contents = LangC_LoadFileFromDisk(pp, fullpath, search_hash, true);
		if (contents)
		{
			uintsize len = strlen(fullpath) + 1;
			char* mem = PushMemory(len);
			memcpy(mem, fullpath, len);
			
			*out_fullpath = StrMake(mem, len);
			return contents;
		}
	}
	
	return NULL;
}

internal void
LangC_IgnoreUntilNewline(LangC_Lexer* lex)
{
	//while (lex->token.kind && lex->token.kind != LangC_TokenKind_NewLine)
	//    LangC_NextToken(lex);
	
	while (lex->head[0] && (lex->head[0] != '\n' || lex->head[-1] == '\\'))
		++lex->head;
	
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
LangC_TracePreprocessor(LangC_Preprocessor* pp, LangC_Lexer* lex, uint32 flags)
{
	char str[2048];
	int32 len;
	assert(flags < 16);
	
	static const char* flag_table[] = {
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
		len = snprintf(str, sizeof str, "# %i \"%.*s\" %s\n", lex->line, StrFmt(lex->file->path), flag_table[flags]);
	else
		len = snprintf(str, sizeof str, "# %i \"%.*s\"\n", lex->line, StrFmt(lex->file->path));
	
	SB_PushArray(pp->buf, len, str);
}

internal void
LangC_ExpandMacro(LangC_Preprocessor* pp, LangC_Macro* macro, LangC_Lexer* parent_lex, String leading_spaces)
{
	// TODO(ljre): Handle native macros
#if 1
	if (MatchCString("__LINE__", macro->name.data, macro->name.size))
	{
		int32 line = parent_lex->line;
		int32 needed = snprintf(NULL, 0, "%i", line) + 1;
		char* mem = PushMemory(needed);
		snprintf(mem, needed + 1, "%i", line);
		
		LangC_Token tok = {
			.kind = LangC_TokenKind_IntLiteral,
			.as_string = StrMake(mem, needed),
			.leading_spaces = Str(""),
			.value_int = line,
		};
		
		LangC_PushTokenToFront(parent_lex, &tok);
		LangC_NextToken(parent_lex);
		return;
	}
	else if (MatchCString("__FILE__", macro->name.data, macro->name.size))
	{
		String file = parent_lex->file->path;
		uintsize needed = file.size + 3;
		char* mem = PushMemory(needed);
		
		mem[0] = '"';
		memcpy(mem + 1, file.data, file.size);
		mem[file.size + 1] = '"';
		mem[file.size + 2] = 0;
		
		LangC_Token tok = {
			.kind = LangC_TokenKind_StringLiteral,
			.as_string = StrMake(mem, needed),
			.leading_spaces = Str(""),
			.value_str = file,
		};
		
		LangC_PushTokenToFront(parent_lex, &tok);
		LangC_NextToken(parent_lex);
		return;
	}
#endif
	
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
				assert(param_count <= ArrayLength(params));
				
				String name;
				char* buf = NULL;
				SB_ReserveAtLeast(buf, 64);
				
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
						
						SB_PushArray(buf, parent_lex->token.as_string.size, parent_lex->token.as_string.data);
						SB_PushArray(buf, parent_lex->token.leading_spaces.size, parent_lex->token.leading_spaces.data);
						
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
						
						SB_PushArray(buf, parent_lex->token.as_string.size, parent_lex->token.as_string.data);
						SB_PushArray(buf, parent_lex->token.leading_spaces.size, parent_lex->token.leading_spaces.data);
						
						LangC_NextToken(parent_lex);
					}
				}
				
				SB_Push(buf, 0);
				
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
		.line = parent_lex->line
	};
	
	// NOTE(ljre): If the lexer already has waiting tokens, we want to insert right before them.
	//             Save and append them to the end after expanding.
	LangC_TokenList* saved_first_token = parent_lex->waiting_token;
	LangC_TokenList* saved_last_token = parent_lex->last_waiting_token;
	
	parent_lex->waiting_token = NULL;
	
	LangC_SetupLexer(lex, def_head);
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
					char* buf = NULL;
					uintsize len = strlen(param->expands_to);
					SB_ReserveAtLeast(buf, len+2);
					
					SB_Push(buf, '"');
					SB_PushArray(buf, len, param->expands_to);
					SB_Push(buf, '"');
					
					LangC_Token tok = {
						.kind = LangC_TokenKind_StringLiteral,
						.leading_spaces = lex->token.leading_spaces,
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
				bool32 from_macro = lex->token_was_pushed;
				LangC_Macro* to_expand;
				
				if (LangC_PeekIncomingToken(lex).kind == LangC_TokenKind_DoubleHashtag)
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
					char* buf = PushMemory(len);
					
					memcpy(buf, tok.as_string.data, tok.as_string.size);
					memcpy(buf + tok.as_string.size, other_tok.as_string.data, other_tok.as_string.size);
					buf[tok.as_string.size + other_tok.as_string.size] = 0;
					
					// TODO(ljre): Check if result of concatenation should be expanded on the 4th step.
					LangC_PushStringOfTokens(parent_lex, buf);
				}
				else if (from_macro &&
						 tok.kind == LangC_TokenKind_Identifier &&
						 (to_expand = LangC_FindMacro(pp, tok.value_ident, 0)))
				{
					// NOTE(ljre): Tokens originated from parameters shall be expanded first.
					LangC_ExpandMacro(pp, to_expand, lex, tok.leading_spaces);
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

internal int32 LangC_EvalPreprocessorExprBinary(LangC_Preprocessor* pp, LangC_Lexer* lex, int32 level);

internal int32
LangC_EvalPreprocessorExprFactor(LangC_Preprocessor* pp, LangC_Lexer* lex)
{
	beginning:;
	int32 result = 0;
	
	switch (lex->token.kind)
	{
		// NOTE(ljre): Unary operators
		case LangC_TokenKind_Minus:
		{
			LangC_NextToken(lex);
			return -LangC_EvalPreprocessorExprFactor(pp, lex);
		} break;
		
		case LangC_TokenKind_Plus:
		{
			LangC_NextToken(lex);
			goto beginning;
		} break;
		
		case LangC_TokenKind_Not:
		{
			LangC_NextToken(lex);
			return -LangC_EvalPreprocessorExprFactor(pp, lex);
		} break;
		
		case LangC_TokenKind_LNot:
		{
			LangC_NextToken(lex);
			return !LangC_EvalPreprocessorExprFactor(pp, lex);
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
					result = (LangC_FindMacro(pp, lex->token.value_ident, 2) != NULL);
					LangC_NextToken(lex);
				}
				
				if (has_paren)
					LangC_EatToken(lex, LangC_TokenKind_RightParen);
			}
			else
			{
				bool32 is_func = (LangC_PeekIncomingToken(lex).kind == LangC_TokenKind_LeftParen);
				LangC_Macro* macro = LangC_FindMacro(pp, name, is_func);
				
				if (macro)
				{
					LangC_ExpandMacro(pp, macro, lex, lex->token.leading_spaces);
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
LangC_EvalPreprocessorExprBinary(LangC_Preprocessor* pp, LangC_Lexer* lex, int32 level)
{
	int32 result = LangC_EvalPreprocessorExprFactor(pp, lex);
	
	LangC_OperatorPrecedence prec;
	while (prec = LangC_operators_precedence[lex->token.kind],
		   prec.level > level)
	{
		LangC_TokenKind op = lex->token.kind;
		LangC_NextToken(lex);
		int32 right = LangC_EvalPreprocessorExprFactor(pp, lex);
		
		LangC_TokenKind lookahead = lex->token.kind;
		LangC_OperatorPrecedence lookahead_prec = LangC_operators_precedence[lookahead];
		
		while (lookahead_prec.level > 0 &&
			   (lookahead_prec.level > prec.level ||
				(lookahead_prec.level == prec.level && lookahead_prec.right2left))) {
			LangC_NextToken(lex);
			
			if (lookahead == LangC_TokenKind_QuestionMark) {
				int32 if_true = LangC_EvalPreprocessorExprBinary(pp, lex, 1);
				
				LangC_EatToken(lex, LangC_TokenKind_Colon);
				int32 if_false = LangC_EvalPreprocessorExprBinary(pp, lex, level + 1);
				
				right = right ? if_true : if_false;
			} else {
				int32 other = LangC_EvalPreprocessorExprBinary(pp, lex, level + 1);
				
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
LangC_EvalPreprocessorExpr(LangC_Preprocessor* pp, LangC_Lexer* lex)
{
	return LangC_EvalPreprocessorExprBinary(pp, lex, 0);
}

internal void
LangC_IgnoreUntilEndOfIf(LangC_Preprocessor* pp, LangC_Lexer* lex, bool32 already_matched)
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
							LangC_PreprocessIfDef(pp, lex, not);
							
							goto out_of_the_loop;
						}
						else if (MatchCString("elif", directive.data, directive.size))
						{
							LangC_NextToken(lex);
							LangC_PreprocessIf(pp, lex);
							
							goto out_of_the_loop;
						}
						else if (MatchCString("else", directive.data, directive.size))
						{
							nesting = 0;
						}
					}
					else
					{
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
	LangC_TracePreprocessor(pp, lex, 0);
}

internal void
LangC_PreprocessIfDef(LangC_Preprocessor* pp, LangC_Lexer* lex, bool32 not)
{
	bool32 result = false;
	
	if (LangC_AssertToken(lex, LangC_TokenKind_Identifier))
	{
		String name = lex->token.value_ident;
		result = (LangC_FindMacro(pp, name, 2) != NULL) ^ not;
	}
	
	if (!result)
	{
		LangC_IgnoreUntilNewline(lex);
		LangC_IgnoreUntilEndOfIf(pp, lex, false);
	}
}

internal void
LangC_PreprocessIf(LangC_Preprocessor* pp, LangC_Lexer* lex)
{
	int32 result = LangC_EvalPreprocessorExpr(pp, lex);
	
	if (result == 0)
	{
		LangC_IgnoreUntilNewline(lex);
		LangC_IgnoreUntilEndOfIf(pp, lex, false);
	}
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
	
	String fullpath;
	const char* contents = LangC_TryToLoadFile(pp, path, relative, lex->file->path, &fullpath);
	if (contents)
	{
		LangC_Preprocess2(pp, fullpath, contents, lex);
		LangC_TracePreprocessor(pp, lex, 2);
	}
	else
	{
		LangC_LexerError(lex, "could not find file '%.*s' when including.", StrFmt(path));
	}
}

internal void
LangC_Preprocess2(LangC_Preprocessor* pp, String path, const char* source, LangC_Lexer* from)
{
	LangC_Lexer* lex = &(LangC_Lexer) {
		.preprocessor = true,
		.file = from ? from->file : NULL,
	};
	
	LangC_SetupLexer(lex, source);
	LangC_PushLexerFile(lex, path, from);
	
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
				
				if (lex->token.kind != LangC_TokenKind_Identifier)
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
						 (not = MatchCString("ifndef", directive.data, directive.size)))
				{
					LangC_NextToken(lex);
					LangC_PreprocessIfDef(pp, lex, not);
				}
				else if (MatchCString("if", directive.data, directive.size))
				{
					LangC_NextToken(lex);
					LangC_PreprocessIf(pp, lex);
				}
				else if (MatchCString("elif", directive.data, directive.size) ||
						 MatchCString("elifdef", directive.data, directive.size) ||
						 MatchCString("elifndef", directive.data, directive.size) ||
						 MatchCString("else", directive.data, directive.size))
				{
					LangC_NextToken(lex);
					LangC_IgnoreUntilEndOfIf(pp, lex, true);
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
					LangC_TracePreprocessor(pp, lex, 0);
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
				else if (MatchCString("pragma", directive.data, directive.size))
				{
					LangC_NextToken(lex);
					
					// TODO(ljre)
				}
				else
				{
					LangC_LexerError(lex, "unknown pre-processor directive '%.*s'.", StrFmt(directive));
				}
				
				LangC_IgnoreUntilNewline(lex);
				LangC_NextToken(lex);
				SB_Push(pp->buf, '\n');
			} break;
			
			case LangC_TokenKind_Identifier:
			{
				String ident = lex->token.value_ident;
				String leading_spaces = lex->token.leading_spaces;
				LangC_Macro* macro;
				
				if (LangC_PeekIncomingToken(lex).kind == LangC_TokenKind_LeftParen)
				{
					macro = LangC_FindMacro(pp, ident, 3);
				}
				else
				{
					macro = LangC_FindMacro(pp, ident, 0);
				}
				
				if (macro)
				{
					LangC_ExpandMacro(pp, macro, lex, leading_spaces);
				}
				else
				{
					SB_PushArray(pp->buf, ident.size, ident.data);
					SB_PushArray(pp->buf, leading_spaces.size, leading_spaces.data);
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
				SB_PushArray(pp->buf, lex->token.leading_spaces.size, lex->token.leading_spaces.data);
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
	LangC_DefineMacro(&pp, Str("__volatile volatile"));
	LangC_DefineMacro(&pp, Str("__volatile__ volatile"));
	LangC_DefineMacro(&pp, Str("__attribute(...)"));
	LangC_DefineMacro(&pp, Str("__attribute__(...)"));
	LangC_DefineMacro(&pp, Str("__declspec(...)"));
	LangC_DefineMacro(&pp, Str("__builtin_offsetof(_Type, _Field) (&((_Type*)0)->_Field)"));
	LangC_DefineMacro(&pp, Str("__cdecl"));
	LangC_DefineMacro(&pp, Str("__stdcall"));
	LangC_DefineMacro(&pp, Str("__vectorcall"));
	LangC_DefineMacro(&pp, Str("__fastcall"));
	LangC_DefineMacro(&pp, Str("_VA_LIST_DEFINED"));
	LangC_DefineMacro(&pp, Str("va_list void*"));
	
	// NOTE(ljre): Those macros are handled internally, but a definition is still needed.
	LangC_DefineMacro(&pp, Str("__LINE__"));
	LangC_DefineMacro(&pp, Str("__FILE__"));
	
	String fullpath;
	const char* source = LangC_TryToLoadFile(&pp, path, true, StrNull, &fullpath);
	
	if (source)
		LangC_Preprocess2(&pp, fullpath, source, NULL);
	else
	{
		Print("error: could not open input file '%.*s'.\n", StrFmt(path));
		++LangC_error_count;
	}
	
	return pp.buf;
}
