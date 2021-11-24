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
LangC_PoisonNode(LangC_Context* ctx, LangC_Node* node)
{
	if (node->flags & LangC_NodeFlags_Poisoned)
		return;
	
	node->flags |= LangC_NodeFlags_Poisoned;
	
	for (int32 i = 0; i < ArrayLength(node->leafs); ++i)
	{
		if (node->leafs[i])
			LangC_PoisonNode(ctx, node->leafs[i]);
	}
}

internal void
LangC_NodeError(LangC_Context* ctx, LangC_Node* node, const char* fmt, ...)
{
	// NOTE(ljre): If this node is already fucked up, no need to report more errors.
	if (node->flags & LangC_NodeFlags_Poisoned)
		return;
	
	LangC_error_count++;
	
	node->flags |= LangC_NodeFlags_Poisoned;
	LangC_LexerFile* lexfile = node->lexfile;
	
	Print("\n");
	if (lexfile->included_from)
		LangC_PrintIncludeStack(lexfile->included_from, lexfile->included_line);
	
	Print("%s%.*s%s(%i:%i): %serror%s: ", LangC_colors.paths, StrFmt(lexfile->path), LangC_colors.reset,
		  node->line, node->col, LangC_colors.error, LangC_colors.reset);
	
	va_list args;
	va_start(args, fmt);
	PrintVarargs(fmt, args);
	va_end(args);
	Print("\n");
}

internal void
LangC_NodeWarning(LangC_Context* ctx, LangC_Node* node, LangC_Warning warning, const char* fmt, ...)
{
	if (!LangC_IsWarningEnabled(ctx, warning))
		return;
	
	LangC_LexerFile* lexfile = node->lexfile;
	char* buf = Arena_End(global_arena);
	
	Arena_PushMemory(global_arena, 1, "\n");
	if (lexfile->included_from)
		LangC_PrintIncludeStackToArena(lexfile->included_from, lexfile->included_line, global_arena);
	
	Arena_Printf(global_arena, "%s%.*s%s(%i:%i): %swarning%s: ",
				 LangC_colors.paths, StrFmt(node->lexfile->path), LangC_colors.reset,
				 node->line, node->col, LangC_colors.warning, LangC_colors.reset);
	
	va_list args;
	va_start(args, fmt);
	Arena_VPrintf(global_arena, fmt, args);
	va_end(args);
	
	Arena_PushMemory(global_arena, 1, "");
	
	LangC_PushWarning(ctx, warning, buf);
}

internal bool32
LangC_IsBaseType(LangC_NodeKind kind)
{
	return kind >= LangC_NodeKind_TypeBase__First && kind <= LangC_NodeKind_TypeBase__Last;
}

