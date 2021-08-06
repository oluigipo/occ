enum LangC_NodeKind
{
	LangC_NodeKind_BaseType, // flags = LangC_Node_BaseType_*
	LangC_NodeKind_FunctionType, // flags = 
	LangC_NodeKind_PointerType, // flags = 
	LangC_NodeKind_ArrayType, // flags = 
	LangC_NodeKind_Ident,
	LangC_NodeKind_Decl,
	LangC_NodeKind_EmptyStmt,
	LangC_NodeKind_ExprStmt,
	LangC_NodeKind_IfStmt,
	LangC_NodeKind_WhileStmt,
	LangC_NodeKind_ForStmt,
	LangC_NodeKind_ReturnStmt,
} typedef LangC_NodeKind;

// Flags for any kind of node
enum
{
	LangC_Node_Poisoned = 1 << 31,
	LangC_Node_Const = 1 << 30,
	LangC_Node_Volatile = 1 << 29,
	LangC_Node_Register = 1 << 28,
	LangC_Node_Restrict = 1 << 27,
	LangC_Node_Static = 1 << 26,
	LangC_Node_Extern = 1 << 25,
	LangC_Node_Auto = 1 << 24,
	LangC_Node_Typedef = 1 << 23,
};

enum
{
	LangC_Node_BaseType_Char = 1,
	LangC_Node_BaseType_Int = 1 << 1,
	LangC_Node_BaseType_Float = 1 << 2,
	LangC_Node_BaseType_Double = 1 << 3,
	LangC_Node_BaseType_Typename = 1 << 4,
	LangC_Node_BaseType_Struct = 1 << 5,
	LangC_Node_BaseType_Union = 1 << 6,
	LangC_Node_BaseType_Enum = 1 << 7,
	LangC_Node_BaseType_Void = 1 << 8,
	LangC_Node_BaseType_Bool = 1 << 9,
	
	LangC_Node_BaseType_Signed = 1 << 10,
	LangC_Node_BaseType_Unsigned = 1 << 11,
	LangC_Node_BaseType_Long = 1 << 12,
	LangC_Node_BaseType_Short = 1 << 13,
	LangC_Node_BaseType_Complex = 1 << 14,
	
	// combination of Short and Long
	LangC_Node_BaseType_LongLong = LangC_Node_BaseType_Long | LangC_Node_BaseType_Short,
};

struct LangC_Node typedef LangC_Node;
struct LangC_Node
{
	LangC_NodeKind kind;
	LangC_Node* next;
	int32 line, col;
	const char* source_path;
	uint64 flags;
	
	String name;
	LangC_Node* type;
	
	// for (init; condition; iter) branch1
	// if (condition) branch1 else branch2
	// left = right
	// left[right]
	// left(right)
	// *expr
	// expr++
	// condition ? branch1 : branch2
	// (type)expr
	// expr;
	// { stmt }
	// type name[expr];
	LangC_Node* condition;
	LangC_Node* init;
	LangC_Node* iter;
	union
	{
		LangC_Node* expr;
		LangC_Node* stmt;
		
		struct
		{
			LangC_Node* branch1;
			LangC_Node* branch2;
			LangC_Node* branch3;
		};
		
		struct
		{
			LangC_Node* left;
			LangC_Node* right;
			LangC_Node* unnamed_for_now;
		};
	};
	
	union
	{
		int64 value_int;
		uint64 value_uint;
		String value_str;
		float value_float;
		double value_double;
	};
};

struct LangC_Parser typedef LangC_Parser;
struct LangC_Parser
{
	LangC_Lexer lex;
	
	
};

internal void
LangC_UpdateNode(LangC_Parser* ctx, LangC_NodeKind kind, LangC_Node* result)
{
	assert(result);
	
	if (kind)
		result->kind = kind;
	result->line = ctx->lex.token.line;
	result->col = ctx->lex.token.col;
	result->source_path = LangC_CurrentWorkingPath(&ctx->lex);
}

