enum LangC_TokenKind
{
	LangC_TokenKind_Eof = 0,
	
	LangC_TokenKind__FirstKeyword,
	LangC_TokenKind_Auto = LangC_TokenKind__FirstKeyword,
	LangC_TokenKind_Break,
	LangC_TokenKind_Case,
	LangC_TokenKind_Char,
	LangC_TokenKind_Const,
	LangC_TokenKind_Continue,
	LangC_TokenKind_Default,
	LangC_TokenKind_Do,
	LangC_TokenKind_Double,
	LangC_TokenKind_Else,
	LangC_TokenKind_Enum,
	LangC_TokenKind_Extern,
	LangC_TokenKind_Float,
	LangC_TokenKind_For,
	LangC_TokenKind_Goto,
	LangC_TokenKind_If,
	LangC_TokenKind_Inline,
	LangC_TokenKind_Int,
	LangC_TokenKind_Long,
	LangC_TokenKind_Register,
	LangC_TokenKind_Restrict,
	LangC_TokenKind_Return,
	LangC_TokenKind_Short,
	LangC_TokenKind_Signed,
	LangC_TokenKind_Sizeof,
	LangC_TokenKind_Static,
	LangC_TokenKind_Struct,
	LangC_TokenKind_Switch,
	LangC_TokenKind_Typedef,
	LangC_TokenKind_Union,
	LangC_TokenKind_Unsigned,
	LangC_TokenKind_Void,
	LangC_TokenKind_Volatile,
	LangC_TokenKind_While,
	LangC_TokenKind_Bool,
	LangC_TokenKind_Complex,
	LangC_TokenKind__LastKeyword = LangC_TokenKind_Complex,
	
	LangC_TokenKind_IntLiteral,
	LangC_TokenKind_LIntLiteral,
	LangC_TokenKind_LLIntLiteral,
	LangC_TokenKind_UintLiteral,
	LangC_TokenKind_LUintLiteral,
	LangC_TokenKind_LLUintLiteral,
	LangC_TokenKind_StringLiteral,
	LangC_TokenKind_FloatLiteral,
	LangC_TokenKind_DoubleLiteral,
	
	LangC_TokenKind_Identifier,
	
	LangC_TokenKind_LeftParen, // (
	LangC_TokenKind_RightParen, // )
	LangC_TokenKind_LeftBrkt, // [
	LangC_TokenKind_RightBrkt, // ]
	LangC_TokenKind_LeftCurl, // {
	LangC_TokenKind_RightCurl, // }
	LangC_TokenKind_Dot, // .
	LangC_TokenKind_VarArgs, // ...
	LangC_TokenKind_Arrow, // ->
	LangC_TokenKind_Comma, // ,
	LangC_TokenKind_Colon, // :
	LangC_TokenKind_Semicolon, // ;
	LangC_TokenKind_QuestionMark, // ?
	LangC_TokenKind_Plus, // +
	LangC_TokenKind_Minus, // -
	LangC_TokenKind_Mul, // *
	LangC_TokenKind_Div, // /
	LangC_TokenKind_Mod, // %
	LangC_TokenKind_LThan, // <
	LangC_TokenKind_GThan, // >
	LangC_TokenKind_LEqual, // <=
	LangC_TokenKind_GEqual, // >=
	LangC_TokenKind_Equals, // ==
	LangC_TokenKind_Inc, // ++
	LangC_TokenKind_Dec, // --
	LangC_TokenKind_LNot, // !
	LangC_TokenKind_LAnd, // &&
	LangC_TokenKind_LOr, // ||
	LangC_TokenKind_Not, // ~
	LangC_TokenKind_And, // &
	LangC_TokenKind_Or, // |
	LangC_TokenKind_Xor, // ^
	LangC_TokenKind_LeftShift, // <<
	LangC_TokenKind_RightShift, // >>
	LangC_TokenKind_NotEquals, // !=
	LangC_TokenKind_Assign, // =
	LangC_TokenKind_PlusAssign, // +=
	LangC_TokenKind_MinusAssign, // -=
	LangC_TokenKind_MulAssign, // *=
	LangC_TokenKind_DivAssign, // /=
	LangC_TokenKind_ModAssign, // %=
	LangC_TokenKind_LeftShiftAssign, // <<=
	LangC_TokenKind_RightShiftAssign, // >>=
	LangC_TokenKind_AndAssign, // &=
	LangC_TokenKind_OrAssign, // |=
	LangC_TokenKind_XorAssign, // ^=
} typedef LangC_TokenKind;

