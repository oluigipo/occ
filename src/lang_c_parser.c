internal inline bool32
LangC_BaseTypeFlagsHasType(uint64 flags)
{
	return 0 != (flags & (LangC_Node_BaseType_Char |
						  LangC_Node_BaseType_Int |
						  LangC_Node_BaseType_Float |
						  LangC_Node_BaseType_Double |
						  LangC_Node_BaseType_Void |
						  LangC_Node_BaseType_Typename |
						  LangC_Node_BaseType_Struct |
						  LangC_Node_BaseType_Enum |
						  LangC_Node_BaseType_Union));
}

// NOTE(ljre): If 'decl_or_base' is not a declaration, it shall be a base type of a struct, union, or enum.
internal LangC_TypeNode*
LangC_DefineTypeNode(LangC_Context* ctx, LangC_Node* decl)
{
	Assert(decl);
	
	LangC_TypeNode* type = ctx->last_typenode;
	if (!type)
	{
		if (!ctx->first_typenode)
		{
			ctx->first_typenode = Arena_Push(ctx->stage_arena, sizeof *ctx->first_typenode);
		}
		
		type = ctx->last_typenode = ctx->first_typenode;
		type->saved_last = NULL;
		type->previous = NULL;
	}
	else if (!type->next)
	{
		type->next = Arena_Push(ctx->stage_arena, sizeof *type->next);
		type->next->saved_last = type->saved_last;
		type->next->previous = type;
		
		type = ctx->last_typenode = type->next;
	}
	
	type->name = decl->name;
	type->name_hash = SimpleHash(type->name);
	
	return type;
}

internal void
LangC_PushTypeNodeScope(LangC_Context* ctx)
{
	if (ctx->last_typenode)
	{
		ctx->last_typenode->saved_last = ctx->last_typenode;
	}
}

internal void
LangC_PopTypeNodeScope(LangC_Context* ctx)
{
	if (ctx->last_typenode)
	{
		ctx->last_typenode = ctx->last_typenode->saved_last;
		ctx->last_typenode->saved_last = (ctx->last_typenode->previous) ? ctx->last_typenode->previous->saved_last : NULL;
	}
}

internal bool32
LangC_TypeNodeExists(LangC_Context* ctx, String name)
{
	uint64 search_hash = SimpleHash(name);
	
	LangC_TypeNode* type = ctx->last_typenode;
	if (type)
	{
		do
		{
			if (type->name_hash == search_hash)
				return true;
		}
		while (type != ctx->first_typenode && (type = type->previous));
	}
	
	return false;
}

internal inline void
LangC_UpdateNode(LangC_Context* ctx, LangC_NodeKind kind, LangC_Node* result)
{
	Assert(result);
	
	if (kind)
		result->kind = kind;
	result->line = ctx->lex.token.line;
	result->col = ctx->lex.token.col;
	result->lexfile = ctx->lex.file;
	result->leading_spaces = ctx->lex.token.leading_spaces;
}

internal inline LangC_Node*
LangC_CreateNode(LangC_Context* ctx, LangC_NodeKind kind)
{
	LangC_Node* result = Arena_Push(ctx->persistent_arena, sizeof *result);
	
	LangC_UpdateNode(ctx, kind, result);
	
	return result;
}

internal void
LangC_NodeError(LangC_Node* node, const char* fmt, ...)
{
	LangC_error_count++;
	
	node->flags |= LangC_Node_Poisoned;
	LangC_LexerFile* lexfile = node->lexfile;
	
	Print("\n");
	if (lexfile->included_from)
		LangC_PrintIncludeStack(lexfile->included_from, lexfile->included_line);
	
	Print("%.*s(%i:%i): error: ", StrFmt(lexfile->path), node->line, node->col);
	
	va_list args;
	va_start(args, fmt);
	PrintVarargs(fmt, args);
	va_end(args);
	Print("\n");
}