internal LangC_Node*
LangC_CreateNode(LangC_Parser* ctx, LangC_NodeKind kind)
{
	LangC_Node* result = PushMemory(sizeof *result);
	
	LangC_UpdateNode(ctx, kind, result);
	
	return result;
}

internal bool32
LangC_BaseTypeFlagsHasType(uint64 flags)
{
	return 0 != (LangC_Node_BaseType_Char |
				 LangC_Node_BaseType_Int |
				 LangC_Node_BaseType_Float |
				 LangC_Node_BaseType_Double |
				 LangC_Node_BaseType_Typename);
}

internal LangC_Node* LangC_ParseStmt(LangC_Parser* ctx);
internal LangC_Node* LangC_ParseBlock(LangC_Parser* ctx);
internal LangC_Node* LangC_ParseExpr(LangC_Parser* ctx, bool32 allow_comma);
internal LangC_Node* LangC_ParseDecl(LangC_Parser* ctx, bool32 type_only, bool32 global);

internal LangC_Node*
LangC_ParseExpr(LangC_Parser* ctx, bool32 allow_comma)
{
	// TODO
	
	return NULL;
}

internal LangC_Node*
LangC_ParseStmt(LangC_Parser* ctx)
{
	// TODO
	
	return NULL;
}

internal LangC_Node*
LangC_ParseBlock(LangC_Parser* ctx)
{
	LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftCurl);
	LangC_Node* result;
	
	if (ctx->lex.token.kind == LangC_TokenKind_RightCurl)
	{
		result = LangC_CreateNode(ctx, LangC_NodeKind_EmptyStmt);
		LangC_EatToken(&ctx->lex, LangC_TokenKind_RightCurl);
	}
	else
	{
		result = LangC_ParseStmt(ctx);
		LangC_Node* head = result;
		
		while (!LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_RightCurl))
		{
			LangC_Node* new_node = LangC_ParseStmt(ctx);
			head = head->next = new_node;
		}
	}
	
	return result;
}

internal LangC_Node*
LangC_ParseRestOfDecl(LangC_Parser* ctx, LangC_Node* base, LangC_Node* decl, bool32 type_only, bool32 is_global)
{
	LangC_Node* result = base;
	LangC_Node* head = result;
	
	for (;;)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_Mul:
			{
				LangC_NextToken(&ctx->lex);
				
				LangC_Node* newtype = LangC_CreateNode(ctx, LangC_NodeKind_PointerType);
				newtype->type = head->type;
				head->type = newtype;
				head = newtype;
			} continue;
			
			case LangC_TokenKind_Const:
			{
				LangC_NextToken(&ctx->lex);
				
				head->type->flags |= LangC_Node_Const;
			} continue;
			
			case LangC_TokenKind_Restrict:
			{
				LangC_NextToken(&ctx->lex);
				
				if (head->type->kind != LangC_NodeKind_PointerType)
				{
					LangC_LexerError(&ctx->lex, "invalid use of 'restrict' with a non-pointer type.\n");
				}
				
				head->type->kind |= LangC_Node_Restrict;
			} continue;
			
			case LangC_TokenKind_Register:
			{
				LangC_NextToken(&ctx->lex);
				LangC_LexerWarning(&ctx->lex, "'register' is straight up ignored here.\n");
				
				head->type->kind |= LangC_Node_Restrict;
			} continue;
		}
		
		break;
	}
	
	if (ctx->lex.token.kind == LangC_TokenKind_LeftParen)
	{
		LangC_NextToken(&ctx->lex);
		head = LangC_ParseRestOfDecl(ctx, head, decl, type_only, is_global);
		LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
	}
	else if (!type_only && ctx->lex.token.kind == LangC_TokenKind_Identifier)
	{
		decl->name = ctx->lex.token.value_ident;
		LangC_NextToken(&ctx->lex);
	}
	
	for (;;)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_LeftParen:
			{
				// TODO
			} continue;
			
			case LangC_TokenKind_LeftBrkt:
			{
				LangC_NextToken(&ctx->lex);
				
				LangC_Node* newtype = LangC_CreateNode(ctx, LangC_NodeKind_ArrayType);
				newtype->type = head->type;
				head->type = newtype;
				head = newtype;
				
				if (ctx->lex.token.kind != LangC_TokenKind_RightBrkt)
				{
					head->expr = LangC_ParseExpr(ctx, true);
				}
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightBrkt);
			} continue;
		}
		
		break;
	}
	
	return result;
}

