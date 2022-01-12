// NOTE(ljre): This lexer won't check if UTF-8 codepoints are valid, though it will check for BOM
//             at the beginning of the file.

internal void C_NextToken(C_Lexer* lex);

internal void
C_SetupLexer(C_Lexer* lex, const char* source, C_Context* ctx, Arena* arena)
{
	if (lex->trace.col == 0)
		lex->trace.col = 1;
	if (lex->trace.line == 0)
		lex->trace.line = 1;
	
	lex->head = source;
	// NOTE(ljre): Ignore BOM.
	if ((unsigned char)lex->head[0] == 0xEF && (unsigned char)lex->head[1] == 0xBB && (unsigned char)lex->head[2] == 0xBF)
		lex->head += 3;
	
	lex->token.kind = C_TokenKind_Eof;
	lex->previous_head = lex->head;
	lex->arena = arena;
	lex->ctx = ctx;
}

internal void
C_PushFileTrace(C_Lexer* lex, String path, C_Lexer* trace_from)
{
	C_SourceFileTrace* file = Arena_Push(lex->arena, sizeof(*file));
	
	if (!trace_from)
		trace_from = lex;
	
	file->path = path;
	file->included_line = trace_from->trace.line;
	file->included_from = trace_from->trace.file;
	file->is_system_file = trace_from->trace.file ? trace_from->trace.file->is_system_file : false;
	lex->trace.file = file;
}

internal void
C_PopFileTrace(C_Lexer* lex)
{
	C_SourceFileTrace* file = lex->trace.file;
	
	lex->trace.file = file->included_from;
	lex->trace.line = file->included_line + 1;
}

internal inline bool32
C_IsNumeric(char ch, int32 base)
{
	return (base == 2 && (ch == '0' || ch == '1') ||
			(ch >= '0' && ch <= '9' || (base == 16 && (ch >= 'a' && ch <= 'f' || ch >= 'A' && ch <= 'F'))));
}

internal inline bool32
C_IsAlpha(char ch)
{
	return (ch >= 'A' & ch <= 'Z') | (ch >= 'a' & ch <= 'z');
}

internal inline bool32
C_IsIdentChar(char ch)
{
	return ch == '_' || (ch >= '0' && ch <= '9') || C_IsAlpha(ch)
		|| (unsigned char)ch >= 128; // NOTE(ljre): This makes every multibyte UTF-8 codepoint a valid char for identifiers.
}

internal void
C_PrintIncludeStackToArena(C_SourceFileTrace* file, uint32 line, Arena* arena)
{
	if (file->included_from)
		C_PrintIncludeStackToArena(file->included_from, file->included_line, arena);
	
	Arena_Printf(arena, "%C1%S%C0(%u): in included file\n", StrFmt(file->path), line);
}

internal void
C_TraceErrorVarargs(C_Context* ctx, C_SourceTrace* trace, const char* fmt, va_list args)
{
	ctx->error_count++;
	C_SourceFileTrace* file = trace->file;
	
	char* buf = Arena_End(ctx->stage_arena);
	Arena_PushMemory(ctx->stage_arena, 1, "\n");
	if (file->included_from)
		C_PrintIncludeStackToArena(file->included_from, file->included_line, ctx->stage_arena);
	
	Arena_Printf(ctx->stage_arena, "%C1%S%C0(%i:%i): %C2error%C0: ", StrFmt(file->path), trace->line, trace->col);
	Arena_VPrintf(ctx->stage_arena, fmt, args);
	Arena_PushMemory(ctx->stage_arena, 2, "\n");
	
	PrintFast(buf);
	Arena_Pop(ctx->stage_arena, buf);
}

internal void
C_TraceError(C_Context* ctx, C_SourceTrace* trace, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	C_TraceErrorVarargs(ctx, trace, fmt, args);
	va_end(args);
}