internal LangC_Node* LangC_ParseStmt(LangC_Context* ctx, LangC_Node** out_last, bool32 allow_decl);
internal LangC_Node* LangC_ParseBlock(LangC_Context* ctx, LangC_Node** out_last);
internal LangC_Node* LangC_ParseExpr(LangC_Context* ctx, int32 level, bool32 allow_init);
internal LangC_Node* LangC_ParseDecl(LangC_Context* ctx, LangC_Node** out_last, int32 options, bool32* out_should_eat_semicolon);
internal LangC_Node* LangC_ParseDeclOrExpr(LangC_Context* ctx, LangC_Node** out_last, bool32 type_only, int32 level);
internal LangC_Node* LangC_ParseDeclAndSemicolonIfNeeded(LangC_Context* ctx, LangC_Node** out_last, int32 options);

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
	LangC_Node* result = LangC_CreateNode(ctx, LangC_NodeKind_TypeBaseEnumEntry);
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
		LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_TypeBaseEnumEntry);
		
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
	bool32 should_eat_semicolon = false;
	LangC_Node* last;
	LangC_Node* result = LangC_ParseDecl(ctx, &last, 4, &should_eat_semicolon);
	
	// NOTE(ljre): Bitfields
	if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Colon))
	{
		LangC_Node* attrib = LangC_CreateNode(ctx, LangC_NodeKind_AttributeBitfield);
		attrib->expr = LangC_ParseExpr(ctx, 1, false);
		attrib->next = result->attributes;
		result->attributes = attrib;
	}
	
	if (should_eat_semicolon)
	{
		should_eat_semicolon = false;
		LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
	}
	
	while (!LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_RightCurl))
	{
		LangC_Node* new_last;
		last->next = LangC_ParseDecl(ctx, &new_last, 4, &should_eat_semicolon);
		
		// NOTE(ljre): Bitfields
		if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Colon))
		{
			LangC_Node* attrib = LangC_CreateNode(ctx, LangC_NodeKind_AttributeBitfield);
			attrib->expr = LangC_ParseExpr(ctx, 1, false);
			attrib->next = last->attributes;
			last->attributes = attrib;
		}
		
		if (should_eat_semicolon)
		{
			should_eat_semicolon = false;
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
		}
		
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
		result = LangC_ParseDeclAndSemicolonIfNeeded(ctx, out_last, type_only);
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
				LangC_Node* newnode = LangC_CreateNode(ctx, LangC_NodeKind_ExprInitializerMember);
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
				LangC_Node* newnode = LangC_CreateNode(ctx, LangC_NodeKind_ExprInitializerIndex);
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
	LangC_Node* result = LangC_CreateNode(ctx, LangC_NodeKind_ExprFactor);
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
				head->kind = LangC_NodeKind_Expr1Negative;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_ExprFactor);
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Mul:
			{
				head->kind = LangC_NodeKind_Expr1Deref;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_ExprFactor);
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_And:
			{
				head->kind = LangC_NodeKind_Expr1Ref;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_ExprFactor);
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Not:
			{
				head->kind = LangC_NodeKind_Expr1Not;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_ExprFactor);
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_LNot:
			{
				head->kind = LangC_NodeKind_Expr1LogicalNot;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_ExprFactor);
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Inc:
			{
				head->kind = LangC_NodeKind_Expr1PrefixInc;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_ExprFactor);
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Dec:
			{
				head->kind = LangC_NodeKind_Expr1PrefixDec;
				head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_ExprFactor);
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Sizeof:
			{
				head->kind = LangC_NodeKind_Expr1Sizeof;
				
				LangC_NextToken(&ctx->lex);
				if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_LeftParen))
				{
					if (LangC_IsBeginningOfDeclOrType(ctx))
					{
						head->kind = LangC_NodeKind_Expr1SizeofType;
						head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_ExprFactor);
						
						LangC_UpdateNode(ctx, LangC_NodeKind_Decl, head);
						head->type = LangC_ParseDeclAndSemicolonIfNeeded(ctx, NULL, 1);
					}
					else
					{
						head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_ExprFactor);
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
					LangC_Node* type = LangC_ParseDeclAndSemicolonIfNeeded(ctx, NULL, 1);
					
					LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
					if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_LeftCurl))
					{
						LangC_UpdateNode(ctx, 0, head);
						head->kind = LangC_NodeKind_ExprCompoundLiteral;
						head->type = type;
						
						goto parse_init;
					}
					else
					{
						head->kind = LangC_NodeKind_Expr1Cast;
						head->type = type;
						head = head->expr = LangC_CreateNode(ctx, LangC_NodeKind_ExprFactor);
					}
				}
				else
				{
					// yes. this is a "leak"
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
		head->kind = LangC_NodeKind_ExprInitializer;
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
				LangC_UpdateNode(ctx, LangC_NodeKind_ExprInt, head);
				head->value_int = ctx->lex.token.value_int;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_LIntLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_ExprLInt, head);
				head->value_int = ctx->lex.token.value_int;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_LLIntLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_ExprLLInt, head);
				head->value_int = ctx->lex.token.value_int;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_UintLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_ExprUInt, head);
				head->value_uint = ctx->lex.token.value_uint;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_LUintLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_ExprULInt, head);
				head->value_uint = ctx->lex.token.value_uint;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_LLUintLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_ExprULLInt, head);
				head->value_uint = ctx->lex.token.value_uint;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_StringLiteral:
			case LangC_TokenKind_WideStringLiteral:
			{
				bool32 is_wide;
				
				LangC_UpdateNode(ctx, LangC_NodeKind_ExprString, head);
				head->value_str = LangC_PrepareStringLiteral(ctx, &is_wide);
				
				if (is_wide)
					head->kind = LangC_NodeKind_ExprWideString;
			} break;
			
			case LangC_TokenKind_FloatLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_ExprFloat, head);
				head->value_float = ctx->lex.token.value_float;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_DoubleLiteral:
			{
				LangC_UpdateNode(ctx, LangC_NodeKind_ExprDouble, head);
				head->value_double = ctx->lex.token.value_double;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_Identifier:
			{
				if (LangC_IsBeginningOfDeclOrType(ctx))
				{
					LangC_LexerError(&ctx->lex, "unexpected typename.");
				}
				
				LangC_UpdateNode(ctx, LangC_NodeKind_ExprIdent, head);
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
	
	bool32 need_to_change_result;
	for (;; need_to_change_result ? (result = head) : 0)
	{
		need_to_change_result = (result == head);
		
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_Inc:
			{
				LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_Expr1PostfixInc);
				new_node->expr = head;
				head = new_node;
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Dec:
			{
				LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_Expr1PostfixDec);
				new_node->expr = head;
				head = new_node;
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_LeftParen:
			{
				LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_Expr2Call);
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
				LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_Expr2Index);
				new_node->left = head;
				head = new_node;
				
				LangC_NextToken(&ctx->lex);
				head->right = LangC_ParseExpr(ctx, 0, false);
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightBrkt);
			} continue;
			
			case LangC_TokenKind_Dot:
			{
				LangC_NodeKind kind = LangC_NodeKind_Expr2Access;
				
				if (0)
				{
					case LangC_TokenKind_Arrow: kind = LangC_NodeKind_Expr2DerefAccess;
				}
				
				LangC_Node* new_node = LangC_CreateNode(ctx, kind);
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
			
			LangC_Node* tmp = LangC_CreateNode(ctx, 0);
			
			if (lookahead == LangC_TokenKind_QuestionMark) {
				tmp->kind = LangC_NodeKind_Expr3Condition;
				tmp->left = right;
				tmp->middle = LangC_ParseExpr(ctx, 1, false);
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_Colon);
				tmp->right = LangC_ParseExpr(ctx, level + 1, false);
			} else {
				tmp->kind = LangC_token_to_op[lookahead];
				tmp->left = right;
				tmp->right = LangC_ParseExpr(ctx, level + 1, false);
			}
			
			lookahead = ctx->lex.token.kind;
			lookahead_prec = LangC_operators_precedence[lookahead];
			right = tmp;
		}
		
		LangC_Node* tmp = LangC_CreateNode(ctx, op);
		
		if (op == LangC_NodeKind_Expr3Condition)
		{
			tmp->left = result;
			tmp->middle = right;
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Colon);
			tmp->right = LangC_ParseExpr(ctx, prec.level, false);
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
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_StmtIf);
			
			LangC_NextToken(&ctx->lex);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			result->expr = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			result->stmt = LangC_ParseStmt(ctx, NULL, false);
			
			if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Else))
			{
				result->stmt2 = LangC_ParseStmt(ctx, NULL, false);
			}
		} break;
		
		case LangC_TokenKind_While:
		{
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_StmtWhile);
			
			LangC_NextToken(&ctx->lex);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			result->expr = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			result->stmt = LangC_ParseStmt(ctx, NULL, false);
		} break;
		
		case LangC_TokenKind_For:
		{
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_StmtFor);
			
			LangC_NextToken(&ctx->lex);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			if (ctx->lex.token.kind != LangC_TokenKind_Semicolon)
				result->init = LangC_ParseDeclOrExpr(ctx, NULL, false, 0);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
			if (ctx->lex.token.kind != LangC_TokenKind_Semicolon)
			{
				result->expr = LangC_ParseExpr(ctx, 0, false);
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
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_StmtDoWhile);
			
			LangC_NextToken(&ctx->lex);
			result->stmt = LangC_ParseStmt(ctx, NULL, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_While);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			result->expr = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
		} break;
		
		case LangC_TokenKind_Semicolon:
		{
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_StmtEmpty);
			
			LangC_NextToken(&ctx->lex);
		} break;
		
		case LangC_TokenKind_Switch:
		{
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_StmtSwitch);
			
			LangC_NextToken(&ctx->lex);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			result->expr = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			result->stmt = LangC_ParseStmt(ctx, NULL, false);
		} break;
		
		case LangC_TokenKind_Return:
		{
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_StmtReturn);
			
			LangC_NextToken(&ctx->lex);
			result->expr = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
		} break;
		
		case LangC_TokenKind_Case:
		{
			result = LangC_CreateNode(ctx, LangC_NodeKind_StmtCase);
			
			LangC_NextToken(&ctx->lex);
			result->expr = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Colon);
			result->stmt = LangC_ParseStmt(ctx, NULL, false);
		} break;
		
		case LangC_TokenKind_Goto:
		{
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_StmtGoto);
			
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
			last = result = LangC_CreateNode(ctx, LangC_NodeKind_StmtEmpty);
			
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
				result = LangC_ParseDeclAndSemicolonIfNeeded(ctx, &last, 4);
			}
			else
			{
				last = result = LangC_ParseExpr(ctx, 0, false);
				
				if (result->kind == LangC_NodeKind_ExprIdent && LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Colon))
				{
					result->kind = LangC_NodeKind_StmtLabel;
					result->stmt = LangC_ParseStmt(ctx, NULL, false);
				}
				else
				{
					LangC_Node* s = LangC_CreateNode(ctx, LangC_NodeKind_StmtExpr);
					s->expr = result;
					last = result = s;
					
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
	LangC_Node* result = LangC_CreateNode(ctx, LangC_NodeKind_StmtCompound);
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
	LangC_Node* paren_stack[64];
	paren_stack[0] = NULL;
	uintsize paren_stack_size = 0;
	
	for (;;)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_Mul:
			{
				LangC_NextToken(&ctx->lex);
				
				LangC_Node* newtype = LangC_CreateNode(ctx, LangC_NodeKind_TypePointer);
				newtype->type = head->type;
				head->type = newtype;
			} continue;
			
			case LangC_TokenKind_Const:
			{
				LangC_NextToken(&ctx->lex);
				
				head->type->flags |= LangC_NodeFlags_Const;
			} continue;
			
			case LangC_TokenKind_Restrict:
			{
				LangC_NextToken(&ctx->lex);
				
				head->type->flags |= LangC_NodeFlags_Restrict;
			} continue;
			
			case LangC_TokenKind_LeftParen:
			{
				LangC_NextToken(&ctx->lex);
				Assert(paren_stack_size < ArrayLength(paren_stack));
				
				paren_stack[paren_stack_size++] = head->type;
			} continue;
		}
		
		break;
	}
	
	if (!type_only && ctx->lex.token.kind == LangC_TokenKind_Identifier)
	{
		decl->name = ctx->lex.token.value_ident;
		LangC_NextToken(&ctx->lex);
	}
	
	LangC_Node* previous_head = head;
	head = head->type;
	
	for (;;)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_LeftParen:
			{
				LangC_NextToken(&ctx->lex);
				LangC_Node* newtype = LangC_CreateNode(ctx, LangC_NodeKind_TypeFunction);
				
				// NOTE(ljre): pointer magic.
				newtype->type = head;
				previous_head->type = newtype;
				previous_head = head;
				head = newtype;
				
				if (ctx->lex.token.kind == LangC_TokenKind_RightParen)
				{
					newtype->type->flags |= LangC_NodeFlags_VarArgs;
				}
				else
				{
					LangC_Node* last_param;
					newtype->params = LangC_ParseDecl(ctx, &last_param, 8, NULL);
					
					while (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Comma))
					{
						if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_VarArgs))
						{
							newtype->flags |= LangC_NodeFlags_VarArgs;
							break;
						}
						
						LangC_Node* new_last_param;
						last_param->next = LangC_ParseDecl(ctx, &new_last_param, 8, NULL);
						last_param = new_last_param;
					}
				}
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			} continue;
			
			case LangC_TokenKind_LeftBrkt:
			{
				LangC_NextToken(&ctx->lex);
				LangC_Node* newtype = LangC_CreateNode(ctx, LangC_NodeKind_TypeArray);
				
				// NOTE(ljre): more pointer magic.
				newtype->type = head;
				previous_head->type = newtype;
				previous_head = head;
				head = newtype;
				
				if (ctx->lex.token.kind != LangC_TokenKind_RightBrkt)
				{
					newtype->expr = LangC_ParseExpr(ctx, 0, false);
				}
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightBrkt);
			} continue;
			
			case LangC_TokenKind_RightParen:
			{
				if (paren_stack_size > 0)
				{
					LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
					
					head = paren_stack[--paren_stack_size];
					
					while (previous_head->type != head)
					{
						previous_head = previous_head->type;
						Assert(previous_head);
					}
					
					continue;
				}
			} /* fallthrough */
		}
		
		break;
	}
	
	if (paren_stack_size > 0)
	{
		LangC_LexerError(&ctx->lex, "expected closing ')' for declarator.");
	}
	
	return result->type; // ignore dummy node
}