internal void
LangC_NodeWarning(LangC_Node* node, LangC_Warning warning, const char* fmt, ...)
{
	LangC_LexerFile* lexfile = node->lexfile;
	char* buf = Arena_End(global_arena);
	
	Arena_PushMemory(global_arena, 1, "\n");
	if (lexfile->included_from)
		LangC_PrintIncludeStackToArena(lexfile->included_from, lexfile->included_line, global_arena);
	
	Arena_Printf(global_arena, "%.*s(%i:%i): warning: ", StrFmt(node->lexfile->path), node->line, node->col);
	
	va_list args;
	va_start(args, fmt);
	Arena_VPrintf(global_arena, fmt, args);
	va_end(args);
	
	Arena_PushMemory(global_arena, 1, "");
	
	LangC_PushWarning(warning, buf);
}

internal LangC_Node* LangC_ParseStmt(LangC_Context* ctx, LangC_Node** out_last, bool32 allow_decl);
internal LangC_Node* LangC_ParseBlock(LangC_Context* ctx, LangC_Node** out_last);
internal LangC_Node* LangC_ParseExpr(LangC_Context* ctx, int32 level, bool32 allow_init);
internal LangC_Node* LangC_ParseDecl(LangC_Context* ctx, LangC_Node** out_last, bool32 type_only, bool32 is_global, bool32 allow_multiple, bool32 decay);
internal LangC_Node* LangC_ParseDeclOrExpr(LangC_Context* ctx, LangC_Node** out_last, bool32 type_only, int32 level);

internal bool32
LangC_IsBeginningOfDeclOrType(LangC_Context* ctx)
{
	switch (ctx->lex.token.kind)
	{
		case LangC_TokenKind_Identifier:
		{
			String ident = ctx->lex.token.value_ident;
			return LangC_TypeNodeExists(ctx, ident);
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
LangC_ParseEnumBody(LangC_Context* ctx)
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
LangC_ParseStructBody(LangC_Context* ctx)
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
LangC_ParseDeclOrExpr(LangC_Context* ctx, LangC_Node** out_last, bool32 type_only, int32 level)
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
LangC_ParseInitializerField(LangC_Context* ctx)
{
	LangC_Node* result = NULL;
	
	for (;;)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_Dot:
			{
				LangC_Node* newnode = LangC_CreateNode(ctx, LangC_NodeKind_InitializerEntry);
				newnode->flags = LangC_Node_InitializerEntry_Field;
				newnode->left = result;
				
				LangC_NextToken(&ctx->lex);
				if (LangC_AssertToken(&ctx->lex, LangC_TokenKind_Identifier))
				{
					newnode->name = ctx->lex.token.value_ident;
				}
				
				result = newnode;
			} continue;
			
			case LangC_TokenKind_LeftBrkt:
			{
				LangC_Node* newnode = LangC_CreateNode(ctx, LangC_NodeKind_InitializerEntry);
				newnode->flags = LangC_Node_InitializerEntry_ArrayIndex;
				newnode->left = result;
				
				LangC_NextToken(&ctx->lex);
				newnode->middle = LangC_ParseExpr(ctx, 0, false);
				result = newnode;
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightBrkt);
			} continue;
			
			default:
			{
				if (result)
				{
					LangC_EatToken(&ctx->lex, LangC_TokenKind_Assign);
					
					result->right = LangC_ParseExpr(ctx, 1, true);
				}
				else
				{
					result = LangC_ParseExpr(ctx, 1, true);
				}
			} break;
		}
		
		break;
	}
	
	return result;
}

internal String
LangC_PrepareStringLiteral(LangC_Context* ctx, bool32* is_wide)
{
	String str;
	char* buffer = Arena_End(ctx->persistent_arena);
	*is_wide = false;
	
	do
	{
		str = ctx->lex.token.value_str;
		Arena_PushMemory(ctx->persistent_arena, StrFmt(str));
		LangC_NextToken(&ctx->lex);
	}
	while (ctx->lex.token.kind == LangC_TokenKind_StringLiteral ||
		   (ctx->lex.token.kind == LangC_TokenKind_WideStringLiteral && (*is_wide = true)));
	
	str.data = buffer;
	str.size = (char*)Arena_End(ctx->persistent_arena) - buffer;
	
	return str;
}

