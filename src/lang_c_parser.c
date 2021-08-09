enum LangC_NodeKind
{
	LangC_NodeKind_Null = 0, // special
	LangC_NodeKind_BaseType, // flags = LangC_Node_BaseType_*
	LangC_NodeKind_FunctionType, // flags = LangC_Node_FunctionType_*
	LangC_NodeKind_PointerType, // flags = 
	LangC_NodeKind_ArrayType, // flags = 
	LangC_NodeKind_Ident,
	LangC_NodeKind_Decl,
	LangC_NodeKind_EmptyStmt,
	LangC_NodeKind_ExprStmt,
	LangC_NodeKind_IfStmt,
	LangC_NodeKind_DoWhileStmt,
	LangC_NodeKind_WhileStmt,
	LangC_NodeKind_ForStmt,
	LangC_NodeKind_SwitchStmt,
	LangC_NodeKind_ReturnStmt,
	LangC_NodeKind_Expr,
	LangC_NodeKind_IntConstant,
	LangC_NodeKind_LIntConstant,
	LangC_NodeKind_LLIntConstant,
	LangC_NodeKind_UintConstant,
	LangC_NodeKind_LUintConstant,
	LangC_NodeKind_LLUintConstant,
	LangC_NodeKind_FloatConstant,
	LangC_NodeKind_DoubleConstant,
	LangC_NodeKind_StringConstant,
	LangC_NodeKind_InitializerEntry,
	LangC_NodeKind_EnumEntry,
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
	LangC_Node_FunctionType_VarArgs = 1,
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

enum
{
	// unary
	LangC_Node_Expr__FirstUnary,
	LangC_Node_Expr_Negative = LangC_Node_Expr__FirstUnary, // -expr
	LangC_Node_Expr_Not, // ~expr
	LangC_Node_Expr_LogicalNot, // !expr
	LangC_Node_Expr_Deref, // *expr
	LangC_Node_Expr_Ref, // &expr
	LangC_Node_Expr_PrefixInc, // ++expr
	LangC_Node_Expr_PrefixDec, // --expr
	LangC_Node_Expr_PostfixInc, // expr++
	LangC_Node_Expr_PostfixDec, // expr--
	LangC_Node_Expr_Sizeof, // sizeof expr // sizeof (type)
	
	// binary
	LangC_Node_Expr__FirstBinary,
	LangC_Node_Expr_Add = LangC_Node_Expr__FirstBinary, // left + right
	LangC_Node_Expr_Sub, // left - right
	LangC_Node_Expr_Mul, // left * right
	LangC_Node_Expr_Div, // left / right
	LangC_Node_Expr_Mod, // left % right
	LangC_Node_Expr_LThan, // left < right
	LangC_Node_Expr_GThan, // left > right
	LangC_Node_Expr_LEqual, // left <= right
	LangC_Node_Expr_GEqual, // left >= right
	LangC_Node_Expr_Equals, // left == right
	LangC_Node_Expr_NotEquals, // left != right
	LangC_Node_Expr_LeftShift, // left << right
	LangC_Node_Expr_RightShift, // left >> right
	LangC_Node_Expr_And, // left & right
	LangC_Node_Expr_Or, // left | right
	LangC_Node_Expr_Xor, // left ^ right
	LangC_Node_Expr_LogicalAnd, // left && right
	LangC_Node_Expr_LogicalOr, // left || right
	LangC_Node_Expr_Assign, // left = right
	LangC_Node_Expr_AssignAdd, // left += right
	LangC_Node_Expr_AssignSub, // left -= right
	LangC_Node_Expr_AssignMul, // left *= right
	LangC_Node_Expr_AssignDiv, // left /= right
	LangC_Node_Expr_AssignMod, // left %= right
	LangC_Node_Expr_AssignLeftShift, // left <<= right
	LangC_Node_Expr_AssignRightShift, // left >>= right
	LangC_Node_Expr_AssignAnd, // left &= right
	LangC_Node_Expr_AssignOr, // left |= right
	LangC_Node_Expr_AssignXor, // left ^= right
	LangC_Node_Expr_Comma, // left, right
	
	// NOTE(ljre): maybe???? cool GCC extension
	//LangC_Node_Expr_Optional, // left ?: right
	
	// special
	LangC_Node_Expr__FirstSpecial,
	LangC_Node_Expr_Call = LangC_Node_Expr__FirstSpecial, // left(right, right->next, ...)
	LangC_Node_Expr_Index, // left[right]
	LangC_Node_Expr_Cast, // (type)expr
	LangC_Node_Expr_Ternary, // condition ? branch1 : branch2
	LangC_Node_Expr_CompoundLiteral, // (type) { init, init->next, ... }
	LangC_Node_Expr_Initializer, // { init, init->next, ... }
	LangC_Node_Expr_Access, // expr.name
	LangC_Node_Expr_DerefAccess, // expr->name
};

enum
{
	LangC_Node_InitializerEntry_JustValue = 0,
	LangC_Node_InitializerEntry_ArrayIndex,
	LangC_Node_InitializerEntry_Field,
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
	// left(right, right->next, ...)
	// *expr
	// expr++
	// condition ? branch1 : branch2
	// (type)expr
	// expr;
	// { stmt stmt->next ... }
	// type name[type->expr];
	// type name = expr;
	// type name(type->params, type->params->next, ...) body
	// sizeof expr
	// struct name body
	// enum { body->name = body->expr, body->next->name, ... }
	LangC_Node* condition;
	LangC_Node* init;
	LangC_Node* iter;
	union
	{
		struct
		{
			LangC_Node* expr;
			LangC_Node* stmt;
		};
		
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
		
		struct
		{
			LangC_Node* params;
			LangC_Node* body;
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

struct LangC_TypeNode typedef LangC_TypeNode;
struct LangC_TypeNode
{
	LangC_TypeNode* next;
	
	uint64 flags; // LangC_Node_BaseType_(Union|Struct|Enum)
	uint64 name_hash;
	String name;
	LangC_Node* decl;
};

struct LangC_Parser typedef LangC_Parser;
struct LangC_Parser
{
	LangC_Lexer lex;
	
	LangC_TypeNode* first_typenode;
	LangC_TypeNode* last_typenode;
	
	// TODO(ljre): Stack of Typenodes. Push stack when entering function scope.
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
	return 0 != (flags & (LangC_Node_BaseType_Char |
						  LangC_Node_BaseType_Int |
						  LangC_Node_BaseType_Float |
						  LangC_Node_BaseType_Double |
						  LangC_Node_BaseType_Typename |
						  LangC_Node_BaseType_Struct |
						  LangC_Node_BaseType_Enum |
						  LangC_Node_BaseType_Union));
}

internal void
LangC_DefineType(LangC_Parser* ctx, LangC_Node* decl)
{
	assert(decl);
	assert(decl->kind == LangC_NodeKind_Decl);
	
	LangC_TypeNode* type = ctx->last_typenode;
	if (!type)
	{
		if (!ctx->first_typenode)
		{
			ctx->first_typenode = PushMemory(sizeof *ctx->first_typenode);
		}
		
		type = ctx->last_typenode = ctx->first_typenode;
	}
	else if (!type->next)
	{
		type = ctx->last_typenode = type->next = PushMemory(sizeof *type->next);
	}
	
	type->flags = decl->flags;
	type->name = decl->name;
	type->name_hash = SimpleHash(type->name);
	type->decl = decl;
}

internal LangC_Node* LangC_ParseStmt(LangC_Parser* ctx, LangC_Node** out_last, bool32 allow_decl);
internal LangC_Node* LangC_ParseBlock(LangC_Parser* ctx, LangC_Node** out_last);
internal LangC_Node* LangC_ParseExpr(LangC_Parser* ctx, int32 level, bool32 allow_init);
internal LangC_Node* LangC_ParseDecl(LangC_Parser* ctx, LangC_Node** out_last, bool32 type_only, bool32 is_global, bool32 allow_multiple, bool32 decay);
internal LangC_Node* LangC_ParseDeclOrExpr(LangC_Parser* ctx, LangC_Node** out_last, bool32 type_only, int32 level);

internal bool32
LangC_IsBeginningOfDeclOrType(LangC_Parser* ctx)
{
	switch (ctx->lex.token.kind)
	{
		case LangC_TokenKind_Identifier:
		{
			String ident = ctx->lex.token.value_ident;
			uint64 search_hash = SimpleHash(ident);
			
			LangC_TypeNode* type = ctx->first_typenode;
			if (type)
			{
				do
				{
					if (type->name_hash == search_hash)
						return true;
				}
				while (type != ctx->last_typenode && (type = type->next));
			}
			
			return false;
		}
		
		case LangC_TokenKind_Int:
		case LangC_TokenKind_Long:
		case LangC_TokenKind_Short:
		case LangC_TokenKind_Char:
		case LangC_TokenKind_Float:
		case LangC_TokenKind_Double:
		case LangC_TokenKind_Const:
		case LangC_TokenKind_Complex:
		case LangC_TokenKind_Bool:
		case LangC_TokenKind_Volatile:
		case LangC_TokenKind_Void:
		case LangC_TokenKind_Unsigned:
		case LangC_TokenKind_Union:
		case LangC_TokenKind_Struct:
		case LangC_TokenKind_Enum:
		case LangC_TokenKind_Signed:
		case LangC_TokenKind_Restrict:
		case LangC_TokenKind_Register:
		
		return true;
	}
	
	return false;
}

internal LangC_Node*
LangC_ParseEnumBody(LangC_Parser* ctx)
{
	LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftCurl);
	LangC_Node* result = LangC_CreateNode(ctx, LangC_NodeKind_EnumEntry);
	LangC_Node* last = result;
	
	if (LangC_AssertToken(&ctx->lex, LangC_TokenKind_Identifier))
	{
		result->name = ctx->lex.token.value_ident;
	}
	
	if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Equals))
	{
		result->expr = LangC_ParseExpr(ctx, 1, false);
	}
	
	while (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Comma) &&
		   ctx->lex.token.kind != LangC_TokenKind_RightCurl)
	{
		LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_EnumEntry);
		
		if (LangC_AssertToken(&ctx->lex, LangC_TokenKind_Identifier))
		{
			new_node->name = ctx->lex.token.value_ident;
		}
		
		if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Equals))
		{
			new_node->expr = LangC_ParseExpr(ctx, 1, false);
		}
		
		last = last->next = new_node;
	}
	
	LangC_EatToken(&ctx->lex, LangC_TokenKind_RightCurl);
	
	return result;
}