internal void
C_TraceWarningVarargs(C_Context* ctx, C_SourceTrace* trace, C_Warning warning, const char* fmt, va_list args)
{
	if (!C_IsWarningEnabled(ctx, warning))
		return;
	
	C_SourceFileTrace* file = trace->file;
	char* buf = Arena_End(ctx->stage_arena);
	
	Arena_PushMemory(ctx->stage_arena, 1, "\n");
	if (file->included_from)
		C_PrintIncludeStackToArena(file->included_from, file->included_line, ctx->stage_arena);
	
	Arena_Printf(ctx->stage_arena, "%C1%S%C0(%i:%i): %C3warning%C0: ", StrFmt(file->path), trace->line, trace->col);
	Arena_VPrintf(ctx->stage_arena, fmt, args);
	Arena_PushMemory(ctx->stage_arena, 1, "");
	
	C_PushWarning(ctx, warning, buf);
}

internal void
C_TraceWarning(C_Context* ctx, C_SourceTrace* trace, C_Warning warning, const char* fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	C_TraceWarningVarargs(ctx, trace, warning, fmt, args);
	va_end(args);
}

internal C_TokenKind
C_FindKeywordByString(String str)
{
	for (int32 keyword = C_TokenKind__FirstKeyword; keyword <= C_TokenKind__LastKeyword; ++keyword)
	{
		if (MatchCString(C_token_str_table[keyword], str))
			return keyword;
	}
	
	return C_TokenKind_Identifier;
}

internal void
C_PushToken(C_Lexer* lex, C_Token* token)
{
	C_TokenList* node = Arena_Push(lex->arena, sizeof *node);
	node->token = *token;
	
	if (!lex->waiting_token)
	{
		lex->waiting_token = node;
		lex->last_waiting_token = node;
	}
	else
	{
		lex->last_waiting_token = lex->last_waiting_token->next = node;
	}
}

internal void
C_PushTokenToFront(C_Lexer* lex, C_Token* token)
{
	C_TokenList* node = Arena_Push(lex->arena, sizeof *node);
	node->token = *token;
	
	if (!lex->waiting_token)
	{
		lex->waiting_token = node;
		lex->last_waiting_token = node;
	}
	else
	{
		node->next = lex->waiting_token;
		lex->waiting_token = node;
	}
}

internal void
C_PushStringOfTokens(C_Lexer* lex, const char* str)
{
	C_Lexer temp_lex = {
		.preprocessor = lex->preprocessor,
		.trace = lex->trace,
	};
	
	C_SetupLexer(&temp_lex, str, lex->ctx, lex->arena);
	C_NextToken(&temp_lex);
	
	while (temp_lex.token.kind)
	{
		C_PushToken(lex, &temp_lex.token);
		
		temp_lex.trace = lex->trace;
		C_NextToken(&temp_lex);
	}
}

internal C_Token
C_PeekIncomingToken(C_Lexer* lex)
{
	C_Lexer tmp = *lex;
	
	C_NextToken(&tmp);
	
	return tmp.token;
}

internal void
C_UpdateLexerPreviousHead(C_Lexer* lex)
{
	Trace();
	
	for (; lex->previous_head < lex->head; ++lex->previous_head)
	{
		switch (lex->previous_head[0])
		{
			case '\n': lex->trace.line++;
			case '\r': lex->trace.col = 1; break;
			default: lex->trace.col++; break;
		}
	}
}