internal LangC_Node*
LangC_ParseExprFactor(LangC_Context* ctx, bool32 allow_init)
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
			case LangC_TokenKind_WideStringLiteral:
			{
				bool32 is_wide;
				
				LangC_UpdateNode(ctx, LangC_NodeKind_StringConstant, head);
				head->value_str = LangC_PrepareStringLiteral(ctx, &is_wide);
				
				if (is_wide)
					head->kind = LangC_NodeKind_WideStringConstant;
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
				new_node->flags = flag;
				new_node->expr = head;
				head = new_node;
				
				LangC_NextToken(&ctx->lex);
				if (LangC_AssertToken(&ctx->lex, LangC_TokenKind_Identifier))
				{
					head->name = ctx->lex.token.value_ident;
					LangC_NextToken(&ctx->lex);
				}
			} continue;
		}
		
		break;
	}
	
	return result;
}

internal int32 LangC_token_to_op[LangC_TokenKind__Count] = {
	[LangC_TokenKind_Comma] = LangC_Node_Expr_Comma,
	
	[LangC_TokenKind_Assign] = LangC_Node_Expr_Assign,
	[LangC_TokenKind_PlusAssign] = LangC_Node_Expr_AssignAdd,
	[LangC_TokenKind_MinusAssign] = LangC_Node_Expr_AssignSub,
	[LangC_TokenKind_MulAssign] = LangC_Node_Expr_AssignMul,
	[LangC_TokenKind_DivAssign] = LangC_Node_Expr_AssignDiv,
	[LangC_TokenKind_LeftShiftAssign] = LangC_Node_Expr_AssignLeftShift,
	[LangC_TokenKind_RightShiftAssign] = LangC_Node_Expr_AssignRightShift,
	[LangC_TokenKind_AndAssign] = LangC_Node_Expr_AssignAnd,
	[LangC_TokenKind_OrAssign] = LangC_Node_Expr_AssignOr,
	[LangC_TokenKind_XorAssign] = LangC_Node_Expr_AssignXor,
	
	[LangC_TokenKind_QuestionMark] = LangC_Node_Expr_Ternary,
	
	[LangC_TokenKind_LOr] = LangC_Node_Expr_LogicalOr,
	[LangC_TokenKind_LAnd] = LangC_Node_Expr_LogicalAnd,
	[LangC_TokenKind_Or] = LangC_Node_Expr_Or,
	[LangC_TokenKind_Xor] = LangC_Node_Expr_Xor,
	[LangC_TokenKind_And] = LangC_Node_Expr_And,
	
	[LangC_TokenKind_Equals] = LangC_Node_Expr_Equals,
	[LangC_TokenKind_NotEquals] = LangC_Node_Expr_NotEquals,
	
	[LangC_TokenKind_LThan] = LangC_Node_Expr_LThan,
	[LangC_TokenKind_GThan] = LangC_Node_Expr_GThan,
	[LangC_TokenKind_LEqual] = LangC_Node_Expr_LEqual,
	[LangC_TokenKind_GEqual] = LangC_Node_Expr_GEqual,
	
	[LangC_TokenKind_LeftShift] = LangC_Node_Expr_LeftShift,
	[LangC_TokenKind_RightShift] = LangC_Node_Expr_RightShift,
	
	[LangC_TokenKind_Plus] = LangC_Node_Expr_Add,
	[LangC_TokenKind_Minus] = LangC_Node_Expr_Sub,
	
	[LangC_TokenKind_Mul] = LangC_Node_Expr_Mul,
	[LangC_TokenKind_Div] = LangC_Node_Expr_Div,
	[LangC_TokenKind_Mod] = LangC_Node_Expr_Mod,
};