internal LangC_Node*
LangC_ParseStructBody(LangC_Parser* ctx)
{
	LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftCurl);
	LangC_Node* last;
	LangC_Node* result = LangC_ParseDecl(ctx, &last, false, false, true, false);
	
	while (!LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_RightCurl))
	{
		LangC_Node* new_last;
		last->next = LangC_ParseDecl(ctx, &new_last, false, false, true, false);
		last = new_last;
	}
	
	return result;
}

internal LangC_Node*
LangC_ParseDeclOrExpr(LangC_Parser* ctx, LangC_Node** out_last, bool32 type_only, int32 level)
{
	LangC_Node* result = NULL;
	
	if (LangC_IsBeginningOfDeclOrType(ctx))
	{
		result = LangC_ParseDecl(ctx, out_last, type_only, false, true, false);
	}
	else
	{
		result = LangC_ParseExpr(ctx, level, false);
	}
	
	return result;
}

internal LangC_Node*
LangC_ParseInitializerField(LangC_Parser* ctx)
{
	// TODO(ljre): designated initializers
	return LangC_ParseExpr(ctx, 1, true);
}

internal String
LangC_PrepareStringLiteral(LangC_Parser* ctx)
{
	String str;
	char* buffer = NULL;
	
	do
	{
		str = ctx->lex.token.value_str;
		SB_PushArray(buffer, str.size, str.data);
		LangC_NextToken(&ctx->lex);
	}
	while (ctx->lex.token.kind == LangC_TokenKind_StringLiteral);
	
	str.data = buffer;
	str.size = SB_Len(buffer);
	
	return str;
}

