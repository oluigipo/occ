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
}
typedef LangC_Token;

struct LangC_LexerFile typedef LangC_LexerFile;
struct LangC_LexerFile
{
	String path;
	int32 included_line;
	bool32 is_system_file;
	LangC_LexerFile* included_from;
};

struct LangC_Lexer
{
	LangC_Token token;
	LangC_LexerFile* file;
	
	const char* head;
	const char* previous_head;
	int32 line, col;
}
typedef LangC_Lexer;

internal void
LangC_SetupLexer(LangC_Lexer* lex, const char* source)
{
	lex->line = lex->col = 1;
	
	lex->token.kind = LangC_TokenKind_Eof;
	lex->head = lex->previous_head = source;
}

internal void
LangC_PushLexerFile(LangC_Lexer* lex, String path)
{
	LangC_LexerFile* file = PushMemory(sizeof *file);
	
	file->path = path;
	file->included_line = lex->line;
	file->included_from = lex->file;
	file->is_system_file = lex->file ? lex->file->is_system_file : false;
	lex->file = file;
}

internal void
LangC_PopLexerFile(LangC_Lexer* lex)
{
	LangC_LexerFile* file = lex->file;
	
	lex->file = file->included_from;
	lex->line = file->included_line + 1;
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
LangC_PrintIncludeStack(LangC_LexerFile* file, int32 line)
{
	if (file->included_from)
		LangC_PrintIncludeStack(file->included_from, file->included_line);
	
	Print("%.*s(%i): in included file\n", StrFmt(file->path), line);
}

internal void
LangC_LexerError(LangC_Lexer* lex, const char* fmt, ...)
{
	LangC_LexerFile* file = lex->file;
	LangC_error_count++;
	
	Print("\n");
	if (file->included_from)
		LangC_PrintIncludeStack(file->included_from, file->included_line);
	
	Print("%.*s(%i:%i): error: ", StrFmt(file->path), lex->line, lex->col);
	
	va_list args;
	va_start(args, fmt);
	PrintVarargs(fmt, args);
	va_end(args);
	
	Print("\n");
}

internal void
LangC_LexerWarning(LangC_Lexer* lex, const char* fmt, ...)
{
	LangC_LexerFile* file = lex->file;
	
	Print("\n");
	if (file->included_from)
		LangC_PrintIncludeStack(file->included_from, file->included_line);
	
	Print("%.*s(%i:%i): warning: ", StrFmt(file->path), lex->line, lex->col);
	
	va_list args;
	va_start(args, fmt);
	PrintVarargs(fmt, args);
	va_end(args);
	
	Print("\n");
}

internal void
LangC_UpdateLexerPreviousHead(LangC_Lexer* lex)
{
	for (; lex->previous_head < lex->head; ++lex->previous_head)
	{
		switch (lex->previous_head[0])
		{
			case '\n': lex->line++;
			case '\r': lex->col = 1; break;
			default: lex->col++; break;
		}
	}
}

internal void
LangC_IgnoreWhitespaces(const char** p, bool32 newline)
{
	for (;;)
	{
		if (!**p)
			break;
		
		if (**p == ' ' || **p == '\t' || **p == '\r' || (newline && **p == '\n'))
		{
			++*p;
		}
		else if (**p == '\\' && (*p)[1] == '\n')
		{
			*p += 2;
		}
		else
		{
			break;
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

internal String
LangC_TokenizeStringLiteral(LangC_Lexer* lex)
{
	const char* begin = lex->head;
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
		LangC_LexerError(lex, "missing closing quote.");
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
LangC_NextToken(LangC_Lexer* lex)
{
	beginning:;
	LangC_IgnoreWhitespaces(&lex->head, true);
	
	lex->token.as_string.data = lex->head;
	lex->token.line = lex->line;
	lex->token.col = lex->col;
	
	switch (lex->head[0])
	{
		case 0:
		{
			lex->token.kind = LangC_TokenKind_Eof;
		} break;
		
		case '#':
		{
			++lex->head;
			
			LangC_IgnoreWhitespaces(&lex->head, false);
			
			if (LangC_IsNumeric(lex->head[0], 10))
			{
				// NOTE(ljre): Parse pre-processor's metadata.
				//             https://gcc.gnu.org/onlinedocs/gcc-11.1.0/cpp/Preprocessor-Output.html
				
				int32 line = strtol(lex->head, (char**)&lex->head, 10);
				
				LangC_IgnoreWhitespaces(&lex->head, false);
				String file = LangC_TokenizeStringLiteral(lex);
				
				int32 flags = 0;
				
				while (LangC_IgnoreWhitespaces(&lex->head, false),
					   lex->head[0] && lex->head[0] != '\n')
				{
					if (LangC_IsNumeric(lex->head[0], 10))
					{
						flags |= 1 << (lex->head[0] - '0' - 1);
					}
					
					++lex->head;
				}
				
				if (lex->head[0] == '\n')
					++lex->head;
				
				lex->previous_head = lex->head;
				lex->line = line;
				lex->col = 1;
				
				// NOTE(ljre): "This indicates the start of a new file."
				if (flags & 1 || !lex->file)
				{
					LangC_PushLexerFile(lex, file);
				}
				
				// NOTE(ljre): "This indicates returning to a file (after having included another file)."
				if (flags & 2)
				{
					LangC_PopLexerFile(lex);
				}
				
				// NOTE(ljre): "This indicates that the following text comes from a system header file, so
				//              certain warnings should be suppressed."
				if (flags & 4)
				{
					lex->file->is_system_file = true;
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
		} break;
		
		case '.':
		{
			if (!LangC_IsNumeric(lex->head[1], 10))
			{
				if (lex->head[1] == '.' && lex->head[2] == '.')
				{
					lex->token.kind = LangC_TokenKind_VarArgs;
					lex->head += 3;
				}
				else
				{
					lex->token.kind = LangC_TokenKind_Dot;
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
				else if (lex->head[1] == 'b')
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
			
			while (LangC_IsNumeric(*end, base)) ++end;
			
			if (*end == 'e' || *end == 'E')
				goto parse_exponent;
			
			if (*end == '.')
			{
				++end;
				
				while (LangC_IsNumeric(*end, base)) ++end;
				
				if (base == 2)
					LangC_LexerError(lex, "error: floats cannot begin with '0b'.");
				
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
					lex->token.kind = LangC_TokenKind_FloatLiteral;
					lex->token.value_float = strtof(begin, NULL);
				}
				else
				{
					if (*end == 'l' || *end == 'L')
						++end;
					
					lex->token.kind = LangC_TokenKind_DoubleLiteral;
					lex->token.value_double = strtod(begin, NULL);
				}
			}
			else
			{
				bool32 is_unsig = false;
				lex->token.kind = LangC_TokenKind_IntLiteral;
				
				if ((is_unsig = *end == 'u' || *end == 'U')) ++end, lex->token.kind = LangC_TokenKind_UintLiteral;
				if (*end == 'l' || *end == 'L') ++end, lex->token.kind += 1;
				if (*end == 'l' || *end == 'L') ++end, lex->token.kind += 1;
				
				if (is_unsig)
					lex->token.value_uint = strtoull(begin, NULL, base);
				else
					lex->token.value_int = strtoll(begin, NULL, base);
			}
			
			int32 eaten = (int32)(end - begin);
			lex->token.as_string.size += eaten;
			lex->head += eaten;
		} break;
		
		case 'L':
		{
			if (lex->head[1] == '"')
			{
				++lex->head;
				
				lex->token.value_str = LangC_TokenizeStringLiteral(lex);
				lex->token.kind = LangC_TokenKind_WideStringLiteral;
				
				// Add L prefix at the beginning
				--lex->token.as_string.data;
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
			while (LangC_IsIdentChar(lex->head[0]))
				++lex->head;
			
			String ident = {
				.data = begin,
				.size = (uintsize)(lex->head - begin),
			};
			
			lex->token.kind = LangC_TokenKind_Identifier;
			lex->token.value_ident = ident;
			
			for (int32 keyword = LangC_TokenKind__FirstKeyword; keyword <= LangC_TokenKind__LastKeyword; ++keyword)
			{
				if (MatchCString(LangC_token_str_table[keyword], ident.data, ident.size))
				{
					lex->token.kind = keyword;
					break;
				}
			}
		} break;
		
		case '"':
		{
			lex->token.value_str = LangC_TokenizeStringLiteral(lex);
			lex->token.kind = LangC_TokenKind_StringLiteral;
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
					value |= LangC_ValueOfEscaped(&end);
					lex->head = end;
				}
				else
				{
					value |= lex->head[0];
					
					++lex->head;
				}
			}
			
			lex->token.kind = LangC_TokenKind_IntLiteral;
			lex->token.value_int = value;
			
			if (lex->head[0] != '\'')
			{
				LangC_LexerError(lex, "missing pair of character literal.");
				break;
			}
			
			++lex->head;
		} break;
		
		case '(': lex->token.kind = LangC_TokenKind_LeftParen; ++lex->head; break;
		case ')': lex->token.kind = LangC_TokenKind_RightParen; ++lex->head; break;
		case '[': lex->token.kind = LangC_TokenKind_LeftBrkt; ++lex->head; break;
		case ']': lex->token.kind = LangC_TokenKind_RightBrkt; ++lex->head; break;
		case '{': lex->token.kind = LangC_TokenKind_LeftCurl; ++lex->head; break;
		case '}': lex->token.kind = LangC_TokenKind_RightCurl; ++lex->head; break;
		case ',': lex->token.kind = LangC_TokenKind_Comma; ++lex->head; break;
		case ':': lex->token.kind = LangC_TokenKind_Colon; ++lex->head; break;
		case ';': lex->token.kind = LangC_TokenKind_Semicolon; ++lex->head; break;
		case '?': lex->token.kind = LangC_TokenKind_QuestionMark; ++lex->head; break;
		
		case '/':
		{
			if (lex->head[1] == '=')
			{
				lex->token.kind = LangC_TokenKind_DivAssign;
				lex->head += 2;
			}
			else
			{
				lex->token.kind = LangC_TokenKind_Div;
				++lex->head;
			}
		} break;
		
		case '-':
		{
			lex->token.kind = LangC_TokenKind_Minus;
			++lex->head;
			
			if (lex->head[0] == '>')
			{
				lex->token.kind = LangC_TokenKind_Arrow;
				++lex->head;
			}
			else if (lex->head[0] == '-')
			{
				lex->token.kind = LangC_TokenKind_Dec;
				++lex->head;
			}
			else if (lex->head[0] == '=')
			{
				lex->token.kind = LangC_TokenKind_MinusAssign;
				++lex->head;
			}
		} break;
		
		case '+':
		{
			lex->token.kind = LangC_TokenKind_Plus;
			++lex->head;
			
			if (lex->head[0] == '+')
			{
				lex->token.kind = LangC_TokenKind_Inc;
				++lex->head;
			}
			else if (lex->head[0] == '=')
			{
				lex->token.kind = LangC_TokenKind_PlusAssign;
				++lex->head;
			}
		} break;
		
		case '*':
		{
			lex->token.kind = LangC_TokenKind_Mul;
			++lex->head;
			
			if (lex->head[0] == '=')
			{
				lex->token.kind = LangC_TokenKind_MulAssign;
				++lex->head;
			}
		} break;
		
		case '%':
		{
			lex->token.kind = LangC_TokenKind_Mod;
			++lex->head;
			
			if (lex->head[0] == '=')
			{
				lex->token.kind = LangC_TokenKind_ModAssign;
				++lex->head;
			}
		} break;
		
		case '<':
		{
			lex->token.kind = LangC_TokenKind_LThan;
			++lex->head;
			
			if (lex->head[0] == '<')
			{
				lex->token.kind = LangC_TokenKind_LeftShift;
				++lex->head;
				
				if (lex->head[0] == '=')
				{
					lex->token.kind = LangC_TokenKind_LeftShiftAssign;
					++lex->head;
				}
			}
			else if (lex->head[0] == '=')
			{
				lex->token.kind = LangC_TokenKind_LEqual;
				++lex->head;
			}
		} break;
		
		case '>':
		{
			lex->token.kind = LangC_TokenKind_GThan;
			++lex->head;
			
			if (lex->head[0] == '>')
			{
				lex->token.kind = LangC_TokenKind_RightShift;
				++lex->head;
				
				if (lex->head[0] == '=')
				{
					lex->token.kind = LangC_TokenKind_RightShiftAssign;
					++lex->head;
				}
			}
			else if (lex->head[0] == '=')
			{
				lex->token.kind = LangC_TokenKind_GEqual;
				++lex->head;
			}
		} break;
		
		case '=':
		{
			lex->token.kind = LangC_TokenKind_Assign;
			++lex->head;
			
			if (lex->head[0] == '=')
			{
				lex->token.kind = LangC_TokenKind_Equals;
				++lex->head;
			}
		} break;
		
		case '!':
		{
			lex->token.kind = LangC_TokenKind_LNot;
			++lex->head;
			
			if (lex->head[0] == '=')
			{
				lex->token.kind = LangC_TokenKind_NotEquals;
				++lex->head;
			}
		} break;
		
		case '&':
		{
			lex->token.kind = LangC_TokenKind_And;
			++lex->head;
			
			if (lex->head[0] == '&')
			{
				lex->token.kind = LangC_TokenKind_LAnd;
				++lex->head;
			}
			else if (lex->head[0] == '=')
			{
				lex->token.kind = LangC_TokenKind_AndAssign;
				++lex->head;
			}
		} break;
		
		case '|':
		{
			lex->token.kind = LangC_TokenKind_Or;
			++lex->head;
			
			if (lex->head[0] == '|')
			{
				lex->token.kind = LangC_TokenKind_LOr;
				++lex->head;
			}
			else if (lex->head[0] == '=')
			{
				lex->token.kind = LangC_TokenKind_OrAssign;
				++lex->head;
			}
		} break;
		
		case '^':
		{
			lex->token.kind = LangC_TokenKind_Xor;
			++lex->head;
			
			if (lex->head[0] == '=')
			{
				lex->token.kind = LangC_TokenKind_XorAssign;
				++lex->head;
			}
		} break;
		
		default:
		{
			LangC_LexerError(lex, "unexpected token '%c'.", lex->head[0]);
			++lex->head;
		} break;
	}
	
	lex->token.as_string.size = (uintsize)(lex->head - lex->token.as_string.data);
	
	// NOTE(ljre): Ignore whitespaces
	LangC_IgnoreWhitespaces(&lex->head, true);
	LangC_UpdateLexerPreviousHead(lex);
}

internal bool32
LangC_AssertToken(LangC_Lexer* lex, LangC_TokenKind kind)
{
	bool32 result = true;
	
	if (lex->token.kind != kind)
	{
		result = false;
		LangC_LexerError(lex, "expected '%s', but got '%.*s'.",
						 LangC_token_str_table[kind], StrFmt(lex->token.as_string));
	}
	
	return result;
}

internal bool32
LangC_EatToken(LangC_Lexer* lex, LangC_TokenKind kind)
{
	bool32 result = LangC_AssertToken(lex, kind);
	LangC_NextToken(lex);
	return result;
}

internal bool32
LangC_TryToEatToken(LangC_Lexer* lex, LangC_TokenKind kind)
{
	if (lex->token.kind == kind)
	{
		LangC_NextToken(lex);
		return true;
	}
	
	return false;
}
