enum LangC_NodeKind
{
	LangC_NodeKind_
} typedef LangC_NodeKind;

struct LangC_Node typedef LangC_Node;
struct LangC_Node
{
	LangC_NodeKind kind;
	LangC_Node* next;
	int32 line, col;
	const char* source_path;
	
	uint32 flags;
	
};

internal LangC_Node*
LangC_ParseFile(const char* path)
{
	LangC_Lexer ctx = { 0 };
	
	LangC_DefineMacro(&ctx, Str("__STDC__ 1"));
	LangC_DefineMacro(&ctx, Str("__STDC_HOSTED__ 1"));
	LangC_DefineMacro(&ctx, Str("__STDC_VERSION__ 199901L"));
	LangC_DefineMacro(&ctx, Str("__x86_64 1"));
	LangC_DefineMacro(&ctx, Str("_WIN32 1"));
	LangC_DefineMacro(&ctx, Str("_WIN64 1"));
	
	if (LangC_InitLexerFile(&ctx.file, path) < 0)
		return NULL;
	
	while (LangC_NextToken(&ctx), ctx.token.kind != LangC_TokenKind_Eof)
	{
		switch (ctx.token.kind)
		{
			case LangC_TokenKind_Identifier: Print("%.*s (ident)\n", StrFmt(ctx.token.value_ident)); break;
			case LangC_TokenKind_StringLiteral: Print("\"%.*s\"\n", StrFmt(ctx.token.value_str)); break;
			default: Print("%s\n", LangC_token_str_table[ctx.token.kind]); break;
		}
	}
	
	return NULL;
}