internal LangC_Node*
LangC_ParseExprFactor(LangC_Parser* ctx, bool32 allow_init)
{
	LangC_Node* result = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
	LangC_Node* head = result;
	bool32 has_unary = false;
	bool32 dont_parse_postfix = true;
	
	for (;; has_unary = true)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_Plus: LangC_NextToken(&ctx->lex); continue; // do nothing????
			
			case LangC_TokenKind_Minus:
			{
				head->flags = LangC_Node_Expr_Negative;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Mul:
			{
				head->flags = LangC_Node_Expr_Deref;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_And:
			{
				head->flags = LangC_Node_Expr_Ref;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Not:
			{
				head->flags = LangC_Node_Expr_Not;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_LNot:
			{
				head->flags = LangC_Node_Expr_LogicalNot;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Inc:
			{
				head->flags = LangC_Node_Expr_PrefixInc;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Dec:
			{
				head->flags = LangC_Node_Expr_PrefixDec;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Sizeof:
			{
				head->flags = LangC_Node_Expr_Sizeof;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
				
				LangC_NextToken(&ctx->lex);
				if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_LeftParen))
				{
					if (LangC_IsBeginningOfDeclOrType(ctx))
					{
						LangC_UpdateNode(ctx, LangC_NodeKind_Decl, head);
						head->type = LangC_ParseDecl(ctx, NULL, true, false, false, false);
					}
					else
					{
						LangC_UpdateNode(ctx, 0, head);
						head->expr = LangC_ParseExpr(ctx, 0, false);
					}
					
					LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
					goto ignore_factor;
				}
			} continue;
			
			case LangC_TokenKind_LeftParen:
			{
				LangC_NextToken(&ctx->lex);
				if (LangC_IsBeginningOfDeclOrType(ctx))
				{
					LangC_Node* type = LangC_ParseDecl(ctx, NULL, true, false, false, false);
					
					LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
					if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_LeftCurl))
					{
						LangC_UpdateNode(ctx, 0, head);
						head->flags = LangC_Node_Expr_CompoundLiteral;
						head->type = type;
						
						goto parse_init;
					}
					else
					{
						head->flags = LangC_Node_Expr_Cast;
						head->type = type;
						head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
					}
				}
				else
				{
					// yes. this is a leak
					*head = *LangC_ParseExpr(ctx, 0, false);
					LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
					goto ignore_factor;
				}
			} continue;
		}
		
		break;
	}
	
	if (allow_init && ctx->lex.token.kind == LangC_TokenKind_LeftCurl)
	{
		LangC_UpdateNode(ctx, 0, head);
		head->flags = LangC_Node_Expr_Initializer;
		if (has_unary)
			LangC_LexerError(&ctx->lex, "expected expression.");
		
		LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftCurl);
		
		if (0)
		{
			parse_init: dont_parse_postfix = false;
		}
		
		head->init = LangC_ParseInitializerField(ctx);
		LangC_Node* last_init = head->init;
		
		while (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Comma) &&
			   ctx->lex.token.kind != LangC_TokenKind_RightCurl)
		{
			last_init = last_init->next = LangC_ParseInitializerField(ctx);
		}
		
		LangC_EatToken(&ctx->lex, LangC_TokenKind_RightCurl);
	}
	else
	{
		dont_parse_postfix = false;
		
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_IntLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_IntConstant, head);
				head->value_int = ctx->lex.token.value_int;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_LIntLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_LIntConstant, head);
				head->value_int = ctx->lex.token.value_int;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_LLIntLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_LLIntConstant, head);
				head->value_int = ctx->lex.token.value_int;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_UintLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_UintConstant, head);
				head->value_uint = ctx->lex.token.value_uint;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_LUintLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_LUintConstant, head);
				head->value_uint = ctx->lex.token.value_uint;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_LLUintLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_LLUintConstant, head);
				head->value_uint = ctx->lex.token.value_uint;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_StringLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_StringConstant, head);
				head->value_str = LangC_PrepareStringLiteral(ctx);
			} break;
			
			case LangC_TokenKind_FloatLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_FloatConstant, head);
				head->value_float = ctx->lex.token.value_float;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_DoubleLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_DoubleConstant, head);
				head->value_double = ctx->lex.token.value_double;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_Identifier:
			{
				if (LangC_IsBeginningOfDeclOrType(ctx))
				{
					LangC_LexerError(&ctx->lex, "unexpected typename.");
				}
				
				LangC_UpdateNode(ctx, LangC_NodeKind_Ident, head);
				head->name = ctx->lex.token.value_ident;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			default:
			{
				LangC_LexerError(&ctx->lex, "expected expression.");
			} break;
		}
	}
	
	ignore_factor:;
	if (dont_parse_postfix)
		return result;
	
	for (;;)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_Inc:
			{
				LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
				new_node->flags = LangC_Node_Expr_PostfixInc;
				head = head->expr = new_node;
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Dec:
			{
				LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
				new_node->flags = LangC_Node_Expr_PostfixDec;
				new_node->expr = head;
				head = new_node;
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_LeftParen:
			{
				LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
				new_node->flags = LangC_Node_Expr_Call;
				new_node->left = head;
				head = new_node;
				
				LangC_NextToken(&ctx->lex);
				if (ctx->lex.token.kind != LangC_TokenKind_RightParen)
				{
					LangC_Node* last_param = LangC_ParseExpr(ctx, 1, false);
					head->right = last_param;
					
					while (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Comma))
					{
						last_param = last_param->next = LangC_ParseExpr(ctx, 1, false);
					}
				}
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			} continue;
			
			case LangC_TokenKind_LeftBrkt:
			{
				LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
				new_node->flags = LangC_Node_Expr_Index;
				new_node->left = head;
				head = new_node;
				
				LangC_NextToken(&ctx->lex);
				head->right = LangC_ParseExpr(ctx, 0, false);
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightBrkt);
			} continue;
			
			case LangC_TokenKind_Dot:
			{
				int32 flag = LangC_Node_Expr_Access;
				
				if (0)
				{
					case LangC_TokenKind_Arrow: flag = LangC_Node_Expr_DerefAccess;
				}
				
				LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
				new_node->flags = LangC_Node_Expr_Access;
				new_node->expr = head;
				head = new_node;
				
				LangC_NextToken(&ctx->lex);
				if (LangC_AssertToken(&ctx->lex, LangC_TokenKind_Identifier))
				{
					head->name = ctx->lex.token.value_ident;
				}
			} continue;
		}
		
		break;
	}
	
	return result;
}

