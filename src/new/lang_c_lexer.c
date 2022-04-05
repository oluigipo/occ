//~ NOTE(ljre): Actual Lexing.
internal String
C_TokenizeStringLiteral(C_Context* ctx, const char** phead, char close)
{
	Assert(**phead == close);
	
	const char* begin = *phead;
	const char* end = begin + 1;
	
	while (end[0] && end[0] != close)
	{
		if (end[0] == '\n')
			break;
		
		if (end[0] == '\\' && (end[1] == close || end[1] == '\\'))
			end += 2;
		else
			++end;
	}
	
	if (*end != close)
	{
		*phead = begin + 1;
		return StrNull;
	}
	
	String result = {
		.size = (uintsize)(end - begin),
		.data = begin,
	};
	
	*phead = end + 1;
	
	return result;
}

internal C_TokenSlice
C_Tokenize(C_Context* ctx, Arena* arena, const char* source, C_SourceTrace* up_trace)
{
	TraceName(up_trace->file ? up_trace->file->path : Str("internal string"));
	
	C_Token* result = Arena_Push(arena, sizeof(*result));
	C_Token* tok = result;
	
	const char* head = source;
	if ((uint8)head[0] == 0xEF && (uint8)head[1] == 0xBB && (uint8)head[2] == 0xBF)
		head += 3;
	
	const char* previous_head = source;
	
	C_SourceTrace trace = *up_trace;
	
	while (*head)
	{
		{
			const char* l = head;
			C_IgnoreWhitespaces(&head, false);
			
			tok->leading_spaces = head - l;
		}
		
		for (; previous_head < head; ++previous_head)
		{
			switch (previous_head[0])
			{
				case '\n': trace.line++;
				case '\r': trace.col = 1; break;
				default: trace.col++; break;
			}
		}
		
		tok->as_string.data = head;
		
		switch (head[0])
		{
			case 0:
			{
				tok->kind = C_TokenKind_Eof;
			} break;
			
			case '\r': ++head;
			case '\n':
			{
				tok->kind = C_TokenKind_NewLine;
				++head;
			} break;
			
			case '#':
			{
				if (head[1] == '#')
				{
					tok->kind = C_TokenKind_DoubleHashtag;
					head += 2;
				}
				else
				{
					tok->kind = C_TokenKind_Hashtag;
					++head;
				}
			} break;
			
			case '.':
			{
				if (!C_IsNumeric(head[1], 10))
				{
					if (head[1] == '.' && head[2] == '.')
					{
						tok->kind = C_TokenKind_VarArgs;
						head += 3;
					}
					else
					{
						tok->kind = C_TokenKind_Dot;
						++head;
					}
					
					break;
				}
			} /* fallthrough */
			
			case '0': case '1': case '2': case '3': case '4':
			case '5': case '6': case '7': case '8': case '9':
			{
				int32 base = 10;
				if (head[0] == '0')
				{
					if (head[1] == 'x' || head[1] == 'X')
					{
						head += 2;
						base = 16;
					}
					else if (head[1] == 'b' || head[1] == 'B')
					{
						head += 2;
						base = 2;
					}
					else
					{
						head += 1;
						base = 8;
					}
				}
				
				const char* begin = head;
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
						C_TraceError(ctx, &trace, "floats cannot begin with '0b'.");
					
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
						tok->kind = C_TokenKind_FloatLiteral;
					}
					else
					{
						if (*end == 'l' || *end == 'L')
							++end;
						
						tok->kind = C_TokenKind_DoubleLiteral;
					}
				}
				else
				{
					tok->kind = C_TokenKind_IntLiteral;
					
					if (*end == 'u' || *end == 'U')
						++end, tok->kind = C_TokenKind_UintLiteral;
					if (*end == 'l' || *end == 'L')
						++end, tok->kind += 1;
					if (*end == 'l' || *end == 'L')
						++end, tok->kind += 1;
				}
				
				head = end;
			} break;
			
			case 'L':
			{
				if (head[1] == '"')
				{
					++head;
					
					String r = C_TokenizeStringLiteral(ctx, &head, '"');
					
					if (r.size == 0)
						tok->kind = C_TokenKind_UnclosedQuote;
					else
						tok->kind = C_TokenKind_WideStringLiteral;
					
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
				while (C_IsIdentChar(head[0]))
					++head;
				
				tok->kind = C_TokenKind_Identifier;
			} break;
			
			case '"':
			{
				String r = C_TokenizeStringLiteral(ctx, &head, '"');
				if (r.size == 0)
					tok->kind = C_TokenKind_UnclosedQuote;
				else
					tok->kind = C_TokenKind_StringLiteral;
			} break;
			
			case '\'':
			{
				String r = C_TokenizeStringLiteral(ctx, &head, '\'');
				
				if (r.size == 0)
					tok->kind = C_TokenKind_UnclosedQuote;
				else
					tok->kind = C_TokenKind_CharLiteral;
			} break;
			
			case '(': tok->kind = C_TokenKind_LeftParen; ++head; break;
			case ')': tok->kind = C_TokenKind_RightParen; ++head; break;
			case '[': tok->kind = C_TokenKind_LeftBrkt; ++head; break;
			case ']': tok->kind = C_TokenKind_RightBrkt; ++head; break;
			case '{': tok->kind = C_TokenKind_LeftCurl; ++head; break;
			case '}': tok->kind = C_TokenKind_RightCurl; ++head; break;
			case ',': tok->kind = C_TokenKind_Comma; ++head; break;
			case ':': tok->kind = C_TokenKind_Colon; ++head; break;
			case ';': tok->kind = C_TokenKind_Semicolon; ++head; break;
			case '?': tok->kind = C_TokenKind_QuestionMark; ++head; break;
			case '~': tok->kind = C_TokenKind_Not; ++head; break;
			
			case '/':
			{
				if (head[1] == '=')
				{
					tok->kind = C_TokenKind_DivAssign;
					head += 2;
				}
				else
				{
					tok->kind = C_TokenKind_Div;
					++head;
				}
			} break;
			
			case '-':
			{
				tok->kind = C_TokenKind_Minus;
				++head;
				
				if (head[0] == '>')
				{
					tok->kind = C_TokenKind_Arrow;
					++head;
				}
				else if (head[0] == '-')
				{
					tok->kind = C_TokenKind_Dec;
					++head;
				}
				else if (head[0] == '=')
				{
					tok->kind = C_TokenKind_MinusAssign;
					++head;
				}
			} break;
			
			case '+':
			{
				tok->kind = C_TokenKind_Plus;
				++head;
				
				if (head[0] == '+')
				{
					tok->kind = C_TokenKind_Inc;
					++head;
				}
				else if (head[0] == '=')
				{
					tok->kind = C_TokenKind_PlusAssign;
					++head;
				}
			} break;
			
			case '*':
			{
				tok->kind = C_TokenKind_Mul;
				++head;
				
				if (head[0] == '=')
				{
					tok->kind = C_TokenKind_MulAssign;
					++head;
				}
			} break;
			
			case '%':
			{
				tok->kind = C_TokenKind_Mod;
				++head;
				
				if (head[0] == '=')
				{
					tok->kind = C_TokenKind_ModAssign;
					++head;
				}
			} break;
			
			case '<':
			{
				tok->kind = C_TokenKind_LThan;
				++head;
				
				if (head[0] == '<')
				{
					tok->kind = C_TokenKind_LeftShift;
					++head;
					
					if (head[0] == '=')
					{
						tok->kind = C_TokenKind_LeftShiftAssign;
						++head;
					}
				}
				else if (head[0] == '=')
				{
					tok->kind = C_TokenKind_LEqual;
					++head;
				}
			} break;
			
			case '>':
			{
				tok->kind = C_TokenKind_GThan;
				++head;
				
				if (head[0] == '>')
				{
					tok->kind = C_TokenKind_RightShift;
					++head;
					
					if (head[0] == '=')
					{
						tok->kind = C_TokenKind_RightShiftAssign;
						++head;
					}
				}
				else if (head[0] == '=')
				{
					tok->kind = C_TokenKind_GEqual;
					++head;
				}
			} break;
			
			case '=':
			{
				tok->kind = C_TokenKind_Assign;
				++head;
				
				if (head[0] == '=')
				{
					tok->kind = C_TokenKind_Equals;
					++head;
				}
			} break;
			
			case '!':
			{
				tok->kind = C_TokenKind_LNot;
				++head;
				
				if (head[0] == '=')
				{
					tok->kind = C_TokenKind_NotEquals;
					++head;
				}
			} break;
			
			case '&':
			{
				tok->kind = C_TokenKind_And;
				++head;
				
				if (head[0] == '&')
				{
					tok->kind = C_TokenKind_LAnd;
					++head;
				}
				else if (head[0] == '=')
				{
					tok->kind = C_TokenKind_AndAssign;
					++head;
				}
			} break;
			
			case '|':
			{
				tok->kind = C_TokenKind_Or;
				++head;
				
				if (head[0] == '|')
				{
					tok->kind = C_TokenKind_LOr;
					++head;
				}
				else if (head[0] == '=')
				{
					tok->kind = C_TokenKind_OrAssign;
					++head;
				}
			} break;
			
			case '^':
			{
				tok->kind = C_TokenKind_Xor;
				++head;
				
				if (head[0] == '=')
				{
					tok->kind = C_TokenKind_XorAssign;
					++head;
				}
			} break;
			
			default:
			{
				C_TraceError(ctx, &trace, "unexpected token '%c'.", head[0]);
				++head;
			} break;
		}
		
		tok->as_string.size = (uintsize)(head - tok->as_string.data);
		tok->trace = trace;
		
		tok = Arena_Push(arena, sizeof(*tok));
	}
	
	return (C_TokenSlice) { .size = tok - result, .data = result };
}