internal LangC_Node*
LangC_ParseDecl(LangC_Parser* ctx, bool32 type_only, bool32 is_global)
{
	LangC_Node* decl = NULL;
	if (!type_only)
		decl = LangC_CreateNode(ctx, LangC_NodeKind_Decl);
	
	LangC_Node* base = LangC_CreateNode(ctx, LangC_NodeKind_BaseType);
	
	// Parse Base Type
	for (;;)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_Auto:
			{
				if (is_global)
				{
					LangC_LexerError(&ctx->lex, "cannot use automatic storage for global object.\n");
				}
				
				if (type_only)
				{
					LangC_LexerError(&ctx->lex, "expected a type.\n");
					break;
				}
				
				if (decl->flags & LangC_Node_Auto)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'auto'.\n");
				}
				
				decl->flags |= LangC_Node_Auto;
			} break;
			
			case LangC_TokenKind_Char:
			{
				if (base->flags & LangC_Node_BaseType_Char)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'char'.\n");
				}
				else if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more of one type.\n");
				}
				
				base->flags |= LangC_Node_BaseType_Char;
			} break;
			
			case LangC_TokenKind_Const:
			{
				if (base->flags & LangC_Node_Const)
				{
					LangC_LexerError(&ctx->lex, "too much constness.\n");
				}
				
				base->flags |= LangC_Node_Const;
			} break;
			
			case LangC_TokenKind_Double:
			{
				if (base->flags & LangC_Node_BaseType_Double)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'double'.\n");
				}
				else if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more of one type.\n");
				}
				
				base->flags |= LangC_Node_BaseType_Double;
			} break;
			
			case LangC_TokenKind_Extern:
			{
				if (type_only)
				{
					LangC_LexerError(&ctx->lex, "expected a type.\n");
					break;
				}
				
				if (decl->flags & (LangC_Node_Static | LangC_Node_Auto))
				{
					LangC_LexerError(&ctx->lex, "invalid combination of storage modifiers.\n");
				}
				
				decl->flags |= LangC_Node_Extern;
			} break;
			
			case LangC_TokenKind_Float:
			{
				if (base->flags & LangC_Node_BaseType_Float)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'float'.\n");
				}
				else if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more of one type.\n");
				}
				
				base->flags |= LangC_Node_BaseType_Float;
			} break;
			
			case LangC_TokenKind_Int:
			{
				if (base->flags & LangC_Node_BaseType_Int)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'int'.\n");
				}
				else if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more of one type.\n");
				}
				
				base->flags |= LangC_Node_BaseType_Int;
			} break;
			
			case LangC_TokenKind_Long:
			{
				if (base->flags & (LangC_Node_BaseType_Long | LangC_Node_BaseType_Short))
				{
					LangC_LexerError(&ctx->lex, "too long for me.\n");
				}
				else if (base->flags & LangC_Node_BaseType_Short)
				{
					LangC_LexerError(&ctx->lex, "'long' does not work with 'short'.\n");
				}
				else if (base->flags & LangC_Node_BaseType_Long)
				{
					base->flags |= LangC_Node_BaseType_Short;
				}
				else if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more of one type.\n");
				}
				else
				{
					base->flags |= LangC_Node_BaseType_Long;
				}
			} break;
			
			case LangC_TokenKind_Register:
			{
				LangC_LexerWarning(&ctx->lex, "'register' keyword is straight up ignore here.\n");
				base->flags |= LangC_Node_Register;
			} break;
			
			case LangC_TokenKind_Restrict:
			{
				if (base->flags & LangC_Node_Restrict)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'restrict'.\n");
				}
				
				base->flags |= LangC_Node_Restrict;
			} break;
			
			case LangC_TokenKind_Short:
			{
				if (base->flags & LangC_Node_BaseType_Long)
				{
					LangC_LexerError(&ctx->lex, "'short' does not work with 'long'.\n");
				}
				else if (base->flags & LangC_Node_BaseType_Short)
				{
					LangC_LexerError(&ctx->lex, "too short for me.\n");
				}
				
				base->flags |= LangC_Node_BaseType_Short;
			} break;
			
			case LangC_TokenKind_Signed:
			{
				if (base->flags & LangC_Node_BaseType_Signed)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'signed'.\n");
				}
				else if (base->flags & LangC_Node_BaseType_Unsigned)
				{
					LangC_LexerError(&ctx->lex, "'signed' does not work with 'unsigned'.\n");
				}
				
				base->flags |= LangC_Node_BaseType_Signed;
			} break;
			
			case LangC_TokenKind_Static:
			{
				if (type_only)
				{
					LangC_LexerError(&ctx->lex, "expected a type.\n");
					break;
				}
				
				if (decl->flags & (LangC_Node_Auto | LangC_Node_Extern))
				{
					LangC_LexerError(&ctx->lex, "invalid combination of storage modifiers.\n");
				}
				else if (decl->flags & (LangC_Node_Static))
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'static'.\n");
				}
				
				decl->flags |= LangC_Node_Static;
			} break;
			
			case LangC_TokenKind_Struct:
			{
				if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more of one type.\n");
				}
				
				LangC_NextToken(&ctx->lex);
				
				if (ctx->lex.token.kind != LangC_TokenKind_Identifier)
				{
					LangC_LexerError(&ctx->lex, "expected identifier after 'struct' keyword.\n");
					break;
				}
				
				base->flags |= LangC_Node_BaseType_Struct;
				base->name = ctx->lex.token.value_ident;
			} break;
			
			case LangC_TokenKind_Typedef:
			{
				if (type_only)
				{
					LangC_LexerError(&ctx->lex, "expected a type.\n");
					break;
				}
				
				if (decl->flags & LangC_Node_Typedef)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'typedef'.\n");
				}
				
				decl->flags |= LangC_Node_Typedef;
			} break;
			
			case LangC_TokenKind_Union:
			{
				if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more of one type.\n");
				}
				
				LangC_NextToken(&ctx->lex);
				
				if (ctx->lex.token.kind != LangC_TokenKind_Identifier)
				{
					LangC_LexerError(&ctx->lex, "expected identifier after 'union' keyword.\n");
					break;
				}
				
				base->flags |= LangC_Node_BaseType_Union;
				base->name = ctx->lex.token.value_ident;
			} break;
			
			case LangC_TokenKind_Unsigned:
			{
				if (base->flags & LangC_Node_BaseType_Unsigned)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'unsigned'.\n");
				}
				else if (base->flags & LangC_Node_BaseType_Signed)
				{
					LangC_LexerError(&ctx->lex, "'unsigned' does not work with 'signed'.\n");
				}
				
				base->flags |= LangC_Node_BaseType_Unsigned;
			} break;
			
			case LangC_TokenKind_Void:
			{
				if (base->flags & LangC_Node_BaseType_Void)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'void'.\n");
				}
				else if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more of one type.\n");
				}
				
				base->flags |= LangC_Node_BaseType_Void;
			} break;
			
			case LangC_TokenKind_Volatile:
			{
				if (base->flags & LangC_Node_Volatile)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'volatile'.\n");
				}
				
				base->flags |= LangC_Node_Volatile;
			} break;
			
			case LangC_TokenKind_Bool:
			{
				if (base->flags & LangC_Node_BaseType_Bool)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword '_Bool'.\n");
				}
				else if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more of one type.\n");
				}
				
				base->flags |= LangC_Node_BaseType_Bool;
			} break;
			
			case LangC_TokenKind_Complex:
			{
				if (base->flags & LangC_Node_BaseType_Complex)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword '_Complex'.\n");
				}
				
				base->flags |= LangC_Node_BaseType_Complex;
			} break;
			
			default: goto out_of_loop;
		}
		
		LangC_NextToken(&ctx->lex);
	}
	
	out_of_loop:;
	
	if (!LangC_BaseTypeFlagsHasType(base->flags))
	{
		if (0 == (base->flags & (LangC_Node_BaseType_Unsigned |
								 LangC_Node_BaseType_Signed |
								 LangC_Node_BaseType_Short |
								 LangC_Node_BaseType_Long)))
		{
			LangC_LexerWarning(&ctx->lex, "implicit type 'int'.\n");
		}
		
		base->flags = LangC_Node_BaseType_Int;
	}
	else
	{
		// TODO(ljre): validate use of modifiers
	}
	
	LangC_Node* type = LangC_ParseRestOfDecl(ctx, base, decl, type_only, is_global);
	
	if (type_only)
		return type;
	
	// This is a declaration!
	decl->type = type;
	
	if (ctx->lex.token.kind == LangC_TokenKind_LeftCurl)
	{
		if (type->kind != LangC_NodeKind_FunctionType)
		{
			LangC_LexerError(&ctx->lex, "invalid block for non-function declaration.\n");
		}
		
		if (!is_global)
		{
			LangC_LexerError(&ctx->lex, "function definitions are only allowed in global scope.\n");
		}
		
		decl->stmt = LangC_ParseBlock(ctx);
	}
	else
	{
		if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Assign))
		{
			decl->expr = LangC_ParseExpr(ctx, false);
		}
		
		LangC_Node* head = decl;
		while (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Comma))
		{
			LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_Decl);
			new_node->type = LangC_ParseRestOfDecl(ctx, base, new_node, false, is_global);
			head = head->next = new_node;
			
			if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Assign))
			{
				new_node->expr = LangC_ParseExpr(ctx, false);
			}
		}
		
		LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
	}
	
	return decl;
}