struct LangC_OperatorPrecedence typedef LangC_OperatorPrecedence;
struct LangC_OperatorPrecedence
{
	// from lower to higher
	int16 level; // if <= 0, it's not a binary operator
	bool16 right2left;
	int32 op;
};

internal LangC_OperatorPrecedence LangC_operators_precedence[] = {
	[LangC_TokenKind_Comma] = { 1, false, LangC_Node_Expr_Comma },
	
	[LangC_TokenKind_Assign] = { 2, true, LangC_Node_Expr_Assign },
	[LangC_TokenKind_PlusAssign] = { 2, true, LangC_Node_Expr_AssignAdd },
	[LangC_TokenKind_MinusAssign] = { 2, true, LangC_Node_Expr_AssignSub },
	[LangC_TokenKind_MulAssign] = { 2, true, LangC_Node_Expr_AssignMul },
	[LangC_TokenKind_DivAssign] = { 2, true, LangC_Node_Expr_AssignDiv },
	[LangC_TokenKind_LeftShiftAssign] = { 2, true, LangC_Node_Expr_AssignLeftShift },
	[LangC_TokenKind_RightShiftAssign] = { 2, true, LangC_Node_Expr_AssignRightShift },
	[LangC_TokenKind_AndAssign] = { 2, true, LangC_Node_Expr_AssignAnd },
	[LangC_TokenKind_OrAssign] = { 2, true, LangC_Node_Expr_AssignOr },
	[LangC_TokenKind_XorAssign] = { 2, true, LangC_Node_Expr_AssignXor },
	
	[LangC_TokenKind_QuestionMark] = { 3, true, LangC_Node_Expr_Ternary },
	
	[LangC_TokenKind_LOr] = { 3, false, LangC_Node_Expr_LogicalOr },
	[LangC_TokenKind_LAnd] = { 4, false, LangC_Node_Expr_LogicalAnd },
	[LangC_TokenKind_Or] = { 5, false, LangC_Node_Expr_Or },
	[LangC_TokenKind_Xor] = { 6, false, LangC_Node_Expr_Xor },
	[LangC_TokenKind_And] = { 7, false, LangC_Node_Expr_And },
	
	[LangC_TokenKind_Equals] = { 8, false, LangC_Node_Expr_Equals },
	[LangC_TokenKind_NotEquals] = { 8, false, LangC_Node_Expr_NotEquals },
	
	[LangC_TokenKind_LThan] = { 9, false, LangC_Node_Expr_LThan },
	[LangC_TokenKind_GThan] = { 9, false, LangC_Node_Expr_GThan },
	[LangC_TokenKind_LEqual] = { 9, false, LangC_Node_Expr_LEqual },
	[LangC_TokenKind_GEqual] = { 9, false, LangC_Node_Expr_GEqual },
	
	[LangC_TokenKind_LeftShift] = { 10, false, LangC_Node_Expr_LeftShift },
	[LangC_TokenKind_RightShift] = { 10, false, LangC_Node_Expr_RightShift },
	
	[LangC_TokenKind_Plus] = { 11, false, LangC_Node_Expr_Add },
	[LangC_TokenKind_Minus] = { 11, false, LangC_Node_Expr_Sub },
	
	[LangC_TokenKind_Mul] = { 12, false, LangC_Node_Expr_Mul },
	[LangC_TokenKind_Div] = { 12, false, LangC_Node_Expr_Div },
	[LangC_TokenKind_Mod] = { 12, false, LangC_Node_Expr_Mod },
};