//~ NOTE(ljre): Parsing Values
internal uint64
C_TokenizeIntLiteral(C_Context* ctx, String str)
{
	void* end = Arena_End(ctx->scratch_arena);
	
	const char* ptr = Arena_NullTerminateString(ctx->scratch_arena, str);
	uint64 result = strtoull(ptr, NULL, 0);
	
	Arena_Pop(ctx->scratch_arena, end);
	
	return result;
}

internal double
C_TokenizeDoubleLiteral(C_Context* ctx, String str)
{
	void* end = Arena_End(ctx->scratch_arena);
	
	const char* ptr = Arena_NullTerminateString(ctx->scratch_arena, str);
	int64 result = strtod(ptr, NULL);
	
	Arena_Pop(ctx->scratch_arena, end);
	
	return result;
}

internal int32
C_TokenizeCharLiteral(C_Context* ctx, String str)
{
	Assert(str.data[0] == '\'' && str.data[str.size-1] == '\'');
	
	int32 result = 0;
	
	const char* head = str.data + 1;
	const char* end  = str.data + str.size - 1;
	
	while (head < end)
	{
		result <<= 8;
		
		if (*head == '\\' && head + 1 < end)
			result += (uint8)C_ValueOfEscaped(&head, end);
		else
			result += *head;
		
		++head;
	}
	
	return result;
}