// NOTE(ljre): 'options': bitset
//                 1 - type only;
//                 2 - is global;
//                 4 - allow multiple;
//                 8 - decay;
//
// NOTE(ljre): IMPORTANT - 'out_should_eat_semicolon' is only assigned when it should be true and if
//                         'options & 4' is set, so you should initialize it to 'false' before calling
//                         this function.
internal LangC_Node*
LangC_ParseDecl(LangC_Context* ctx, LangC_Node** out_last, int32 options, bool32* out_should_eat_semicolon)
{
	Assert(!(options & 4) || out_should_eat_semicolon);
	
	LangC_Node* decl = NULL;
	if (!(options & 1))
		decl = LangC_CreateNode(ctx, LangC_NodeKind_Decl);
	
	LangC_Node* base = LangC_CreateNode(ctx, LangC_NodeKind_Type);
	
	beginning:;
	// Parse Base Type
	for (;;)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_Auto:
			{
				if (options & 2)
				{
					LangC_LexerError(&ctx->lex, "cannot use automatic storage for global object.");
				}
				
				if (options & 1)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->kind != LangC_NodeKind_Decl)
				{
					LangC_LexerError(&ctx->lex, "invalid use of multiple storage modifiers.");
				}
				else
				{
					decl->kind = LangC_NodeKind_DeclAuto;
				}
			} break;
			
			case LangC_TokenKind_Char:
			{
				if (base->kind != LangC_NodeKind_Type)
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type; did you miss a semicolon?");
				}
				else
				{
					base->kind |= LangC_NodeKind_TypeBaseChar;
				}
			} break;
			
			case LangC_TokenKind_Const:
			{
				if (base->flags & LangC_NodeFlags_Const)
				{
					LangC_LexerError(&ctx->lex, "too much constness.");
				}
				
				base->flags |= LangC_NodeFlags_Const;
			} break;
			
			case LangC_TokenKind_Double:
			{
				if (base->kind != LangC_NodeKind_Type)
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				else
				{
					base->kind = LangC_NodeKind_TypeBaseDouble;
				}
			} break;
			
			case LangC_TokenKind_Extern:
			{
				if (options & 1)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->kind != LangC_NodeKind_Decl)
				{
					LangC_LexerError(&ctx->lex, "invalid use of multiple storage modifiers.");
				}
				else
				{
					decl->flags |= LangC_NodeKind_DeclExtern;
				}
			} break;
			
			case LangC_TokenKind_Enum:
			{
				if (base->kind != LangC_NodeKind_Type)
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				
				base->kind = LangC_NodeKind_TypeBaseEnum;
				
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
				if (base->kind != LangC_NodeKind_Type)
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				else
				{
					base->kind = LangC_NodeKind_TypeBaseFloat;
				}
			} break;
			
			case LangC_TokenKind_Int:
			{
				if (base->kind != LangC_NodeKind_Type)
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				else
				{
					base->kind = LangC_NodeKind_TypeBaseInt;
				}
			} break;
			
			case LangC_TokenKind_Inline:
			{
				if (options & 1)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->flags & LangC_NodeFlags_Inline)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'inline'.");
				}
				
				decl->flags |= LangC_NodeFlags_Inline;
			} break;
			
			case LangC_TokenKind_Long:
			{
				if (base->flags & LangC_NodeFlags_LongLong)
				{
					LangC_LexerError(&ctx->lex, "too long for me.");
				}
				else if (base->flags & LangC_NodeFlags_Short)
				{
					LangC_LexerError(&ctx->lex, "'long' does not work with 'short'.");
				}
				else if (base->flags & LangC_NodeFlags_Long)
				{
					// combination of long and short flags is long long
					base->flags &= ~LangC_NodeFlags_Long;
					base->flags |= LangC_NodeFlags_LongLong;
				}
				else
				{
					base->flags |= LangC_NodeFlags_Long;
				}
			} break;
			
			case LangC_TokenKind_Register:
			{
				if (options & 1)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->kind != LangC_NodeKind_Decl)
				{
					LangC_LexerError(&ctx->lex, "invalid use of multiple storage modifiers.");
				}
				else
				{
					decl->flags |= LangC_NodeKind_DeclRegister;
				}
			} break;
			
			case LangC_TokenKind_Restrict:
			{
				if (base->flags & LangC_NodeFlags_Restrict)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'restrict'.");
				}
				
				base->flags |= LangC_NodeFlags_Restrict;
			} break;
			
			case LangC_TokenKind_Short:
			{
				if (base->flags & (LangC_NodeFlags_Long | LangC_NodeFlags_LongLong))
				{
					LangC_LexerError(&ctx->lex, "'short' does not work with 'long'.");
				}
				else if (base->flags & LangC_NodeFlags_Short)
				{
					LangC_LexerError(&ctx->lex, "too short for me.\n");
				}
				else
				{
					base->flags |= LangC_NodeFlags_Short;
				}
			} break;
			
			case LangC_TokenKind_Signed:
			{
				if (base->flags & LangC_NodeFlags_Signed)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'signed'.");
				}
				else if (base->flags & LangC_NodeFlags_Unsigned)
				{
					LangC_LexerError(&ctx->lex, "'signed' does not work with 'unsigned'.");
				}
				else
				{
					base->flags |= LangC_NodeFlags_Signed;
				}
			} break;
			
			case LangC_TokenKind_Static:
			{
				if (options & 1)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->kind != LangC_NodeKind_Decl)
				{
					LangC_LexerError(&ctx->lex, "invalid use of multiple storage modifiers.");
				}
				else
				{
					decl->kind = LangC_NodeKind_DeclStatic;
				}
			} break;
			
			case LangC_TokenKind_Struct:
			{
				if (base->kind != LangC_NodeKind_Type)
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				
				base->kind = LangC_NodeKind_TypeBaseStruct;
				
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
				if (options & 1)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->kind != LangC_NodeKind_Decl)
				{
					LangC_LexerError(&ctx->lex, "invalid use of multiple storage modifiers.");
				}
				else
				{
					decl->kind = LangC_NodeKind_DeclTypedef;
				}
			} break;
			
			case LangC_TokenKind_Union:
			{
				if (base->kind != LangC_NodeKind_Type)
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				
				base->kind = LangC_NodeKind_TypeBaseUnion;
				
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
				if (base->flags & LangC_NodeFlags_Unsigned)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'unsigned'.");
				}
				else if (base->flags & LangC_NodeFlags_Signed)
				{
					LangC_LexerError(&ctx->lex, "'unsigned' does not work with 'signed'.");
				}
				else
				{
					base->flags |= LangC_NodeFlags_Unsigned;
				}
			} break;
			
			case LangC_TokenKind_Void:
			{
				if (base->kind != LangC_NodeKind_Type)
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				else
				{
					base->kind |= LangC_NodeKind_TypeBaseVoid;
				}
			} break;
			
			case LangC_TokenKind_Volatile:
			{
				if (base->flags & LangC_NodeFlags_Volatile)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'volatile'.");
				}
				
				base->flags |= LangC_NodeFlags_Volatile;
			} break;
			
			case LangC_TokenKind_Bool:
			{
				if (base->kind != LangC_NodeKind_Type)
				{
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				}
				else
				{
					base->kind = LangC_NodeKind_TypeBaseBool;
				}
			} break;
			
			case LangC_TokenKind_Complex:
			{
				if (base->flags & LangC_NodeFlags_Complex)
				{
					LangC_LexerError(&ctx->lex, "repeated use of keyword '_Complex'.");
				}
				
				base->flags |= LangC_NodeFlags_Complex;
			} break;
			
			case LangC_TokenKind_Identifier:
			{
				if (LangC_IsBeginningOfDeclOrType(ctx))
				{
					if (base->kind != LangC_NodeKind_Type)
					{
						LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
					}
					
					base->kind = LangC_NodeKind_TypeBaseTypename;
					base->name = ctx->lex.token.value_ident;
					break;
				}
				else if (options & 1)
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
	if (base->kind == LangC_NodeKind_Type)
	{
		if (base->flags & (LangC_NodeFlags_Unsigned |
						   LangC_NodeFlags_Signed |
						   LangC_NodeFlags_Short |
						   LangC_NodeFlags_Long |
						   LangC_NodeFlags_LongLong))
		{
			base->kind = LangC_NodeKind_TypeBaseInt;
		}
		else
		{
			implicit_int = true;
		}
	}
	
	right_before_parsing_rest_of_decl:;
	LangC_Node* type = LangC_ParseRestOfDecl(ctx, base, decl, options & 1, options & 2);
	
	if (implicit_int)
	{
		bool32 reported = false;
		
		if (type->kind >= LangC_NodeKind_TypeBase__First && type->kind <= LangC_NodeKind_TypeBase__Last)
		{
			if (options & 1)
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
			if (type->kind == LangC_NodeKind_TypeFunction)
				LangC_LexerWarning(&ctx->lex, LangC_Warning_ImplicitInt, "implicit return type 'int'.");
			else
				LangC_LexerWarning(&ctx->lex, LangC_Warning_ImplicitInt, "implicit type 'int'.");
		}
		
		base->kind = LangC_NodeKind_TypeBaseInt;
	}
	
	if (options & 8)
	{
		if (type->kind == LangC_NodeKind_TypeArray)
		{
			type->kind = LangC_NodeKind_TypePointer;
			type->flags |= LangC_NodeFlags_Decayed;
		}
		else if (type->kind == LangC_NodeKind_TypeFunction)
		{
			LangC_Node* new_node = LangC_CreateNode(ctx, LangC_NodeKind_TypePointer);
			new_node->type = type;
			type = new_node;
			
			type->flags |= LangC_NodeFlags_Decayed;
		}
	}
	
	if (options & 1)
		return type;
	
	// This is a declaration!
	LangC_Node* last = decl;
	decl->type = type;
	
	if (decl->kind == LangC_NodeKind_DeclTypedef)
		LangC_DefineTypeNode(ctx, decl);
	
	if (ctx->lex.token.kind == LangC_TokenKind_LeftCurl)
	{
		if (type->kind != LangC_NodeKind_TypeFunction)
		{
			LangC_LexerError(&ctx->lex, "invalid block for non-function declaration.");
		}
		
		if (!(options & 2))
		{
			LangC_LexerError(&ctx->lex, "function definitions are only allowed in global scope.");
		}
		
		decl->body = LangC_ParseBlock(ctx, NULL);
	}
	else if (options & 4)
	{
		if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Assign))
		{
			if (decl->kind == LangC_NodeKind_DeclTypedef)
				LangC_LexerError(&ctx->lex, "cannot assign to types.");
			else if (decl->kind == LangC_NodeKind_DeclExtern)
				LangC_LexerError(&ctx->lex, "cannot initialize a declaration with 'extern' storage modifier.");
			
			decl->expr = LangC_ParseExpr(ctx, 1, true);
		}
		
		while (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Comma))
		{
			LangC_Node* new_node = LangC_CreateNode(ctx, decl->kind);
			new_node->type = LangC_ParseRestOfDecl(ctx, base, new_node, false, options & 2);
			last = last->next = new_node;
			
			if (decl->kind == LangC_NodeKind_DeclTypedef)
			{
				LangC_DefineTypeNode(ctx, last);
			}
			
			if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Assign))
			{
				if (decl->kind == LangC_NodeKind_DeclTypedef)
					LangC_LexerError(&ctx->lex, "cannot assign to types.");
				else if (decl->kind == LangC_NodeKind_DeclExtern)
					LangC_LexerError(&ctx->lex, "cannot initialize a declaration with 'extern' storage modifier.");
				
				new_node->expr = LangC_ParseExpr(ctx, 1, true);
			}
		}
		
		*out_should_eat_semicolon = true;
	}
	
	if (out_last)
		*out_last = last;
	
	return decl;
}