internal LangC_Node*
LangC_ParseExpr(LangC_Parser* ctx, int32 level, bool32 allow_init)
{
	LangC_Node* result = LangC_ParseExprFactor(ctx, allow_init);
	
	LangC_OperatorPrecedence prec;
	while (prec = LangC_operators_precedence[ctx->lex.token.kind],
		   prec.level > level)
	{
		int32 op = prec.op;
		LangC_NextToken(&ctx->lex);
		LangC_Node* right = LangC_ParseExprFactor(ctx, false);
		
		LangC_TokenKind lookahead = ctx->lex.token.kind;
		LangC_OperatorPrecedence lookahead_prec = LangC_operators_precedence[lookahead];
		
		while (lookahead_prec.level > 0 &&
			   (lookahead_prec.level > prec.level ||
				(lookahead_prec.level == prec.level && lookahead_prec.right2left))) {
			LangC_NextToken(&ctx->lex);
			
			LangC_Node* tmp = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
			
			if (lookahead == LangC_TokenKind_QuestionMark) {
				tmp->flags = LangC_Node_Expr_Ternary;
				tmp->condition = right;
				tmp->branch1 = LangC_ParseExpr(ctx, 1, false);
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_Colon);
				tmp->branch2 = LangC_ParseExpr(ctx, level + 1, false);
			} else {
				tmp->flags = lookahead_prec.op;
				tmp->left = right;
				tmp->right = LangC_ParseExpr(ctx, level + 1, false);
			}
			
			lookahead = ctx->lex.token.kind;
			lookahead_prec = LangC_operators_precedence[lookahead];
			right = tmp;
		}
		
		LangC_Node* tmp = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
		tmp->flags = op;
		tmp->left = result;
		tmp->right = right;
		
		result = tmp;
	}
	
	return result;
}