internal const char* LangC_token_str_table[] = {
	[LangC_TokenKind_Eof] = "(EOF)",
	
	[LangC_TokenKind_Auto] = "auto",
	[LangC_TokenKind_Break] = "break",
	[LangC_TokenKind_Case] = "case",
	[LangC_TokenKind_Char] = "char",
	[LangC_TokenKind_Const] = "const",
	[LangC_TokenKind_Continue] = "continue",
	[LangC_TokenKind_Default] = "default",
	[LangC_TokenKind_Do] = "do",
	[LangC_TokenKind_Double] = "double",
	[LangC_TokenKind_Else] = "else",
	[LangC_TokenKind_Enum] = "enum",
	[LangC_TokenKind_Extern] = "extern",
	[LangC_TokenKind_Float] = "float",
	[LangC_TokenKind_For] = "for",
	[LangC_TokenKind_Goto] = "goto",
	[LangC_TokenKind_If] = "if",
	[LangC_TokenKind_Inline] = "inline",
	[LangC_TokenKind_Int] = "int",
	[LangC_TokenKind_Long] = "long",
	[LangC_TokenKind_Register] = "register",
	[LangC_TokenKind_Restrict] = "restrict",
	[LangC_TokenKind_Return] = "return",
	[LangC_TokenKind_Short] = "short",
	[LangC_TokenKind_Signed] = "signed",
	[LangC_TokenKind_Sizeof] = "sizeof",
	[LangC_TokenKind_Static] = "static",
	[LangC_TokenKind_Struct] = "struct",
	[LangC_TokenKind_Switch] = "switch",
	[LangC_TokenKind_Typedef] = "typedef",
	[LangC_TokenKind_Union] = "union",
	[LangC_TokenKind_Unsigned] = "unsigned",
	[LangC_TokenKind_Void] = "void",
	[LangC_TokenKind_Volatile] = "volatile",
	[LangC_TokenKind_While] = "while",
	[LangC_TokenKind_Bool] = "bool",
	[LangC_TokenKind_Complex] = "complex",
	
	[LangC_TokenKind_IntLiteral] = "(int literal)",
	[LangC_TokenKind_LIntLiteral] = "(long literal)",
	[LangC_TokenKind_LLIntLiteral] = "(long long literal)",
	[LangC_TokenKind_UintLiteral] = "(unsigned literal)",
	[LangC_TokenKind_LUintLiteral] = "(unsigned long literal)",
	[LangC_TokenKind_LLUintLiteral] = "(unsigned long long literal)",
	[LangC_TokenKind_StringLiteral] = "(const char[] literal)",
	[LangC_TokenKind_FloatLiteral] = "(float literal)",
	[LangC_TokenKind_DoubleLiteral] = "(double literal)",
	
	[LangC_TokenKind_Identifier] = "(identifier)",
	
	[LangC_TokenKind_LeftParen] = "(",
	[LangC_TokenKind_RightParen] = ")",
	[LangC_TokenKind_LeftBrkt] = "[",
	[LangC_TokenKind_RightBrkt] = "]",
	[LangC_TokenKind_LeftCurl] = "{",
	[LangC_TokenKind_RightCurl] = "}",
	[LangC_TokenKind_Dot] = ".",
	[LangC_TokenKind_VarArgs] = "...",
	[LangC_TokenKind_Arrow] = "->",
	[LangC_TokenKind_Comma] = ",",
	[LangC_TokenKind_Colon] = ":",
	[LangC_TokenKind_Semicolon] = ";",
	[LangC_TokenKind_QuestionMark] = "?",
	[LangC_TokenKind_Plus] = "+",
	[LangC_TokenKind_Minus] = "-",
	[LangC_TokenKind_Mul] = "*",
	[LangC_TokenKind_Div] = "/",
	[LangC_TokenKind_Mod] = "%",
	[LangC_TokenKind_LThan] = "<",
	[LangC_TokenKind_GThan] = ">",
	[LangC_TokenKind_LEqual] = "<=",
	[LangC_TokenKind_GEqual] = ">=",
	[LangC_TokenKind_Equals] = "==",
	[LangC_TokenKind_Inc] = "++",
	[LangC_TokenKind_Dec] = "--",
	[LangC_TokenKind_LNot] = "!",
	[LangC_TokenKind_LAnd] = "&&",
	[LangC_TokenKind_LOr] = "||",
	[LangC_TokenKind_Not] = "~",
	[LangC_TokenKind_And] = "&",
	[LangC_TokenKind_Or] = "|",
	[LangC_TokenKind_Xor] = "^",
	[LangC_TokenKind_LeftShift] = "<<",
	[LangC_TokenKind_RightShift] = ">>",
	[LangC_TokenKind_NotEquals] = "!=",
	[LangC_TokenKind_Assign] = "=",
	[LangC_TokenKind_PlusAssign] = "+=",
	[LangC_TokenKind_MinusAssign] = "-=",
	[LangC_TokenKind_MulAssign] = "*=",
	[LangC_TokenKind_DivAssign] = "/=",
	[LangC_TokenKind_ModAssign] = "%=",
	[LangC_TokenKind_LeftShiftAssign] = "<<=",
	[LangC_TokenKind_RightShiftAssign] = ">>=",
	[LangC_TokenKind_AndAssign] = "&=",
	[LangC_TokenKind_OrAssign] = "|=",
	[LangC_TokenKind_XorAssign] = "^=",
};