internal LangC_Node*
LangC_ParseFile(const char* path)
{
	LangC_Parser ctx = { 0 };
	LangC_Node* first_node;
	LangC_Node* last_node;
	
	LangC_DefineMacro(&ctx.lex, Str("__STDC__ 1"));
	LangC_DefineMacro(&ctx.lex, Str("__STDC_HOSTED__ 1"));
	LangC_DefineMacro(&ctx.lex, Str("__STDC_VERSION__ 199901L"));
	LangC_DefineMacro(&ctx.lex, Str("__x86_64 1"));
	LangC_DefineMacro(&ctx.lex, Str("_WIN32 1"));
	LangC_DefineMacro(&ctx.lex, Str("_WIN64 1"));
	
	if (LangC_InitLexerFile(&ctx.lex.file, path) < 0)
		return NULL;
	
	LangC_NextToken(&ctx.lex);
	
	first_node = last_node = LangC_ParseDecl(&ctx, false, true);
	
	while (ctx.lex.token.kind != LangC_TokenKind_Eof)
	{
		last_node = last_node->next = LangC_ParseDecl(&ctx, false, true);
	}
	
	/*while (LangC_NextToken(&ctx), ctx.token.kind != LangC_TokenKind_Eof)
	{
		switch (ctx.token.kind)
		{
			case LangC_TokenKind_Identifier: Print("%.*s (ident)\n", StrFmt(ctx.token.value_ident)); break;
			case LangC_TokenKind_StringLiteral: Print("\"%.*s\"\n", StrFmt(ctx.token.value_str)); break;
			default: Print("%s\n", LangC_token_str_table[ctx.token.kind]); break;
		}
	}*/
	
	return first_node;
}