internal LangC_Node*
LangC_ParseStmt(LangC_Parser* ctx, LangC_Node** out_last, bool32 allow_decl)
{
	LangC_Node* result = NULL;
	LangC_Node* last = NULL;
	
	switch (ctx->lex.token.kind)
	{
		case LangC_TokenKind_LeftCurl:
		{
			result = LangC_ParseBlock(ctx, &last);
		} break;
		
		case LangC_TokenKind_If:
		{
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_IfStmt);
			
			LangC_NextToken(&ctx->lex);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			result->condition = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			result->branch1 = LangC_ParseStmt(ctx, NULL, false);
			
			if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Else))
			{
				result->branch2 = LangC_ParseStmt(ctx, NULL, false);
			}
		} break;
		
		case LangC_TokenKind_While:
		{
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_WhileStmt);
			
			LangC_NextToken(&ctx->lex);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			result->condition = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			result->stmt = LangC_ParseStmt(ctx, NULL, false);
		} break;
		
		case LangC_TokenKind_For:
		{
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_ForStmt);
			
			LangC_NextToken(&ctx->lex);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			if (ctx->lex.token.kind != LangC_TokenKind_Semicolon)
				result->init = LangC_ParseDeclOrExpr(ctx, NULL, false, 0);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
			if (ctx->lex.token.kind != LangC_TokenKind_Semicolon)
			{
				result->condition = LangC_ParseExpr(ctx, 0, false);
			}
			else
			{
				result->condition = LangC_CreateNode(ctx, LangC_NodeKind_IntConstant);
				result->condition->value_int = 1;
			}
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
			if (ctx->lex.token.kind != LangC_TokenKind_RightParen)
			{
				result->iter = LangC_ParseExpr(ctx, 0, false);
			}
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			result->stmt = LangC_ParseStmt(ctx, NULL, false);
		} break;
		
		case LangC_TokenKind_Do:
		{
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_DoWhileStmt);
			
			LangC_NextToken(&ctx->lex);
			result->stmt = LangC_ParseStmt(ctx, NULL, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_While);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			result->condition = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
		} break;
		
		case LangC_TokenKind_Semicolon:
		{
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_EmptyStmt);
			
			LangC_NextToken(&ctx->lex);
		} break;
		
		case LangC_TokenKind_Switch:
		{
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_SwitchStmt);
			
			LangC_NextToken(&ctx->lex);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			result->expr = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			result->stmt = LangC_ParseStmt(ctx, NULL, false);
		} break;
		
		case LangC_TokenKind_Return:
		{
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_ReturnStmt);
			
			LangC_NextToken(&ctx->lex);
			result->expr = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
		} break;
		
		default:
		{
			if (allow_decl)
			{
				result = LangC_ParseDeclOrExpr(ctx, &last, false, 0);
			}
			else
			{
				last = result = LangC_ParseExpr(ctx, 0, false);
				LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
			}
		} break;
	}
	
	if (out_last)
		*out_last = last;
	
	return result;
}

internal LangC_Node*
LangC_ParseBlock(LangC_Parser* ctx, LangC_Node** out_last)
{
	LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftCurl);
	LangC_Node* result;
	LangC_Node* last = NULL;
	
	if (ctx->lex.token.kind == LangC_TokenKind_RightCurl)
	{
		last = result = LangC_CreateNode(ctx, LangC_NodeKind_EmptyStmt);
		LangC_EatToken(&ctx->lex, LangC_TokenKind_RightCurl);
	}
	else
	{
		result = LangC_ParseStmt(ctx, &last, true);
		
		while (!LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_RightCurl))
		{
			LangC_Node* new_last;
			LangC_Node* new_node = LangC_ParseStmt(ctx, &new_last, true);
			last->next = new_node;
			last = new_last;
		}
	}
	
	if (out_last)
		*out_last = last;
	
	return result;
}

internal LangC_Node*
LangC_ParseRestOfDecl(LangC_Parser* ctx, LangC_Node* base, LangC_Node* decl, bool32 type_only, bool32 is_global)
{
	LangC_Node* result = LangC_CreateNode(ctx, 0); // dummy node
	LangC_Node* head = result;
	
	head->type = base;
	
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
					LangC_LexerError(&ctx->lex, "invalid use of 'restrict' with a non-pointer type.");
				}
				
				head->type->kind |= LangC_Node_Restrict;
			} continue;
			
			case LangC_TokenKind_Register:
			{
				LangC_NextToken(&ctx->lex);
				LangC_LexerWarning(&ctx->lex, "'register' is straight up ignored here.");
				
				head->type->kind |= LangC_Node_Register;
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
				LangC_NextToken(&ctx->lex);
				LangC_Node* newtype = LangC_CreateNode(ctx, LangC_NodeKind_FunctionType);
				newtype->type = head->type;
				head->type = newtype;
				head = newtype;
				
				if (ctx->lex.token.kind == LangC_TokenKind_RightParen)
				{
					head->flags |= LangC_Node_FunctionType_VarArgs;
				}
				else if (!LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Void))
				{
					LangC_Node* last_param;
					head->params = LangC_ParseDecl(ctx, &last_param, false, false, false, true);
					
					while (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Comma))
					{
						if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_VarArgs))
						{
							head->flags |= LangC_Node_FunctionType_VarArgs;
							break;
						}
						
						LangC_Node* new_last_param;
						last_param->next = LangC_ParseDecl(ctx, &new_last_param, false, false, false, true);
						last_param = new_last_param;
					}
				}
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
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
					head->expr = LangC_ParseExpr(ctx, 0, false);
				}
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightBrkt);
			} continue;
		}
		
		break;
	}
	
	return result->type; // ignore dummy node. only ->type field matters
}