internal LangC_Node*
LangC_ParseDeclAndSemicolonIfNeeded(LangC_Context* ctx, LangC_Node** out_last, int32 options)
{
	bool32 b = false;
	LangC_Node* result = LangC_ParseDecl(ctx, out_last, options, &b);
	
	if (b)
		LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
	
	return result;
}

internal bool32
LangC_ParseFile(LangC_Context* ctx)
{
	Trace();
	
	LangC_Node* first_node = NULL;
	LangC_Node* last_node;
	
	LangC_SetupLexer(&ctx->lex, ctx->pre_source, ctx->persistent_arena);
	ctx->lex.ctx = ctx;
	
#if 1
	LangC_NextToken(&ctx->lex);
	
	while (ctx->lex.token.kind == LangC_TokenKind_Semicolon)
		LangC_NextToken(&ctx->lex);
	first_node = LangC_ParseDeclAndSemicolonIfNeeded(ctx, &last_node, 2 | 4);
	
	while (ctx->lex.token.kind != LangC_TokenKind_Eof)
	{
		while (ctx->lex.token.kind == LangC_TokenKind_Semicolon)
			LangC_NextToken(&ctx->lex);
		
		LangC_Node* new_last = NULL;
		last_node->next = LangC_ParseDeclAndSemicolonIfNeeded(ctx, &new_last, 2 | 4);
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