internal LangC_Node*
LangC_ParseExpr(LangC_Context* ctx, int32 level, bool32 allow_init)
{
	LangC_Node* result = LangC_ParseExprFactor(ctx, allow_init);
	
	LangC_OperatorPrecedence prec;
	while (prec = LangC_operators_precedence[ctx->lex.token.kind],
		   prec.level > level)
	{
		int32 op = LangC_token_to_op[ctx->lex.token.kind];
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
				tmp->flags = LangC_token_to_op[lookahead];
				tmp->left = right;
				tmp->right = LangC_ParseExpr(ctx, level + 1, false);
			}
			
			lookahead = ctx->lex.token.kind;
			lookahead_prec = LangC_operators_precedence[lookahead];
			right = tmp;
		}
		
		LangC_Node* tmp = LangC_CreateNode(ctx, LangC_NodeKind_Expr);
		tmp->flags = op;
		
		if (op == LangC_Node_Expr_Ternary)
		{
			tmp->condition = result;
			tmp->branch1 = right;
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Colon);
			tmp->branch2 = LangC_ParseExpr(ctx, prec.level, false);
		}
		else
		{
			tmp->left = result;
			tmp->right = right;
		}
		
		result = tmp;
	}
	
	return result;
}

internal LangC_Node*
LangC_ParseStmt(LangC_Context* ctx, LangC_Node** out_last, bool32 allow_decl)
{
	LangC_Node* result = NULL;
	LangC_Node* last = NULL;
	
	switch (ctx->lex.token.kind)
	{
		case LangC_TokenKind_LeftCurl:
		{
			result = LangC_ParseBlock(ctx, NULL);
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
		
		case LangC_TokenKind_Case:
		{
			result = LangC_CreateNode(ctx, LangC_NodeKind_CaseLabel);
			
			LangC_NextToken(&ctx->lex);
			result->expr = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Colon);
			result->stmt = LangC_ParseStmt(ctx, NULL, false);
		} break;
		
		case LangC_TokenKind_Goto:
		{
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_GotoStmt);
			
			LangC_NextToken(&ctx->lex);
			if (LangC_AssertToken(&ctx->lex, LangC_TokenKind_Identifier))
			{
				result->name = ctx->lex.token.value_ident;
				
				LangC_NextToken(&ctx->lex);
			}
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
		} break;
		
		case LangC_TokenKind_MsvcAsm:
		case LangC_TokenKind_GccAsm:
		{
			// TODO(ljre): Inline assembly
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_EmptyStmt);
			
			LangC_NextToken(&ctx->lex);
			LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Volatile);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			
			while (ctx->lex.token.kind != LangC_TokenKind_RightParen)
				LangC_NextToken(&ctx->lex);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
		} break;
		
		default:
		{
			if (allow_decl && LangC_IsBeginningOfDeclOrType(ctx))
			{
				result = LangC_ParseDecl(ctx, &last, false, false, true, false);
			}
			else
			{
				last = result = LangC_ParseExpr(ctx, 0, false);
				
				if (result->kind == LangC_NodeKind_Ident && LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Colon))
				{
					result->kind = LangC_NodeKind_Label;
					result->stmt = LangC_ParseStmt(ctx, NULL, false);
				}
				else
				{
					LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
				}
			}
		} break;
	}
	
	if (out_last)
		*out_last = last;
	
	return result;
}

internal LangC_Node*
LangC_ParseBlock(LangC_Context* ctx, LangC_Node** out_last)
{
	LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftCurl);
	LangC_Node* result = LangC_CreateNode(ctx, LangC_NodeKind_CompoundStmt);
	LangC_Node* last = NULL;
	
	if (ctx->lex.token.kind == LangC_TokenKind_RightCurl)
	{
		// NOTE(ljre): Not needed
		//result->stmt = LangC_CreateNode(ctx, LangC_NodeKind_EmptyStmt);
		
		LangC_EatToken(&ctx->lex, LangC_TokenKind_RightCurl);
	}
	else
	{
		LangC_PushTypeNodeScope(ctx);
		result->stmt = LangC_ParseStmt(ctx, &last, true);
		
		while (!LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_RightCurl))
		{
			LangC_Node* new_last;
			LangC_Node* new_node = LangC_ParseStmt(ctx, &new_last, true);
			last->next = new_node;
			last = new_last;
		}
		
		LangC_PopTypeNodeScope(ctx);
	}
	
	if (out_last)
		*out_last = result;
	
	return result;
}