internal void
C_IgnoreWhitespaces(const char** p, bool32 newline)
{
	for (;;)
	{
		if (!**p)
			break;
		
		// NOTE(ljre): C++-style comments
		if (**p == '/' && (*p)[1] == '/')
		{
			*p += 2;
			
			while (**p && (**p != '\n' || (*p)[-1] == '\\'))
				++*p;
		}
		// NOTE(ljre): C-style comments
		else if (**p == '/' && (*p)[1] == '*')
		{
			*p += 2;
			
			// TODO(ljre): Maybe nesting?
			while (**p && !((*p)[-2] == '*' && (*p)[-1] == '/'))
				++*p;
		}
		// NOTE(ljre): General whitespaces & newlines
		else if (**p == ' ' || **p == '\t' || **p == '\r' || (newline && **p == '\n'))
		{
			++*p;
		}
		// NOTE(ljre): Treating a \ followed by a newline as a whitespace. Yes, this is incorrect by the standard.
		else if (**p == '\\' && (*p)[1] == '\n')
		{
			*p += 2;
		}
		else if (**p == '\\' && (*p)[1] == '\r')
		{
			*p += 2 + ((*p)[2] == '\n');
		}
		else
		{
			break;
		}
	}
}

internal int32
C_ValueOfEscaped(const char** ptr)
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
			
			while (chars_in_hex --> 0 &&
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

internal String
C_TokenizeStringLiteral(C_Lexer* lex)
{
	const char* begin = lex->head;
	const char* end = begin + 1;
	
	while (end[0] && end[0] != '"')
	{
		if (end[0] == '\n')
			break;
		
		if (end[0] == '\\' && ((end[1] == '\n' || end[1] == '\r' && end[2] == '\n')
							   || end[1] == '\\' || end[1] == '"'))
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
		C_TraceError(lex->ctx, &lex->trace, "missing closing quote.");
		return StrNull;
	}
	
	int32 len = (int32)(end - begin);
	
	String result = {
		.size = (uintsize)(len - 1),
		.data = begin + 1,
	};
	
	lex->head += len + 1;
	
	return result;
}

internal void
C_NextToken(C_Lexer* lex)
{
	Trace();
	
	if (lex->waiting_token)
	{
		lex->token = lex->waiting_token->token;
		lex->waiting_token = lex->waiting_token->next;
		lex->token_was_pushed = true;
		return;
	}
	
	lex->token_was_pushed = false;
	
	beginning:;
	C_IgnoreWhitespaces(&lex->head, !lex->preprocessor);
	
	lex->token.as_string.data = lex->head;
	lex->token.trace = lex->trace;
	
	switch (lex->head[0])
	{
		case 0:
		{
			lex->token.kind = C_TokenKind_Eof;
		} break;
		
		case '\n':
		{
			lex->token.kind = C_TokenKind_NewLine;
			++lex->head;
		} break;
		
		case '#':
		{
			if (lex->preprocessor)
			{
				if (lex->head[1] == '#')
				{
					lex->token.kind = C_TokenKind_DoubleHashtag;
					lex->head += 2;
				}
				else
				{
					lex->token.kind = C_TokenKind_Hashtag;
					++lex->head;
				}
			}
			else
			{
				++lex->head;
				C_IgnoreWhitespaces(&lex->head, false);
				
				if (C_IsNumeric(lex->head[0], 10))
				{
					// NOTE(ljre): Parse pre-processor's metadata.
					//             https://gcc.gnu.org/onlinedocs/gcc-11.1.0/cpp/Preprocessor-Output.html
					
					int32 line = strtol(lex->head, (char**)&lex->head, 10);
					
					C_IgnoreWhitespaces(&lex->head, false);
					String file = C_TokenizeStringLiteral(lex);
					
					int32 flags = 0;
					
					while (C_IgnoreWhitespaces(&lex->head, false),
						   lex->head[0] && lex->head[0] != '\n')
					{
						if (C_IsNumeric(lex->head[0], 10))
						{
							flags |= 1 << (lex->head[0] - '0' - 1);
						}
						
						++lex->head;
					}
					
					if (lex->head[0] == '\n')
						++lex->head;
					
					lex->previous_head = lex->head;
					lex->trace.col = 1;
					
					// NOTE(ljre): "This indicates the start of a new file."
					if (flags & 1 || !lex->trace.file)
					{
						C_PushFileTrace(lex, file, NULL);
					}
					
					// NOTE(ljre): "This indicates returning to a file (after having included another file)."
					if (flags & 2)
					{
						C_PopFileTrace(lex);
					}
					
					lex->trace.line = line;
					
					// NOTE(ljre): "This indicates that the following text comes from a system header file, so
					//              certain warnings should be suppressed."
					if (flags & 4)
					{
						lex->trace.file->is_system_file = true;
					}
					
					// NOTE(ljre): "This indicates that the following text should be treated as being wrapped
					//              in an implicit extern "C" block."
					if (flags & 8)
					{
						// TODO(ljre)
					}
				}
				else
				{
					while (lex->head[0] && lex->head++[0] != '\n');
				}
				
				goto beginning;
			}
		} break;
		
		case '.':
		{
			if (!C_IsNumeric(lex->head[1], 10))
			{
				if (lex->head[1] == '.' && lex->head[2] == '.')
				{
					lex->token.kind = C_TokenKind_VarArgs;
					lex->head += 3;
				}
				else
				{
					lex->token.kind = C_TokenKind_Dot;
					++lex->head;
				}
				
				break;
			}
		} /* fallthrough */
		
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9':
		{
			int32 base = 10;
			if (lex->head[0] == '0')
			{
				if (lex->head[1] == 'x' || lex->head[1] == 'X')
				{
					lex->head += 2;
					base = 16;
				}
				else if (lex->head[1] == 'b' || lex->head[1] == 'B')
				{
					lex->head += 2;
					base = 2;
				}
				else
				{
					lex->head += 1;
					base = 8;
				}
			}
			
			const char* begin = lex->head;
			const char* end = begin;
			
			while (C_IsNumeric(*end, base))
				++end;
			
			if (*end == 'e' || *end == 'E')
				goto parse_exponent;
			
			if (*end == '.')
			{
				++end;
				
				while (C_IsNumeric(*end, base))
					++end;
				
				if (base == 2)
					C_TraceError(lex->ctx, &lex->trace, "floats cannot begin with '0b'.");
				
				if (*end == 'e' || *end == 'E' || (base == 16 && (*end == 'p' || *end == 'P')))
				{
					parse_exponent:;
					++end;
					
					if (*end == '+' || *end == '-')
						++end;
					
					while (C_IsNumeric(*end, 10))
						++end;
				}
				
				if (*end == 'f' || *end == 'F')
				{
					++end;
					lex->token.kind = C_TokenKind_FloatLiteral;
					lex->token.value_float = strtof(begin, NULL);
				}
				else
				{
					if (*end == 'l' || *end == 'L')
						++end;
					
					lex->token.kind = C_TokenKind_DoubleLiteral;
					lex->token.value_double = strtod(begin, NULL);
				}
			}
			else
			{
				bool32 is_unsig = false;
				lex->token.kind = C_TokenKind_IntLiteral;
				
				if ((is_unsig = *end == 'u' || *end == 'U'))
					++end, lex->token.kind = C_TokenKind_UintLiteral;
				if (*end == 'l' || *end == 'L')
					++end, lex->token.kind += 1;
				if (*end == 'l' || *end == 'L')
					++end, lex->token.kind += 1;
				
				if (is_unsig)
					lex->token.value_uint = strtoull(begin, NULL, base);
				else
					lex->token.value_int = strtoll(begin, NULL, base);
			}
			
			lex->head = end;
		} break;
		
		case 'L':
		{
			if (lex->head[1] == '"')
			{
				++lex->head;
				
				lex->token.value_str = C_TokenizeStringLiteral(lex);
				lex->token.kind = C_TokenKind_WideStringLiteral;
				break;
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
			const char* begin = lex->head;
			while (C_IsIdentChar(lex->head[0]))
				++lex->head;
			
			lex->token.kind = C_TokenKind_Identifier;
			lex->token.value_ident.data = begin;
			lex->token.value_ident.size = (uintsize)(lex->head - begin);
			
			if (!lex->preprocessor)
				lex->token.kind = C_FindKeywordByString(lex->token.value_ident);
		} break;
		
		case '"':
		{
			lex->token.value_str = C_TokenizeStringLiteral(lex);
			lex->token.kind = C_TokenKind_StringLiteral;
		} break;
		
		case '\'':
		{
			int64 value = 0;
			int32 max_chars = 4;
			
			++lex->head;
			
			while (max_chars --> 0)
			{
				if (lex->head[0] == '\'')
					break;
				
				value <<= 8;
				
				if (lex->head[0] == '\\')
				{
					const char* end = ++lex->head;
					value |= C_ValueOfEscaped(&end);
					lex->head = end;
				}
				else
				{
					value |= lex->head[0];
					
					++lex->head;
				}
			}
			
			lex->token.kind = C_TokenKind_IntLiteral;
			lex->token.value_int = value;
			
			if (lex->head[0] != '\'')
			{
				C_TraceError(lex->ctx, &lex->trace, "missing pair of character literal.");
				break;
			}
			
			++lex->head;
		} break;
		
		case '(': lex->token.kind = C_TokenKind_LeftParen; ++lex->head; break;
		case ')': lex->token.kind = C_TokenKind_RightParen; ++lex->head; break;
		case '[': lex->token.kind = C_TokenKind_LeftBrkt; ++lex->head; break;
		case ']': lex->token.kind = C_TokenKind_RightBrkt; ++lex->head; break;
		case '{': lex->token.kind = C_TokenKind_LeftCurl; ++lex->head; break;
		case '}': lex->token.kind = C_TokenKind_RightCurl; ++lex->head; break;
		case ',': lex->token.kind = C_TokenKind_Comma; ++lex->head; break;
		case ':': lex->token.kind = C_TokenKind_Colon; ++lex->head; break;
		case ';': lex->token.kind = C_TokenKind_Semicolon; ++lex->head; break;
		case '?': lex->token.kind = C_TokenKind_QuestionMark; ++lex->head; break;
		case '~': lex->token.kind = C_TokenKind_Not; ++lex->head; break;
		
		case '/':
		{
			if (lex->head[1] == '=')
			{
				lex->token.kind = C_TokenKind_DivAssign;
				lex->head += 2;
			}
			else
			{
				lex->token.kind = C_TokenKind_Div;
				++lex->head;
			}
		} break;
		
		case '-':
		{
			lex->token.kind = C_TokenKind_Minus;
			++lex->head;
			
			if (lex->head[0] == '>')
			{
				lex->token.kind = C_TokenKind_Arrow;
				++lex->head;
			}
			else if (lex->head[0] == '-')
			{
				lex->token.kind = C_TokenKind_Dec;
				++lex->head;
			}
			else if (lex->head[0] == '=')
			{
				lex->token.kind = C_TokenKind_MinusAssign;
				++lex->head;
			}
		} break;
		
		case '+':
		{
			lex->token.kind = C_TokenKind_Plus;
			++lex->head;
			
			if (lex->head[0] == '+')
			{
				lex->token.kind = C_TokenKind_Inc;
				++lex->head;
			}
			else if (lex->head[0] == '=')
			{
				lex->token.kind = C_TokenKind_PlusAssign;
				++lex->head;
			}
		} break;
		
		case '*':
		{
			lex->token.kind = C_TokenKind_Mul;
			++lex->head;
			
			if (lex->head[0] == '=')
			{
				lex->token.kind = C_TokenKind_MulAssign;
				++lex->head;
			}
		} break;
		
		case '%':
		{
			lex->token.kind = C_TokenKind_Mod;
			++lex->head;
			
			if (lex->head[0] == '=')
			{
				lex->token.kind = C_TokenKind_ModAssign;
				++lex->head;
			}
		} break;
		
		case '<':
		{
			lex->token.kind = C_TokenKind_LThan;
			++lex->head;
			
			if (lex->head[0] == '<')
			{
				lex->token.kind = C_TokenKind_LeftShift;
				++lex->head;
				
				if (lex->head[0] == '=')
				{
					lex->token.kind = C_TokenKind_LeftShiftAssign;
					++lex->head;
				}
			}
			else if (lex->head[0] == '=')
			{
				lex->token.kind = C_TokenKind_LEqual;
				++lex->head;
			}
		} break;
		
		case '>':
		{
			lex->token.kind = C_TokenKind_GThan;
			++lex->head;
			
			if (lex->head[0] == '>')
			{
				lex->token.kind = C_TokenKind_RightShift;
				++lex->head;
				
				if (lex->head[0] == '=')
				{
					lex->token.kind = C_TokenKind_RightShiftAssign;
					++lex->head;
				}
			}
			else if (lex->head[0] == '=')
			{
				lex->token.kind = C_TokenKind_GEqual;
				++lex->head;
			}
		} break;
		
		case '=':
		{
			lex->token.kind = C_TokenKind_Assign;
			++lex->head;
			
			if (lex->head[0] == '=')
			{
				lex->token.kind = C_TokenKind_Equals;
				++lex->head;
			}
		} break;
		
		case '!':
		{
			lex->token.kind = C_TokenKind_LNot;
			++lex->head;
			
			if (lex->head[0] == '=')
			{
				lex->token.kind = C_TokenKind_NotEquals;
				++lex->head;
			}
		} break;
		
		case '&':
		{
			lex->token.kind = C_TokenKind_And;
			++lex->head;
			
			if (lex->head[0] == '&')
			{
				lex->token.kind = C_TokenKind_LAnd;
				++lex->head;
			}
			else if (lex->head[0] == '=')
			{
				lex->token.kind = C_TokenKind_AndAssign;
				++lex->head;
			}
		} break;
		
		case '|':
		{
			lex->token.kind = C_TokenKind_Or;
			++lex->head;
			
			if (lex->head[0] == '|')
			{
				lex->token.kind = C_TokenKind_LOr;
				++lex->head;
			}
			else if (lex->head[0] == '=')
			{
				lex->token.kind = C_TokenKind_OrAssign;
				++lex->head;
			}
		} break;
		
		case '^':
		{
			lex->token.kind = C_TokenKind_Xor;
			++lex->head;
			
			if (lex->head[0] == '=')
			{
				lex->token.kind = C_TokenKind_XorAssign;
				++lex->head;
			}
		} break;
		
		default:
		{
			C_TraceError(lex->ctx, &lex->trace, "unexpected token '%c'.", lex->head[0]);
			++lex->head;
		} break;
	}
	
	lex->token.as_string.size = (uintsize)(lex->head - lex->token.as_string.data);
	
	lex->token.leading_spaces.data = lex->head;
	C_IgnoreWhitespaces(&lex->head, !lex->preprocessor);
	lex->token.leading_spaces.size = (uintsize)(lex->head - lex->token.leading_spaces.data);
	
	C_UpdateLexerPreviousHead(lex);
}

internal bool32
C_AssertToken(C_Lexer* lex, C_TokenKind kind)
{
	bool32 result = true;
	
	if (lex->token.kind != kind)
	{
		result = false;
		if (lex->token.kind)
			C_TraceError(lex->ctx, &lex->token.trace, "expected '%s', but got '%S'.",
						 C_token_str_table[kind], StrFmt(lex->token.as_string));
		else
			C_TraceError(lex->ctx, &lex->token.trace, "expected '%s' before end of file.", C_token_str_table[kind]);
	}
	
	return result;
}

internal bool32
C_EatToken(C_Lexer* lex, C_TokenKind kind)
{
	bool32 result = C_AssertToken(lex, kind);
	C_NextToken(lex);
	return result;
}

internal bool32
C_TryToEatToken(C_Lexer* lex, C_TokenKind kind)
{
	if (lex->token.kind == kind)
	{
		C_NextToken(lex);
		return true;
	}
	
	return false;
}