struct LangC_Token
{
	int32 line, col;
	LangC_TokenKind kind;
	
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
	bool32 ifdef_already_matched;
	bool32 error;
	
	LangC_Macro* first_macro;
	LangC_Macro* last_macro;
	
	LangC_LexerMacroContext* macroctx_first;
	LangC_LexerMacroContext* macroctx_last;
};

internal void LangC_NextToken(LangC_Lexer* ctx);

internal int32
LangC_InitLexerFile(LangC_LexerFile* lexfile, const char* path)
{
	//Print("Loading file '%s'\n", path);
	
	lexfile->source = OS_ReadWholeFile(path);
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
LangC_IsNumeric(char ch)
{
	return (ch >= '0' & ch <= '9');
}

internal inline bool32
LangC_IsAlpha(char ch)
{
	return (ch >= 'A' & ch <= 'Z') | (ch >= 'a' & ch <= 'z');
}

internal inline bool32
LangC_IsIdentChar(char ch)
{
	return ch == '_' || LangC_IsNumeric(ch) || LangC_IsAlpha(ch);
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

internal const char*
LangC_CurrentWorkingPath(LangC_Lexer* ctx)
{
	LangC_LexerFile* lexfile = &ctx->file;
	
	while (lexfile->next_included && lexfile->next_included->active)
		lexfile = lexfile->next_included;
	
	return lexfile->path;
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
LangC_ConsumeEofState(LangC_Lexer* ctx)
{
	// TODO
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
	// TODO(ljre): better way?
	
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

internal LangC_Macro*
LangC_FindMacro(LangC_Lexer* ctx, String name, int32 func /* 0 = false, 1 = true, 2 = doesnt matter */)
{
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

internal String
LangC_TokenAsString(LangC_Token* tok)
{
	String result;
	
	switch (tok->kind)
	{
		case LangC_TokenKind_Identifier:
		{
			result = tok->value_ident;
		} break;
		
		default:
		{
			const char* str = LangC_token_str_table[tok->kind];
			uintsize len = strlen(str);
			
			result.data = str;
			result.size = len;
		} break;
	}
	
	return result;
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

// NOTE(ljre): 'phead' may be NULL if 'macro->is_func' is false.
internal void
LangC_PushMacroContext(LangC_Lexer* ctx, const char** phead, LangC_Macro* macro, bool32 expand_args)
{
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
	
	const char* head = macro->def;
	
	// ignore macro name
	LangC_TokenizeIdent(&head);
	
	if (macro->is_func)
	{
		assert(*head == '(');
		++head;
		
		// implementation shouldn't call this function if there's "macro type" mismatch
		assert(**phead == '(');
		++*phead;
		
		if (*head != ')')
		{
			do
			{
				LangC_IgnoreWhitespaces(&head, false);
				LangC_IgnoreWhitespaces(phead, false);
				
				if (*head == '.')
				{
					if (head[1] != '.' || head[2] != '.')
					{
						LangC_LexerError(ctx, "expected '...' for VA_ARGS.\n");
						continue;
					}
					
					head += 3;
					const char name[] = "__VA_ARGS__";
					
					const char* begin = *phead;
					LangC_IgnoreUntilPairOfNestedOr(phead, '(', ')', 0);
					
					const char* end = *phead;
					uintsize size = (uintsize)(end - begin);
					
					uintsize def_len = sizeof(name) + 1 + size + 1;
					char* def_str = PushMemory(def_len);
					snprintf(def_str, def_len, "%s %.*s", name, (int32)size, begin);
					
					String def = {
						.size = def_len,
						.data = def_str,
					};
					
					LangC_DefineMacro(ctx, def);
					break;
				}
				else
				{
					String ident = LangC_TokenizeIdent(&head);
					
					const char* begin = *phead;
					LangC_IgnoreUntilPairOfNestedOr(phead, '(', ')', ',');
					
					if (!**phead)
					{
						LangC_LexerError(ctx, "unexpected end-of-file.\n");
						return;
					}
					
					const char* end = *phead;
					uintsize size = (uintsize)(end - begin);
					
					uintsize def_len = ident.size + 1 + size + 1;
					char* def_str = PushMemory(def_len);
					snprintf(def_str, def_len, "%.*s %.*s", StrFmt(ident), (int32)size, begin);
					
					String def = {
						.size = def_len,
						.data = def_str,
					};
					
					LangC_DefineMacro(ctx, def);
				}
				
				LangC_IgnoreWhitespaces(&head, false);
				LangC_IgnoreWhitespaces(phead, false);
			}
			while (*head == ',' && *(*phead) == ',' && ++head);
			
			if (*head == ',' && **phead != ',')
			{
				LangC_LexerError(ctx, "expected more arguments in function-like macro.");
			}
		}
		
		if (*head != ')')
		{
			LangC_LexerError(ctx, "expected ')' at the end of function-like macro arguments.");
		}
		
		if (*head == ')' && **phead != ')')
		{
			LangC_LexerWarning(ctx, "excessive number of arguments for function-like macro.");
			LangC_IgnoreUntilPairOfNestedOr(phead, '(', ')', 0);
		}
		
		assert(*head == ')');
		++head;
		
		assert(**phead == ')');
		++*phead;
		
		LangC_IgnoreWhitespaces(&head, false);
	}
	
	ctx->macroctx_last->saved_last_macro = ctx->last_macro;
	ctx->macroctx_last->expanding_macro = macro;
	ctx->macroctx_last->head = head;
}

internal String
LangC_ExpandMacro(LangC_Lexer* ctx, LangC_Macro* macro)
{
	char* buf = NULL;
	SB_ReserveAtLeast(buf, 64);
	LangC_LexerMacroContext* end_context = ctx->macroctx_last;
	
	assert(!macro->is_func);
	LangC_PushMacroContext(ctx, NULL, macro, false);
	while (LangC_ConsumeEofState(ctx), ctx->macroctx_last != end_context)
	{
		// TODO(ljre): this isn't right - expanded macro should be raw text instead of tokens
		LangC_NextToken(ctx);
		String str = LangC_TokenAsString(&ctx->token);
		
		SB_PushArray(buf, str.size, str.data);
	}
	
	String result = {
		.size = SB_Len(buf),
		.data = buf,
	};
	
	return result;
}

internal uint8
LangC_ValueOfEscaped(const char** ptr)
{
	uint8 value = 0;
	
	switch (**ptr)
	{
		case '\\': value = '\\'; ++*ptr; break;
		case '\'': value = '\''; ++*ptr; break;
		case '"': value = '"'; ++*ptr; break;
		case '0': value = 0; ++*ptr; break;
		// TODO: everything else
		default: value = **ptr; ++*ptr; break;
	}
	
	return value;
}

internal int32
LangC_EvalPreProcessorExpr(LangC_Lexer* ctx, LangC_LexerFile* lexfile, String expr)
{
	// TODO
	
	return 0;
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
	
	const char* end = begin;
	while (*end && (*end != '\n' || end[-1] == '\\')) ++end;
	
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
	const char* begin = lexfile->head;
	LangC_IgnoreUntilEndOfLine(lexfile);
	const char* end = lexfile->head;
	
	String expr = {
		.size = (uintsize)(end - begin),
		.data = begin,
	};
	
	if (LangC_EvalPreProcessorExpr(ctx, lexfile, expr))
	{
		++ctx->ifdef_nesting;
	}
	else
	{
		++ctx->ifdef_failed_nesting;
	}
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
	switch (**phead)
	{
		case '.':
		{
			if (!LangC_IsNumeric((*phead)[1]))
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
				if ((*phead)[1] == 'x')
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
			
			while (LangC_IsNumeric(*end)) ++end;
			
			if (*end == '.')
			{
				if (base == 16)
					LangC_LexerError(ctx, "error: floats cannot begin with '0x'.");
				else if (base == 2)
					LangC_LexerError(ctx, "error: floats cannot begin with '0b'.");
				
				++end;
				while (LangC_IsNumeric(*end)) ++end;
				
				if (*end == 'f' || *end == 'F')
				{
					++end;
					ctx->token.kind = LangC_TokenKind_FloatLiteral;
					ctx->token.value_float = strtof(begin, NULL);
				}
				else if (*end == 'e' || *end == 'E')
				{
					// TODO(ljre)
				}
				else
				{
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
			(*phead) += eaten;
		} break;
		
		case '"':
		{
			const char* begin = (*phead);
			const char* end = begin + 1;
			
			while (*end)
			{
				if (end[0] == '"' && end[-1] != '\\')
					break;
				if (end[0] == '\n')
					break;
				
				++end;
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
}

internal void
LangC_NextToken(LangC_Lexer* ctx)
{
	beginning:;
	
	if (ctx->macroctx_last != NULL)
	{
		//~ Macro State
		LangC_LexerMacroContext* macroctx = ctx->macroctx_last;
		LangC_IgnoreWhitespaces(&macroctx->head, false);
		
		switch (*macroctx->head)
		{
			case 0: LangC_PopMacroContext(ctx); goto beginning;
			
			case '#': // Stringify
			{
				++macroctx->head;
				
				LangC_IgnoreWhitespaces(&macroctx->head, false);
				String ident = LangC_TokenizeIdent(&macroctx->head);
				
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
			} break;
			
			case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
			case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T':
			case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': case 'a': case 'b': case 'c': case 'd':
			case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': case 'l': case 'm': case 'n':
			case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x':
			case 'y': case 'z': case '_':
			{
				String ident = LangC_TokenizeIdent(&macroctx->head);
				
				// TODO(ljre): Proper macro expansion - what if this macro expands to a function macro?
				LangC_Macro* macro = LangC_FindMacro(ctx, ident, *macroctx->head == '(');
				
				if (macro)
				{
					LangC_PushMacroContext(ctx, &macroctx->head, macro, true);
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
			} break;
			
			default: LangC_TokenizeSimpleTokens(ctx, &macroctx->head); break;
		}
	}
	else
	{
		LangC_LexerFile* lexfile = &ctx->file;
		if (!lexfile->active)
		{
			ctx->token.kind = LangC_TokenKind_Eof;
			return;
		}
		
		while (lexfile->next_included && lexfile->next_included->active)
		{
			lexfile = lexfile->next_included;
		}
		
		LangC_IgnoreWhitespaces(&lexfile->head, true);
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
				case 0: lexfile->active = false; goto beginning;
				
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
						--ctx->ifdef_nesting;
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
				
				case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I':
				case 'J': case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R':
				case 'S': case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': case 'a':
				case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j':
				case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's':
				case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': case '_':
				{
					String ident = LangC_TokenizeIdent(&lexfile->head);
					LangC_Macro* macro = LangC_FindMacro(ctx, ident, *lexfile->head == '(');
					
					if (macro)
					{
						LangC_UpdateFileLineAndCol(lexfile);
						LangC_PushMacroContext(ctx, &lexfile->head, macro, false);
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
				} break;
				
				default: LangC_TokenizeSimpleTokens(ctx, &lexfile->head); break;
			}
		}
	}
}

internal void
LangC_EatToken(LangC_Lexer* ctx, LangC_TokenKind kind)
{
	if (ctx->token.kind != kind)
	{
		String got = LangC_TokenAsString(&ctx->token);
		LangC_LexerError(ctx, "expected '%s', but got '%.*s'.", LangC_token_str_table[kind], StrFmt(got));
	}
	
	LangC_NextToken(ctx);
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
		String got = LangC_TokenAsString(&ctx->token);
		LangC_LexerError(ctx, "expected '%s', but got '%.*s'.", LangC_token_str_table[kind], StrFmt(got));
		
		return false;
	}
	
	return true;
}