internal LangC_Node*
LangC_ParseRestOfDecl(LangC_Context* ctx, LangC_Node* base, LangC_Node* decl, bool32 type_only, bool32 is_global)
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
				
				head->flags |= LangC_Node_Const;
			} continue;
			
			case LangC_TokenKind_Restrict:
			{
				LangC_NextToken(&ctx->lex);
				
				if (head->kind != LangC_NodeKind_PointerType)
				{
					LangC_LexerError(&ctx->lex, "invalid use of 'restrict' with a non-pointer type.");
				}
				
				head->flags |= LangC_Node_Restrict;
			} continue;
			
			case LangC_TokenKind_Register:
			{
				LangC_NextToken(&ctx->lex);
				LangC_LexerWarning(&ctx->lex, LangC_Warning_RegisterIgnored, "'register' is straight up ignored here.");
				
				head->flags |= LangC_Node_Register;
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
				newtype->type = result->type;
				result->type = newtype;
				
				if (ctx->lex.token.kind == LangC_TokenKind_RightParen)
				{
					result->type->flags |= LangC_Node_FunctionType_VarArgs;
				}
				else
				{
					LangC_Node* last_param;
					result->type->params = LangC_ParseDecl(ctx, &last_param, false, false, false, true);
					
					if (result->type->params->kind == LangC_Node_BaseType_Void)
					{
						// makes it easier to check later
						result->type->params = NULL;
					}
					else
					{
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
				}
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			} continue;
			
			case LangC_TokenKind_LeftBrkt:
			{
				LangC_NextToken(&ctx->lex);
				LangC_Node* newtype = LangC_CreateNode(ctx, LangC_NodeKind_ArrayType);
				newtype->type = result->type;
				result->type = newtype;
				
				if (ctx->lex.token.kind != LangC_TokenKind_RightBrkt)
				{
					result->type->expr = LangC_ParseExpr(ctx, 0, false);
				}
				else if (is_global)
				{
					LangC_LexerWarning(&ctx->lex, LangC_Warning_ImplicitLengthOf1, "implicit length of 1 in array.");
					result->type->expr = LangC_CreateNode(ctx, LangC_NodeKind_IntConstant);
					result->type->expr->value_int = 1;
				}
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightBrkt);
			} continue;
		}
		
		break;
	}
	
	return result->type; // ignore dummy node. only ->type field matters
}