internal LangC_Node*
LangC_ParseDecl(LangC_Parser* ctx, LangC_Node** out_last, bool32 type_only, bool32 is_global, bool32 allow_multiple, bool32 decay)
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
					LangC_LexerError(&ctx->lex, "cannot use automatic storage for global object.");
				}
				
				if (type_only)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->flags & LangC_Node_Auto)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'auto'.");
				}
				
				decl->flags |= LangC_Node_Auto;
			} break;
			
			case LangC_TokenKind_Char:
			{
				if (base->flags & LangC_Node_BaseType_Char)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'char'.");
				}
				else if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				
				base->flags |= LangC_Node_BaseType_Char;
			} break;
			
			case LangC_TokenKind_Const:
			{
				if (base->flags & LangC_Node_Const)
				{
					LangC_LexerError(&ctx->lex, "too much constness.");
				}
				
				base->flags |= LangC_Node_Const;
			} break;
			
			case LangC_TokenKind_Double:
			{
				if (base->flags & LangC_Node_BaseType_Double)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'double'.");
				}
				else if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				
				base->flags |= LangC_Node_BaseType_Double;
			} break;
			
			case LangC_TokenKind_Extern:
			{
				if (type_only)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->flags & (LangC_Node_Static | LangC_Node_Auto))
				{
					LangC_LexerError(&ctx->lex, "invalid combination of storage modifiers.");
				}
				
				decl->flags |= LangC_Node_Extern;
			} break;
			
			case LangC_TokenKind_Enum:
			{
				if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				
				base->flags |= LangC_Node_BaseType_Enum;
				
				LangC_NextToken(&ctx->lex);
				if (ctx->lex.token.kind == LangC_TokenKind_Identifier)
				{
					base->name = ctx->lex.token.value_ident;
					
					LangC_NextToken(&ctx->lex);
				}
				
				if (ctx->lex.token.kind == LangC_TokenKind_LeftCurl)
					base->body = LangC_ParseEnumBody(ctx);
			} continue;  // ignore the LangC_NextToken() at the end.
			
			case LangC_TokenKind_Float:
			{
				if (base->flags & LangC_Node_BaseType_Float)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'float'.");
				}
				else if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				
				base->flags |= LangC_Node_BaseType_Float;
			} break;
			
			case LangC_TokenKind_Int:
			{
				if (base->flags & LangC_Node_BaseType_Int)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'int'.");
				}
				else if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				
				base->flags |= LangC_Node_BaseType_Int;
			} break;
			
			case LangC_TokenKind_Long:
			{
				if (LangC_Node_BaseType_LongLong == (base->flags & LangC_Node_BaseType_LongLong))
				{
					LangC_LexerError(&ctx->lex, "too long for me.");
				}
				else if (base->flags & LangC_Node_BaseType_Short)
				{
					LangC_LexerError(&ctx->lex, "'long' does not work with 'short'.");
				}
				else if (base->flags & LangC_Node_BaseType_Long)
				{
					// combination of long and short flags is long long
					base->flags |= LangC_Node_BaseType_Short;
				}
				else if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				else
				{
					base->flags |= LangC_Node_BaseType_Long;
				}
			} break;
			
			case LangC_TokenKind_Register:
			{
				LangC_LexerWarning(&ctx->lex, "'register' keyword is straight up ignore here.");
				base->flags |= LangC_Node_Register;
			} break;
			
			case LangC_TokenKind_Restrict:
			{
				if (base->flags & LangC_Node_Restrict)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'restrict'.");
				}
				
				base->flags |= LangC_Node_Restrict;
			} break;
			
			case LangC_TokenKind_Short:
			{
				if (base->flags & LangC_Node_BaseType_Long)
				{
					LangC_LexerError(&ctx->lex, "'short' does not work with 'long'.");
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
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'signed'.");
				}
				else if (base->flags & LangC_Node_BaseType_Unsigned)
				{
					LangC_LexerError(&ctx->lex, "'signed' does not work with 'unsigned'.");
				}
				
				base->flags |= LangC_Node_BaseType_Signed;
			} break;
			
			case LangC_TokenKind_Static:
			{
				if (type_only)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->flags & (LangC_Node_Auto | LangC_Node_Extern))
				{
					LangC_LexerError(&ctx->lex, "invalid combination of storage modifiers.");
				}
				else if (decl->flags & (LangC_Node_Static))
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'static'.");
				}
				
				decl->flags |= LangC_Node_Static;
			} break;
			
			case LangC_TokenKind_Struct:
			{
				if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				
				base->flags |= LangC_Node_BaseType_Struct;
				
				LangC_NextToken(&ctx->lex);
				if (ctx->lex.token.kind == LangC_TokenKind_Identifier)
				{
					base->name = ctx->lex.token.value_ident;
					
					LangC_NextToken(&ctx->lex);
				}
				
				if (ctx->lex.token.kind == LangC_TokenKind_LeftCurl)
					base->body = LangC_ParseStructBody(ctx);
			} continue;  // ignore the LangC_NextToken() at the end.
			
			case LangC_TokenKind_Typedef:
			{
				if (type_only)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->flags & LangC_Node_Typedef)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'typedef'.");
				}
				
				decl->flags |= LangC_Node_Typedef;
			} break;
			
			case LangC_TokenKind_Union:
			{
				if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				
				base->flags |= LangC_Node_BaseType_Union;
				
				LangC_NextToken(&ctx->lex);
				if (ctx->lex.token.kind == LangC_TokenKind_Identifier)
				{
					base->name = ctx->lex.token.value_ident;
					
					LangC_NextToken(&ctx->lex);
				}
				
				if (ctx->lex.token.kind == LangC_TokenKind_LeftCurl)
					base->body = LangC_ParseStructBody(ctx);
			} continue; // ignore the LangC_NextToken() at the end.
			
			case LangC_TokenKind_Unsigned:
			{
				if (base->flags & LangC_Node_BaseType_Unsigned)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'unsigned'.");
				}
				else if (base->flags & LangC_Node_BaseType_Signed)
				{
					LangC_LexerError(&ctx->lex, "'unsigned' does not work with 'signed'.");
				}
				
				base->flags |= LangC_Node_BaseType_Unsigned;
			} break;
			
			case LangC_TokenKind_Void:
			{
				if (base->flags & LangC_Node_BaseType_Void)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'void'.");
				}
				else if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				
				base->flags |= LangC_Node_BaseType_Void;
			} break;
			
			case LangC_TokenKind_Volatile:
			{
				if (base->flags & LangC_Node_Volatile)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'volatile'.");
				}
				
				base->flags |= LangC_Node_Volatile;
			} break;
			
			case LangC_TokenKind_Bool:
			{
				if (base->flags & LangC_Node_BaseType_Bool)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword '_Bool'.");
				}
				else if (LangC_BaseTypeFlagsHasType(base->flags))
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				
				base->flags |= LangC_Node_BaseType_Bool;
			} break;
			
			case LangC_TokenKind_Complex:
			{
				if (base->flags & LangC_Node_BaseType_Complex)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword '_Complex'.");
				}
				
				base->flags |= LangC_Node_BaseType_Complex;
			} break;
			
			case LangC_TokenKind_Identifier:
			{
				if (LangC_IsBeginningOfDeclOrType(ctx))
				{
					if (LangC_BaseTypeFlagsHasType(base->flags))
					{
						LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
					}
					
					base->flags |= LangC_Node_BaseType_Typename;
					base->name = ctx->lex.token.value_ident;
					break;
				}
			} /* fallthrough */
			
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
			LangC_LexerWarning(&ctx->lex, "implicit type 'int'.");
		}
		
		base->flags = LangC_Node_BaseType_Int;
	}
	else
	{
		// TODO(ljre): validate use of modifiers
	}
	
	LangC_Node* type = LangC_ParseRestOfDecl(ctx, base, decl, type_only, is_global);
	
	if (decay)
	{
		if (type->kind == LangC_NodeKind_ArrayType)
		{
			type->kind = LangC_NodeKind_PointerType;
		}
		else if (type->kind == LangC_NodeKind_FunctionType)
		{
			LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_PointerType);
			new_node->type = type;
			type = new_node;
		}
	}
	
	if (type_only)
		return type;
	
	// This is a declaration!
	LangC_Node* last = decl;
	decl->type = type;
	
	if (decl->flags & LangC_Node_Typedef)
		LangC_DefineType(ctx, decl);
	
	if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Semicolon))
	{
		// ignore others else ifs - TryToEatToken() already has done it's job.
	}
	else if (ctx->lex.token.kind == LangC_TokenKind_LeftCurl)
	{
		if (type->kind != LangC_NodeKind_FunctionType)
		{
			LangC_LexerError(&ctx->lex, "invalid block for non-function declaration.");
		}
		
		if (!is_global)
		{
			LangC_LexerError(&ctx->lex, "function definitions are only allowed in global scope.");
		}
		
		decl->stmt = LangC_ParseBlock(ctx, NULL);
	}
	else
	{
		if (allow_multiple)
		{
			if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Assign))
			{
				if (decl->flags & LangC_Node_Typedef)
					LangC_LexerError(&ctx->lex, "cannot assign to types.");
				
				decl->expr = LangC_ParseExpr(ctx, 1, true);
			}
			
			while (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Comma))
			{
				LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_Decl);
				new_node->type = LangC_ParseRestOfDecl(ctx, base, new_node, false, is_global);
				last = last->next = new_node;
				
				if (decl->flags & LangC_Node_Typedef)
				{
					LangC_DefineType(ctx, last);
				}
				else if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Assign))
				{
					new_node->expr = LangC_ParseExpr(ctx, 1, true);
				}
			}
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
		}
	}
	
	if (out_last)
		*out_last = last;
	
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
	LangC_DefineMacro(&ctx.lex, Str("__OCC__ 1"));
	LangC_DefineMacro(&ctx.lex, Str("__int64 long long"));
	LangC_DefineMacro(&ctx.lex, Str("__int32 int"));
	LangC_DefineMacro(&ctx.lex, Str("__int16 short"));
	LangC_DefineMacro(&ctx.lex, Str("__int8 char"));
	
	if (LangC_InitLexerFile(&ctx.lex.file, path) < 0)
		return NULL;
	
	LangC_NextToken(&ctx.lex);
	
	first_node = LangC_ParseDecl(&ctx, &last_node, false, true, true, false);
	
	while (ctx.lex.token.kind != LangC_TokenKind_Eof)
	{
		LangC_Node* new_last = NULL;
		last_node->next = LangC_ParseDecl(&ctx, &new_last, false, true, true, false);
		last_node = new_last;
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