//~ NOTE(ljre): C_TokenReader
internal const C_Token C_eof_token = { 0 };

internal inline void
C_NextToken(C_TokenReader* rd)
{
	if (rd->queue)
	{
		rd->head = &rd->queue->token;
		rd->queue = rd->queue->next;
	}
	else if (rd->slice_head < rd->slice_end)
		rd->head = rd->slice_head++;
	else
		rd->head = &C_eof_token;
}

internal inline const C_Token*
C_PeekToken(C_TokenReader* rd)
{
	if (rd->queue)
		return &rd->queue->token;
	else if (rd->slice_head < rd->slice_end)
		return rd->slice_head;
	else
		return &C_eof_token;
}

internal inline bool
C_AssertToken(C_Context* ctx, C_TokenReader* rd, C_TokenKind kind)
{
	bool result = true;
	
	if (rd->head->kind != kind)
	{
		result = false;
		
		if (rd->head->kind)
			C_TraceErrorRd(ctx, rd, "expected '%s', but got '%S'.",
						   C_token_str_table[kind], StrFmt(rd->head->as_string));
		else
			C_TraceErrorRd(ctx, rd, "expected '%s' before end of file.", C_token_str_table[kind]);
	}
	
	return result;
}

internal inline bool
C_EatToken(C_Context* ctx, C_TokenReader* rd, C_TokenKind kind)
{
	bool result = C_AssertToken(ctx, rd, kind);
	C_NextToken(rd);
	return result;
}

internal inline bool
C_TryToEatToken(C_TokenReader* rd, C_TokenKind kind)
{
	if (rd->head->kind == kind)
	{
		C_NextToken(rd);
		return true;
	}
	
	return false;
}