internal LangC_Node*
LangC_ParseDecl(LangC_Context* ctx, LangC_Node** out_last, bool32 type_only, bool32 is_global, bool32 allow_multiple, bool32 decay)
{
	LangC_Node* decl = NULL;
	if (!type_only)
		decl = LangC_CreateNode(ctx, LangC_NodeKind_Decl);
	
	LangC_Node* base = LangC_CreateNode(ctx, LangC_NodeKind_BaseType);
	
	beginning:;
	bool32 at_least_one_base = false;
	
	// Parse Base Type
	for (;; at_least_one_base = true)
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
			
			case LangC_TokenKind_Inline:
			{
				if (type_only)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->flags & LangC_Node_Inline)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'inline'.");
				}
				
				decl->flags |= LangC_Node_Inline;
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
				LangC_LexerWarning(&ctx->lex, LangC_Warning_RegisterIgnored, "'register' keyword is straight up ignore here.");
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
				else if (type_only)
				{
					LangC_LexerError(&ctx->lex, "'%.*s' is not a typename", StrFmt(ctx->lex.token.value_ident));
				}
			} /* fallthrough */
			
			default: goto out_of_loop;
		}
		
		LangC_NextToken(&ctx->lex);
	}
	
	out_of_loop:;
	
	bool32 implicit_int = false;
	if (!LangC_BaseTypeFlagsHasType(base->flags))
	{
		if (0 == (base->flags & (LangC_Node_BaseType_Unsigned |
								 LangC_Node_BaseType_Signed |
								 LangC_Node_BaseType_Short |
								 LangC_Node_BaseType_Long)))
		{
			implicit_int = true;
		}
	}
	else
	{
		// TODO(ljre): validate use of modifiers
	}
	
	right_before_parsing_rest_of_decl:;
	LangC_Node* type = LangC_ParseRestOfDecl(ctx, base, decl, type_only, is_global);
	
	if (implicit_int)
	{
		bool32 reported = false;
		
		if (type->kind == LangC_NodeKind_BaseType)
		{
			if (type_only)
			{
				if (ctx->lex.token.kind == LangC_TokenKind_Identifier)
				{
					reported = true;
					String name = ctx->lex.token.value_ident;
					LangC_LexerError(&ctx->lex, "'%.*s' is not a typename", StrFmt(name));
					LangC_NextToken(&ctx->lex);
					
					goto beginning;
				}
			}
			else if (LangC_IsBeginningOfDeclOrType(ctx))
			{
				reported = true;
				LangC_LexerError(&ctx->lex, "'%.*s' is not a typename", StrFmt(decl->name));
				
				goto beginning;
			}
			else if (ctx->lex.token.kind == LangC_TokenKind_Identifier ||
					 ctx->lex.token.kind == LangC_TokenKind_Mul)
			{
				reported = true;
				LangC_LexerError(&ctx->lex, "'%.*s' is not a typename", StrFmt(decl->name));
				
				goto right_before_parsing_rest_of_decl;
			}
		}
		
		if (!reported)
		{
			if (type->kind == LangC_NodeKind_FunctionType)
				LangC_LexerWarning(&ctx->lex, LangC_Warning_ImplicitInt, "implicit return type 'int'.");
			else
				LangC_LexerWarning(&ctx->lex, LangC_Warning_ImplicitInt, "implicit type 'int'.");
		}
		
		base->flags = LangC_Node_BaseType_Int;
	}
	
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
		LangC_DefineTypeNode(ctx, decl);
	
	if (ctx->lex.token.kind == LangC_TokenKind_LeftCurl)
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
					LangC_DefineTypeNode(ctx, last);
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

internal bool32
LangC_ParseFile(LangC_Context* ctx, const char* source)
{
	LangC_Node* first_node = NULL;
	LangC_Node* last_node;
	
	LangC_SetupLexer(&ctx->lex, source, ctx->persistent_arena);
	
#if 1
	LangC_NextToken(&ctx->lex);
	
	while (ctx->lex.token.kind == LangC_TokenKind_Semicolon)
		LangC_NextToken(&ctx->lex);
	first_node = LangC_ParseDecl(ctx, &last_node, false, true, true, false);
	
	while (ctx->lex.token.kind != LangC_TokenKind_Eof)
	{
		while (ctx->lex.token.kind == LangC_TokenKind_Semicolon)
			LangC_NextToken(&ctx->lex);
		
		LangC_Node* new_last = NULL;
		last_node->next = LangC_ParseDecl(ctx, &new_last, false, true, true, false);
		last_node = new_last;
	}
#else
	while (LangC_NextToken(&ctx->lex), ctx->lex.token.kind != LangC_TokenKind_Eof)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_Identifier: Print("%.*s (ident)\n", StrFmt(ctx->lex.token.value_ident)); break;
			case LangC_TokenKind_StringLiteral: Print("\"%.*s\"\n", StrFmt(ctx->lex.token.value_str)); break;
			default: Print("%s\n", LangC_token_str_table[ctx->lex.token.kind]); break;
		}
	}
#endif
	
	ctx->ast = first_node;
	return true;
}
