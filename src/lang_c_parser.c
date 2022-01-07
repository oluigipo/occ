internal inline void
C_UpdateNode(C_Context* ctx, C_AstKind kind, C_AstNode* node)
{
	Assert(node);
	
	if (kind)
		node->kind = kind;
	node->trace = &ctx->token->trace;
}

internal inline C_Node*
C_CreateNode(C_Context* ctx, C_AstKind kind, uintsize size)
{
	C_AstNode* result = Arena_Push(ctx->persistent_arena, size);
	C_UpdateNode(ctx, kind, result);
	
	return result;
}

internal void
C_PoisonNode(C_Context* ctx, C_AstNode* node)
{
	if (node->flags & C_AstFlags_Poisoned)
		return;
	
	node->flags |= C_AstFlags_Poisoned;
	
	// TODO(ljre): Poison children.
}

internal void
C_AppendNode(C_Context* ctx, C_Node** to_, C_Node* node)
{
	C_AstNode** to = (void*)to_;
	
	if (!*to)
		*to = node;
	else
	{
		while ((*to)->next)
			to = &(*to)->next;
		
		*to = node;
	}
}

internal void
C_NodeError(C_Context* ctx, C_Node* node_, const char* fmt, ...)
{
	C_AstNode* node = node_;
	
	// NOTE(ljre): If this node is already fucked up, no need to report more errors.
	if (node->flags & C_AstFlags_Poisoned)
		return;
	
	va_list args;
	va_start(args, fmt);
	C_TraceErrorVarargs(ctx, node->trace, fmt, args);
	va_end(args);
}

internal void
C_NodeWarning(C_Context* ctx, C_Node* node_, C_Warning warning, const char* fmt, ...)
{
	if (!C_IsWarningEnabled(ctx, warning))
		return;
	
	C_AstNode* node = node_;
	
	va_list args;
	va_start(args, fmt);
	C_TraceWarningVarargs(ctx, node->trace, warning, fmt, args);
	va_end(args);
}

internal C_SymbolScope*
C_PushSymbolScope(C_Context* ctx)
{
	C_SymbolScope* scope = Arena_Push(ctx->persistent_arena, sizeof(*scope));
	
	if (ctx->previous_scope)
	{
		ctx->previous_scope->next = scope;
		ctx->previous_scope = NULL;
		ctx->scope->down = scope;
	}
	
	scope->up = ctx->scope;
	ctx->scope = scope;
	
	return scope;
}

internal void
C_PopSymbolScope(C_Context* ctx)
{
	ctx->previous_scope = ctx->scope;
	ctx->scope = ctx->scope->up;
}

internal void
C_TypedefDecl(C_Context* ctx, C_AstDecl* decl)
{
	C_Symbol* sym = Arena_Push(ctx->persistent_arena, sizeof(*sym));
	String name = decl->name;
	
	sym->kind = C_SymbolKind_Typename;
	sym->decl = decl;
	sym->name = name;
	
	if (!ctx->scope->types)
		ctx->scope->types = LittleMap_Create(ctx->persistent_arena, 128);
	
	LittleMap_Insert(ctx->scope->types, name, sym);
}

internal bool32
C_IsBeginningOfDeclOrType(C_Context* ctx)
{
	Trace();
	
	switch (ctx->token->kind)
	{
		case C_TokenKind_Identifier:
		{
			String ident = ctx->token->value_ident;
			uint64 hash = SimpleHash(ident);
			C_SymbolScope* scope = ctx->scope;
			
			while (scope)
			{
				if (scope->types && LittleMap_FetchWithCachedHash(scope->types, ident, hash))
					return true;
				
				scope = scope->up;
			}
			
			return false;
		}
		
		case C_TokenKind_Auto:
		case C_TokenKind_Bool:
		case C_TokenKind_Char:
		case C_TokenKind_Complex:
		case C_TokenKind_Const:
		case C_TokenKind_Double:
		case C_TokenKind_Enum:
		case C_TokenKind_Extern:
		case C_TokenKind_Float:
		case C_TokenKind_Inline:
		case C_TokenKind_Int:
		case C_TokenKind_Long:
		case C_TokenKind_Register:
		case C_TokenKind_Restrict:
		case C_TokenKind_Short:
		case C_TokenKind_Signed:
		case C_TokenKind_Static:
		case C_TokenKind_Struct:
		case C_TokenKind_Typedef:
		case C_TokenKind_Union:
		case C_TokenKind_Unsigned:
		case C_TokenKind_Void:
		case C_TokenKind_Volatile:
		
		return true;
	}
	
	return false;
}

internal C_Node* C_ParseStmt(C_Context* ctx, C_Node** out_last, bool32 allow_decl);
internal C_Node* C_ParseBlock(C_Context* ctx, C_Node** out_last);
internal C_Node* C_ParseExpr(C_Context* ctx, int32 level, bool32 allow_init);
internal C_Node* C_ParseDecl(C_Context* ctx, C_Node** out_last, int32 options, bool32* out_should_eat_semicolon);
internal C_Node* C_ParseDeclOrExpr(C_Context* ctx, C_Node** out_last, bool32 type_only, int32 level);
internal C_Node* C_ParseDeclAndSemicolonIfNeeded(C_Context* ctx, C_Node** out_last, int32 options);
internal bool32 C_ParsePossibleGnuAttribute(C_Context* ctx, C_AstNode* apply_to);

internal C_AstDecl*
C_ParseEnumBody(C_Context* ctx)
{
	Trace();
	
	C_StreamEatToken(ctx, C_TokenKind_LeftCurl);
	C_AstDecl* result = C_CreateNode(ctx, C_AstKind_DeclEnumEntry, sizeof(C_AstDecl));
	C_AstNode* last = &result->h;
	
	if (C_StreamAssertToken(ctx, C_TokenKind_Identifier))
	{
		result->name = ctx->token->value_ident;
		C_StreamNextToken(ctx);
	}
	
	C_ParsePossibleGnuAttribute(ctx, &result->h);
	
	if (C_StreamTryEatToken(ctx, C_TokenKind_Assign))
		result->init = C_ParseExpr(ctx, 1, false);
	
	while (C_StreamTryEatToken(ctx, C_TokenKind_Comma) &&
		   ctx->token->kind != C_TokenKind_RightCurl)
	{
		C_AstDecl* new_node = C_CreateNode(ctx, C_AstKind_DeclEnumEntry, sizeof(C_AstDecl));
		
		if (C_StreamAssertToken(ctx, C_TokenKind_Identifier))
		{
			new_node->name = ctx->token->value_ident;
			C_StreamNextToken(ctx);
		}
		
		C_ParsePossibleGnuAttribute(ctx, &new_node->h);
		
		if (C_StreamTryEatToken(ctx, C_TokenKind_Assign))
			new_node->init = C_ParseExpr(ctx, 1, false);
		
		last = last->next = &new_node->h;
	}
	
	C_StreamEatToken(ctx, C_TokenKind_RightCurl);
	
	return result;
}

internal C_AstDecl*
C_ParseStructBody(C_Context* ctx)
{
	Trace();
	
	C_StreamEatToken(ctx, C_TokenKind_LeftCurl);
	bool32 should_eat_semicolon = false;
	C_AstNode* last;
	C_AstDecl* result = C_ParseDecl(ctx, (void*)&last, 4, &should_eat_semicolon);
	
	// NOTE(ljre): Bitfields
	if (C_StreamTryEatToken(ctx, C_TokenKind_Colon))
	{
		C_AstAttribute* attrib = C_CreateNode(ctx, C_AstKind_AttributeBitfield, SizeofPoly(C_AstAttribute, bitfield));
		attrib->as->bitfield.expr = C_ParseExpr(ctx, 1, false);
		attrib->h.next = (void*)result->h.attributes;
		result->h.attributes = attrib;
	}
	
	if (should_eat_semicolon)
	{
		should_eat_semicolon = false;
		C_StreamEatToken(ctx, C_TokenKind_Semicolon);
	}
	
	while (!C_StreamTryEatToken(ctx, C_TokenKind_RightCurl))
	{
		C_Node* new_last;
		last->next = C_ParseDecl(ctx, &new_last, 4, &should_eat_semicolon);
		
		// NOTE(ljre): Bitfields
		if (C_StreamTryEatToken(ctx, C_TokenKind_Colon))
		{
			C_AstAttribute* attrib = C_CreateNode(ctx, C_AstKind_AttributeBitfield, SizeofPoly(C_AstAttribute, bitfield));
			attrib->as->bitfield.expr = C_ParseExpr(ctx, 1, false);
			attrib->h.next = (void*)last->attributes;
			last->attributes = attrib;
		}
		
		if (should_eat_semicolon)
		{
			should_eat_semicolon = false;
			C_StreamEatToken(ctx, C_TokenKind_Semicolon);
		}
		
		last = new_last;
	}
	
	return result;
}

internal C_Node*
C_ParseDeclOrExpr(C_Context* ctx, C_Node** out_last, bool32 type_only, int32 level)
{
	void* result = NULL;
	
	if (C_IsBeginningOfDeclOrType(ctx))
		result = C_ParseDeclAndSemicolonIfNeeded(ctx, out_last, type_only);
	else
		result = C_ParseExpr(ctx, level, false);
	
	return result;
}

internal void*
C_ParseInitializerField(C_Context* ctx)
{
	Trace();
	
	C_AstExpr* result = NULL;
	C_AstExpr* head = NULL;
	
	for (;;)
	{
		switch (ctx->token->kind)
		{
			case C_TokenKind_Dot:
			{
				C_AstExpr* newnode = C_CreateNode(ctx, C_AstKind_ExprInitializerMember, SizeofPoly(C_AstExpr, desig_entry));
				if (head)
					head = head->as->desig_entry.next = newnode;
				else
					result = head = newnode;
				
				C_StreamNextToken(ctx);
				if (C_StreamAssertToken(ctx, C_TokenKind_Identifier))
					newnode->as->desig_entry.name = ctx->token->value_ident;
			} continue;
			
			case C_TokenKind_LeftBrkt:
			{
				C_AstExpr* newnode = C_CreateNode(ctx, C_AstKind_ExprInitializerIndex, SizeofPoly(C_AstExpr, desig_entry));
				if (head)
					head = head->as->desig_entry.next = newnode;
				else
					result = head = newnode;
				
				C_StreamNextToken(ctx);
				newnode->as->desig_entry.index = C_ParseExpr(ctx, 0, false);
				
				C_StreamEatToken(ctx, C_TokenKind_RightBrkt);
			} continue;
			
			default:
			{
				if (result)
				{
					C_StreamEatToken(ctx, C_TokenKind_Assign);
					
					C_AstExpr* newnode = C_CreateNode(ctx, C_AstKind_ExprInitializerIndex, SizeofPoly(C_AstExpr, desig_init));
					newnode->as->desig_init.desig = result;
					newnode->as->desig_init.expr = C_ParseExpr(ctx, 1, true);
					
					result = newnode;
				}
				else
					result = C_ParseExpr(ctx, 1, true);
			} break;
		}
		
		break;
	}
	
	return result;
}

internal String
C_PrepareStringLiteral(C_Context* ctx, bool32* restrict is_wide)
{
	Trace();
	
	String str;
	char* buffer = Arena_End(ctx->persistent_arena);
	*is_wide = false;
	
	do
	{
		str = ctx->token->value_str;
		Arena_PushMemory(ctx->persistent_arena, StrFmt(str));
		C_StreamNextToken(ctx);
	}
	while (ctx->token->kind == C_TokenKind_StringLiteral ||
		   (ctx->token->kind == C_TokenKind_WideStringLiteral && (*is_wide = true)));
	
	str.data = buffer;
	str.size = (char*)Arena_End(ctx->persistent_arena) - buffer;
	
	return str;
}

internal C_Node*
C_ParseExprFactor(C_Context* ctx, bool32 allow_init)
{
	Trace();
	
	C_AstExpr* result = C_CreateNode(ctx, C_AstKind_ExprFactor, SizeofPolyAny(C_AstExpr));
	C_AstExpr* head = result;
	bool32 has_unary = false;
	bool32 dont_parse_postfix = true;
	
	for (;; has_unary = true)
	{
		switch (ctx->token->kind)
		{
			case C_TokenKind_Plus: C_StreamNextToken(ctx); continue; // do nothing????
			
			case C_TokenKind_Minus:
			{
				head->h.kind = C_AstKind_Expr1Negative;
				head = head->as->unary.expr = C_CreateNode(ctx, C_AstKind_ExprFactor, SizeofPolyAny(C_AstExpr));
				
				C_StreamNextToken(ctx);
			} continue;
			
			case C_TokenKind_Mul:
			{
				head->h.kind = C_AstKind_Expr1Deref;
				head = head->as->unary.expr = C_CreateNode(ctx, C_AstKind_ExprFactor, SizeofPolyAny(C_AstExpr));
				
				C_StreamNextToken(ctx);
			} continue;
			
			case C_TokenKind_And:
			{
				head->h.kind = C_AstKind_Expr1Ref;
				head = head->as->unary.expr = C_CreateNode(ctx, C_AstKind_ExprFactor, SizeofPolyAny(C_AstExpr));
				
				C_StreamNextToken(ctx);
			} continue;
			
			case C_TokenKind_Not:
			{
				head->h.kind = C_AstKind_Expr1Not;
				head = head->as->unary.expr = C_CreateNode(ctx, C_AstKind_ExprFactor, SizeofPolyAny(C_AstExpr));
				
				C_StreamNextToken(ctx);
			} continue;
			
			case C_TokenKind_LNot:
			{
				head->h.kind = C_AstKind_Expr1LogicalNot;
				head = head->as->unary.expr = C_CreateNode(ctx, C_AstKind_ExprFactor, SizeofPolyAny(C_AstExpr));
				
				C_StreamNextToken(ctx);
			} continue;
			
			case C_TokenKind_Inc:
			{
				head->h.kind = C_AstKind_Expr1PrefixInc;
				head = head->as->unary.expr = C_CreateNode(ctx, C_AstKind_ExprFactor, SizeofPolyAny(C_AstExpr));
				
				C_StreamNextToken(ctx);
			} continue;
			
			case C_TokenKind_Dec:
			{
				head->h.kind = C_AstKind_Expr1PrefixDec;
				head = head->as->unary.expr = C_CreateNode(ctx, C_AstKind_ExprFactor, SizeofPolyAny(C_AstExpr));
				
				C_StreamNextToken(ctx);
			} continue;
			
			case C_TokenKind_Sizeof:
			{
				head->h.kind = C_AstKind_Expr1Sizeof;
				
				C_StreamNextToken(ctx);
				if (C_StreamTryEatToken(ctx, C_TokenKind_LeftParen))
				{
					if (C_IsBeginningOfDeclOrType(ctx))
					{
						head->h.kind = C_AstKind_Expr1SizeofType;
						
						C_UpdateNode(ctx, C_AstKind_Decl, &head->h);
						head->as->sizeof_type.type = C_ParseDecl(ctx, NULL, 1, NULL);
					}
					else
					{
						head->as->sizeof_expr.expr = C_ParseExpr(ctx, 0, false);
						C_UpdateNode(ctx, 0, &head->h);
						head = head->as->sizeof_expr.expr;
					}
					
					C_StreamEatToken(ctx, C_TokenKind_RightParen);
					goto ignore_factor;
				}
				else
				{
					head = head->as->sizeof_expr.expr = C_CreateNode(ctx, C_AstKind_ExprFactor, SizeofPolyAny(C_AstExpr));
				}
			} continue;
			
			case C_TokenKind_LeftParen:
			{
				C_StreamNextToken(ctx);
				if (C_IsBeginningOfDeclOrType(ctx))
				{
					C_AstType* type = C_ParseDecl(ctx, NULL, 1, NULL);
					
					C_StreamEatToken(ctx, C_TokenKind_RightParen);
					if (C_StreamTryEatToken(ctx, C_TokenKind_LeftCurl))
					{
						C_UpdateNode(ctx, 0, &head->h);
						head->h.kind = C_AstKind_ExprCompoundLiteral;
						head->as->compound.type = type;
						
						goto parse_init;
					}
					else
					{
						head->h.kind = C_AstKind_Expr1Cast;
						head->as->cast.to = type;
						head = head->as->cast.expr = C_CreateNode(ctx, C_AstKind_ExprFactor, SizeofPolyAny(C_AstExpr));
					}
				}
				else
				{
					// yes. this is a "leak"
					OurMemCopy(head, C_ParseExpr(ctx, 0, false), SizeofPolyAny(C_AstExpr));
					
					C_StreamEatToken(ctx, C_TokenKind_RightParen);
					dont_parse_postfix = false;
					goto ignore_factor;
				}
			} continue;
		}
		
		break;
	}
	
	if (allow_init && ctx->token->kind == C_TokenKind_LeftCurl)
	{
		C_UpdateNode(ctx, 0, &head->h);
		head->h.kind = C_AstKind_ExprInitializer;
		if (has_unary)
			C_TraceError(ctx, &ctx->token->trace, "expected expression.");
		
		C_StreamEatToken(ctx, C_TokenKind_LeftCurl);
		
		if (0)
		{
			parse_init: dont_parse_postfix = false;
		}
		
		head->as->init.exprs = C_ParseInitializerField(ctx);
		C_AstNode* last_init = (void*)head->as->init.exprs;
		
		while (C_StreamTryEatToken(ctx, C_TokenKind_Comma) &&
			   ctx->token->kind != C_TokenKind_RightCurl)
		{
			last_init = last_init->next = C_ParseInitializerField(ctx);
		}
		
		C_StreamEatToken(ctx, C_TokenKind_RightCurl);
	}
	else
	{
		dont_parse_postfix = false;
		
		switch (ctx->token->kind)
		{
			case C_TokenKind_IntLiteral:
			{
				C_UpdateNode(ctx, C_AstKind_ExprInt, &head->h);
				head->value_int = ctx->token->value_int;
				
				C_StreamNextToken(ctx);
			} break;
			
			case C_TokenKind_LIntLiteral:
			{
				C_UpdateNode(ctx, C_AstKind_ExprLInt, &head->h);
				head->value_int = ctx->token->value_int;
				
				C_StreamNextToken(ctx);
			} break;
			
			case C_TokenKind_LLIntLiteral:
			{
				C_UpdateNode(ctx, C_AstKind_ExprLLInt, &head->h);
				head->value_int = ctx->token->value_int;
				
				C_StreamNextToken(ctx);
			} break;
			
			case C_TokenKind_UintLiteral:
			{
				C_UpdateNode(ctx, C_AstKind_ExprUInt, &head->h);
				head->value_uint = ctx->token->value_uint;
				
				C_StreamNextToken(ctx);
			} break;
			
			case C_TokenKind_LUintLiteral:
			{
				C_UpdateNode(ctx, C_AstKind_ExprULInt, &head->h);
				head->value_uint = ctx->token->value_uint;
				
				C_StreamNextToken(ctx);
			} break;
			
			case C_TokenKind_LLUintLiteral:
			{
				C_UpdateNode(ctx, C_AstKind_ExprULLInt, &head->h);
				head->value_uint = ctx->token->value_uint;
				
				C_StreamNextToken(ctx);
			} break;
			
			case C_TokenKind_StringLiteral:
			case C_TokenKind_WideStringLiteral:
			{
				bool32 is_wide;
				
				C_UpdateNode(ctx, C_AstKind_ExprString, &head->h);
				head->value_str = C_PrepareStringLiteral(ctx, &is_wide);
				
				if (is_wide)
					head->h.kind = C_AstKind_ExprWideString;
			} break;
			
			case C_TokenKind_FloatLiteral:
			{
				C_UpdateNode(ctx, C_AstKind_ExprFloat, &head->h);
				head->value_float = ctx->token->value_float;
				
				C_StreamNextToken(ctx);
			} break;
			
			case C_TokenKind_DoubleLiteral:
			{
				C_UpdateNode(ctx, C_AstKind_ExprDouble, &head->h);
				head->value_double = ctx->token->value_double;
				
				C_StreamNextToken(ctx);
			} break;
			
			case C_TokenKind_Identifier:
			{
				if (C_IsBeginningOfDeclOrType(ctx))
					C_TraceError(ctx, &ctx->token->trace, "unexpected typename.");
				
				C_UpdateNode(ctx, C_AstKind_ExprIdent, &head->h);
				head->as->ident.name = ctx->token->value_ident;
				
				C_StreamNextToken(ctx);
			} break;
			
			case C_TokenKind_Eof:
			{
				C_TraceError(ctx, &ctx->token->trace, "expected expression before end of file.");
			} break;
			
			default:
			{
				C_TraceError(ctx, &ctx->token->trace, "expected expression.");
			} break;
		}
	}
	
	ignore_factor:;
	if (dont_parse_postfix)
		return result;
	
	int32 need_to_change_result = 0;
	for (;; (need_to_change_result == 1) ? (result = head) : 0)
	{
		need_to_change_result += (result == head);
		
		switch (ctx->token->kind)
		{
			case C_TokenKind_Inc:
			{
				C_AstExpr* new_node = C_CreateNode(ctx, C_AstKind_Expr1PostfixInc, SizeofPolyAny(C_AstExpr));
				new_node->as->unary.expr = head;
				head = new_node;
				
				C_StreamNextToken(ctx);
			} continue;
			
			case C_TokenKind_Dec:
			{
				C_AstExpr* new_node = C_CreateNode(ctx, C_AstKind_Expr1PostfixInc, SizeofPolyAny(C_AstExpr));
				new_node->as->unary.expr = head;
				head = new_node;
				
				C_StreamNextToken(ctx);
			} continue;
			
			case C_TokenKind_LeftParen:
			{
				C_AstExpr* new_node = C_CreateNode(ctx, C_AstKind_Expr2Call, SizeofPolyAny(C_AstExpr));
				new_node->as->binary.left = head;
				head = new_node;
				
				C_StreamNextToken(ctx);
				if (ctx->token->kind != C_TokenKind_RightParen)
				{
					C_AstNode* last_param = C_ParseExpr(ctx, 1, false);
					head->as->binary.right = (void*)last_param;
					
					while (C_StreamTryEatToken(ctx, C_TokenKind_Comma))
					{
						last_param = last_param->next = C_ParseExpr(ctx, 1, false);
					}
				}
				
				C_StreamEatToken(ctx, C_TokenKind_RightParen);
			} continue;
			
			case C_TokenKind_LeftBrkt:
			{
				C_AstExpr* new_node = C_CreateNode(ctx, C_AstKind_Expr2Index, SizeofPolyAny(C_AstExpr));
				new_node->as->binary.left = head;
				head = new_node;
				
				C_StreamNextToken(ctx);
				head->as->binary.right = C_ParseExpr(ctx, 0, false);
				
				C_StreamEatToken(ctx, C_TokenKind_RightBrkt);
			} continue;
			
			case C_TokenKind_Dot:
			{
				C_AstKind kind = C_AstKind_Expr2Access;
				
				if (0)
				{
					case C_TokenKind_Arrow: kind = C_AstKind_Expr2DerefAccess;
				}
				
				C_AstExpr* new_node = C_CreateNode(ctx, kind, SizeofPolyAny(C_AstExpr));
				new_node->as->access.expr = head;
				head = new_node;
				
				C_StreamNextToken(ctx);
				if (C_StreamAssertToken(ctx, C_TokenKind_Identifier))
				{
					head->as->access.field = ctx->token->value_ident;
					C_StreamNextToken(ctx);
				}
			} continue;
		}
		
		break;
	}
	
	return result;
}

internal C_Node*
C_ParseExpr(C_Context* ctx, int32 level, bool32 allow_init)
{
	Trace();
	
	C_AstExpr* result = C_ParseExprFactor(ctx, allow_init);
	
	C_OperatorPrecedence prec;
	while (prec = C_operators_precedence[ctx->token->kind],
		   prec.level > level)
	{
		int32 op = C_token_to_op[ctx->token->kind];
		C_StreamNextToken(ctx);
		C_AstExpr* right = C_ParseExprFactor(ctx, false);
		
		C_TokenKind lookahead = ctx->token->kind;
		C_OperatorPrecedence lookahead_prec = C_operators_precedence[lookahead];
		
		while (lookahead_prec.level > 0 &&
			   (lookahead_prec.level > prec.level ||
				(lookahead_prec.level == prec.level && lookahead_prec.right2left))) {
			C_StreamNextToken(ctx);
			
			C_AstExpr* tmp;
			
			if (lookahead == C_TokenKind_QuestionMark) {
				tmp = C_CreateNode(ctx, C_AstKind_Expr3Condition, SizeofPolyAny(C_AstExpr));
				tmp->as->ternary.left = right;
				tmp->as->ternary.middle = C_ParseExpr(ctx, 0, false);
				
				C_StreamEatToken(ctx, C_TokenKind_Colon);
				tmp->as->ternary.right = C_ParseExpr(ctx, lookahead_prec.level - 1, false);
			} else {
				tmp = C_CreateNode(ctx, C_token_to_op[lookahead], SizeofPolyAny(C_AstExpr));
				tmp->as->binary.left = right;
				tmp->as->binary.right = C_ParseExpr(ctx, level + 1, false);
			}
			
			lookahead = ctx->token->kind;
			lookahead_prec = C_operators_precedence[lookahead];
			right = tmp;
		}
		
		C_AstExpr* tmp = C_CreateNode(ctx, op, SizeofPolyAny(C_AstExpr));
		
		if (op == C_AstKind_Expr3Condition)
		{
			tmp->as->ternary.left = result;
			tmp->as->ternary.middle = right;
			
			C_StreamEatToken(ctx, C_TokenKind_Colon);
			tmp->as->ternary.right = C_ParseExpr(ctx, prec.level, false);
		}
		else
		{
			tmp->as->binary.left = result;
			tmp->as->binary.right = right;
		}
		
		result = tmp;
	}
	
	return result;
}

internal bool32
C_ParsePossibleGnuAttribute(C_Context* ctx, C_AstNode* apply_to)
{
	Trace();
	
	if (C_StreamTryEatToken(ctx, C_TokenKind_GccAttribute))
	{
		C_StreamEatToken(ctx, C_TokenKind_LeftParen);
		C_StreamEatToken(ctx, C_TokenKind_LeftParen);
		
		if (ctx->token->kind != C_TokenKind_RightParen)
		{
			do
			{
				if (!C_IsKeyword(ctx->token->kind) && ctx->token->kind != C_TokenKind_Identifier)
					break;
				
				C_AstAttribute* attrib = NULL;
				String name = ctx->token->value_ident;
				if (MatchCString("packed", name) || MatchCString("__packed__", name))
				{
					attrib = C_CreateNode(ctx, C_AstKind_AttributePacked, SizeofPoly(C_AstAttribute, aligned));
					
					C_StreamNextToken(ctx);
				}
				else if (MatchCString("aligned", name))
				{
					attrib = C_CreateNode(ctx, C_AstKind_AttributePacked, SizeofPoly(C_AstAttribute, aligned));
					
					C_StreamNextToken(ctx);
					if (C_StreamTryEatToken(ctx, C_TokenKind_LeftParen))
					{
						attrib->as->aligned.expr = C_ParseExpr(ctx, 0, false);
						
						C_StreamEatToken(ctx, C_TokenKind_RightParen);
					}
				}
				
				if (attrib)
				{
					attrib->h.next = (void*)apply_to->attributes;
					apply_to->attributes = attrib;
				}
			}
			while (!C_StreamEatToken(ctx, C_TokenKind_Comma));
		}
		
		C_StreamEatToken(ctx, C_TokenKind_RightParen);
		C_StreamEatToken(ctx, C_TokenKind_RightParen);
		
		return true;
	}
	
	return false;
}

internal bool32
C_ParsePossibleMsvcDeclspec(C_Context* ctx, C_AstNode* apply_to)
{
	Trace();
	
	if (C_StreamTryEatToken(ctx, C_TokenKind_MsvcDeclspec))
	{
		C_StreamEatToken(ctx, C_TokenKind_LeftParen);
		
		// TODO(ljre): Proper parsing
		while (ctx->token->kind != C_TokenKind_RightParen)
			C_StreamNextToken(ctx);
		
		C_StreamEatToken(ctx, C_TokenKind_RightParen);
		
		return true;
	}
	
	return false;
}

internal void*
C_ParseStmt(C_Context* ctx, C_Node** out_last, bool32 allow_decl)
{
	Trace();
	
	C_AstStmt* result = NULL;
	C_Node* last = NULL;
	
	switch (ctx->token->kind)
	{
		case C_TokenKind_Eof:
		{
			C_TraceError(ctx, &ctx->token->trace, "expected statement before end of file.");
		} break;
		
		case C_TokenKind_LeftCurl:
		{
			last = result = C_ParseBlock(ctx, NULL);
		} break;
		
		case C_TokenKind_If:
		{
			last = result = C_CreateNode(ctx, C_AstKind_StmtIf, SizeofPoly(C_AstStmt, generic));
			
			C_StreamNextToken(ctx);
			C_StreamEatToken(ctx, C_TokenKind_LeftParen);
			result->as->generic.leafs[0] = C_ParseExpr(ctx, 0, false);
			
			C_StreamEatToken(ctx, C_TokenKind_RightParen);
			result->as->generic.leafs[1] = C_ParseStmt(ctx, NULL, false);
			
			if (C_StreamTryEatToken(ctx, C_TokenKind_Else))
				result->as->generic.leafs[2] = C_ParseStmt(ctx, NULL, false);
		} break;
		
		case C_TokenKind_While:
		{
			last = result = C_CreateNode(ctx, C_AstKind_StmtWhile, SizeofPoly(C_AstStmt, generic));
			
			C_StreamNextToken(ctx);
			C_StreamEatToken(ctx, C_TokenKind_LeftParen);
			result->as->generic.leafs[0] = C_ParseExpr(ctx, 0, false);
			
			C_StreamEatToken(ctx, C_TokenKind_RightParen);
			result->as->generic.leafs[1] = C_ParseStmt(ctx, NULL, false);
		} break;
		
		case C_TokenKind_For:
		{
			last = result = C_CreateNode(ctx, C_AstKind_StmtFor, SizeofPoly(C_AstStmt, generic));
			
			C_StreamNextToken(ctx);
			C_StreamEatToken(ctx, C_TokenKind_LeftParen);
			if (ctx->token->kind != C_TokenKind_Semicolon)
				result->as->generic.leafs[0] = C_ParseDeclOrExpr(ctx, NULL, false, 0);
			
			C_StreamEatToken(ctx, C_TokenKind_Semicolon);
			if (ctx->token->kind != C_TokenKind_Semicolon)
				result->as->generic.leafs[1] = C_ParseExpr(ctx, 0, false);
			
			C_StreamEatToken(ctx, C_TokenKind_Semicolon);
			if (ctx->token->kind != C_TokenKind_RightParen)
				result->as->generic.leafs[2] = C_ParseExpr(ctx, 0, false);
			
			C_StreamEatToken(ctx, C_TokenKind_RightParen);
			result->as->generic.leafs[3] = C_ParseStmt(ctx, NULL, false);
		} break;
		
		case C_TokenKind_Do:
		{
			last = result = C_CreateNode(ctx, C_AstKind_StmtDoWhile, SizeofPoly(C_AstStmt, generic));
			
			C_StreamNextToken(ctx);
			result->as->generic.leafs[0] = C_ParseStmt(ctx, NULL, false);
			
			C_StreamEatToken(ctx, C_TokenKind_While);
			C_StreamEatToken(ctx, C_TokenKind_LeftParen);
			result->as->generic.leafs[1] = C_ParseExpr(ctx, 0, false);
			
			C_StreamEatToken(ctx, C_TokenKind_RightParen);
			C_StreamEatToken(ctx, C_TokenKind_Semicolon);
		} break;
		
		case C_TokenKind_GccAttribute:
		{
			last = result = C_CreateNode(ctx, C_AstKind_StmtEmpty, sizeof(C_AstStmt));
			C_ParsePossibleGnuAttribute(ctx, &result->h);
			
			if (allow_decl && C_IsBeginningOfDeclOrType(ctx))
			{
				C_AstAttribute* attribs = result->h.attributes;
				result = C_ParseDeclAndSemicolonIfNeeded(ctx, (void*)&last, 4);
				
				for (C_AstNode* it = (void*)result; it; it = it->next)
					C_AppendNode(ctx, (void*)&it->attributes, attribs);
			}
			else
				C_StreamEatToken(ctx, C_TokenKind_Semicolon);
		} break;
		
		case C_TokenKind_Semicolon:
		{
			last = result = C_CreateNode(ctx, C_AstKind_StmtEmpty, sizeof(C_AstStmt));
			
			C_StreamNextToken(ctx);
		} break;
		
		case C_TokenKind_Switch:
		{
			last = result = C_CreateNode(ctx, C_AstKind_StmtSwitch, SizeofPoly(C_AstStmt, generic));
			
			C_StreamNextToken(ctx);
			C_StreamEatToken(ctx, C_TokenKind_LeftParen);
			result->as->generic.leafs[0] = C_ParseExpr(ctx, 0, false);
			
			C_StreamEatToken(ctx, C_TokenKind_RightParen);
			result->as->generic.leafs[1] = C_ParseStmt(ctx, NULL, false);
		} break;
		
		case C_TokenKind_Return:
		{
			last = result = C_CreateNode(ctx, C_AstKind_StmtReturn, SizeofPoly(C_AstStmt, generic));
			
			C_StreamNextToken(ctx);
			if (ctx->token->kind != C_TokenKind_Semicolon)
				result->as->generic.leafs[0] = C_ParseExpr(ctx, 0, false);
			
			C_StreamEatToken(ctx, C_TokenKind_Semicolon);
		} break;
		
		case C_TokenKind_Case:
		{
			last = result = C_CreateNode(ctx, C_AstKind_StmtCase, SizeofPoly(C_AstStmt, generic));
			
			C_StreamNextToken(ctx);
			result->as->generic.leafs[0] = C_ParseExpr(ctx, 0, false);
			
			C_StreamEatToken(ctx, C_TokenKind_Colon);
			result->as->generic.leafs[1] = C_ParseStmt(ctx, NULL, false);
		} break;
		
		case C_TokenKind_Goto:
		{
			last = result = C_CreateNode(ctx, C_AstKind_StmtGoto, SizeofPoly(C_AstStmt, go_to));
			
			C_StreamNextToken(ctx);
			if (C_StreamAssertToken(ctx, C_TokenKind_Identifier))
			{
				result->as->go_to.label_name = ctx->token->value_ident;
				
				C_StreamNextToken(ctx);
			}
			
			C_StreamEatToken(ctx, C_TokenKind_Semicolon);
		} break;
		
		case C_TokenKind_Break:
		{
			// TODO(ljre): Find host
			last = result = C_CreateNode(ctx, C_AstKind_StmtBreak, sizeof(C_AstStmt));
			C_StreamNextToken(ctx);
			C_StreamEatToken(ctx, C_TokenKind_Semicolon);
		} break;
		
		case C_TokenKind_Continue:
		{
			// TODO(ljre): Find host
			last = result = C_CreateNode(ctx, C_AstKind_StmtContinue, sizeof(C_AstStmt));
			C_StreamNextToken(ctx);
			C_StreamEatToken(ctx, C_TokenKind_Semicolon);
		} break;
		
		case C_TokenKind_Default:
		{
			last = result = C_CreateNode(ctx, C_AstKind_StmtContinue, SizeofPoly(C_AstStmt, label));
			C_StreamNextToken(ctx);
			C_StreamEatToken(ctx, C_TokenKind_Colon);
			
			result->as->label.stmt = C_ParseStmt(ctx, NULL, false);
		} break;
		
		case C_TokenKind_MsvcAsm:
		{
			// TODO(ljre): Proper __asm parsing. WTF is this syntax??????????
			last = result = C_CreateNode(ctx, C_AstKind_StmtEmpty, sizeof(C_AstStmt));
			
			C_StreamNextToken(ctx);
			if (C_StreamTryEatToken(ctx, C_TokenKind_LeftCurl))
			{
				while (ctx->token->kind != C_TokenKind_RightCurl)
					C_StreamNextToken(ctx);
				
				C_StreamNextToken(ctx);
			}
			
			C_StreamTryEatToken(ctx, C_TokenKind_Semicolon);
		} break;
		
		case C_TokenKind_GccAsm:
		{
			last = result = C_CreateNode(ctx, C_AstKind_StmtGccAsm, SizeofPoly(C_AstStmt, gcc_asm));
			
			C_StreamNextToken(ctx);
			for (;; C_StreamNextToken(ctx))
			{
				switch (ctx->token->kind)
				{
					case C_TokenKind_Volatile: result->h.flags |= C_AstFlags_GccAsmVolatile; continue;
					case C_TokenKind_Inline: result->h.flags |= C_AstFlags_GccAsmInline; continue;
					case C_TokenKind_Goto: result->h.flags |= C_AstFlags_GccAsmGoto; continue;
				}
				
				break;
			}
			
			C_StreamEatToken(ctx, C_TokenKind_LeftParen);
			if (C_StreamAssertToken(ctx, C_TokenKind_StringLiteral))
			{
				result->as->gcc_asm.leafs[0] = C_ParseExprFactor(ctx, false);
				
				for (int32 i = 1; i <= 4; ++i)
				{
					if (!C_StreamTryEatToken(ctx, C_TokenKind_Colon))
						break;
					
					if (ctx->token->kind == C_TokenKind_StringLiteral ||
						ctx->token->kind == C_TokenKind_Identifier)
					{
						C_AstNode* head = C_ParseExprFactor(ctx, false);
						result->as->gcc_asm.leafs[i] = (void*)head;
						
						while (C_StreamTryEatToken(ctx, C_TokenKind_Comma))
							head = head->next = C_ParseExprFactor(ctx, false);
					}
				}
			}
			
			C_StreamEatToken(ctx, C_TokenKind_RightParen);
		} break;
		
		default:
		{
			if (allow_decl && C_IsBeginningOfDeclOrType(ctx))
				last = result = C_ParseDeclAndSemicolonIfNeeded(ctx, &last, 4);
			else
			{
				last = result = C_ParseExpr(ctx, 0, false);
				
				if (result->h.kind == C_AstKind_ExprIdent && C_StreamTryEatToken(ctx, C_TokenKind_Colon))
				{
					result->h.kind = C_AstKind_StmtLabel;
					result->as->label.stmt = C_ParseStmt(ctx, NULL, false);
					
					if (result->as->label.stmt->h.kind == C_AstKind_StmtEmpty)
						result->h.attributes = result->as->label.stmt->h.attributes; // NOTE(ljre): For GCC __attribute__ only.
				}
				else
				{
					C_AstStmt* s = C_CreateNode(ctx, C_AstKind_StmtExpr, SizeofPoly(C_AstStmt, expr));
					s->as->expr.expr = (void*)result;
					last = result = s;
					
					C_StreamEatToken(ctx, C_TokenKind_Semicolon);
				}
			}
		} break;
	}
	
	if (out_last)
		*out_last = last;
	
	return result;
}

internal C_Node*
C_ParseBlock(C_Context* ctx, C_Node** out_last)
{
	Trace();
	
	C_StreamEatToken(ctx, C_TokenKind_LeftCurl);
	C_AstStmt* result = C_CreateNode(ctx, C_AstKind_StmtCompound, SizeofPoly(C_AstStmt, compound));
	C_AstNode* last = NULL;
	
	if (ctx->token->kind == C_TokenKind_RightCurl)
	{
		// NOTE(ljre): Not needed
		//result->stmt = C_CreateNode(ctx, C_NodeKind_EmptyStmt);
		
		C_StreamEatToken(ctx, C_TokenKind_RightCurl);
	}
	else if (ctx->token->kind)
	{
		result->as->compound.scope = C_PushSymbolScope(ctx);
		result->as->compound.stmts = C_ParseStmt(ctx, (void*)&last, true);
		
		while (ctx->token->kind && !C_StreamTryEatToken(ctx, C_TokenKind_RightCurl))
		{
			C_Node* new_last;
			C_AstNode* new_node = C_ParseStmt(ctx, &new_last, true);
			last->next = new_node;
			last = new_last;
		}
		
		C_PopSymbolScope(ctx);
	}
	
	if (out_last)
		*out_last = result;
	
	return result;
}

internal C_AstType*
C_ParseRestOfDeclIt(C_Context* ctx, C_AstType** head, uint32* flags_head, C_AstAttribute** attrib_head, C_AstDecl* decl, bool32 type_only, bool32 is_global)
{
	Trace();
	
	C_AstType** result = head;
	uint32 saved_flags = 0;
	C_AstAttribute* saved_attrib = NULL;
	
	for (;;)
	{
		switch (ctx->token->kind)
		{
			case C_TokenKind_Mul:
			{
				C_StreamNextToken(ctx);
				
				C_AstType* newtype = C_CreateNode(ctx, C_AstKind_TypePointer, SizeofPoly(C_AstType, ptr));
				*head = newtype;
				
				head = &newtype->as->ptr.to;
				flags_head = &newtype->h.flags;
				attrib_head = &newtype->h.attributes;
			} continue;
			
			case C_TokenKind_Const:
			{
				C_StreamNextToken(ctx);
				
				*flags_head |= C_AstFlags_Const;
			} continue;
			
			case C_TokenKind_Restrict:
			{
				C_StreamNextToken(ctx);
				
				*flags_head |= C_AstFlags_Restrict;
			} continue;
			
			// NOTE(ljre): MSVC calling convention keywords
			{
				uint32 flags;
				
				/**/ if (0) case C_TokenKind_MsvcCdecl: flags = C_AstFlags_MsvcCdecl;
				else if (0) case C_TokenKind_MsvcStdcall: flags = C_AstFlags_MsvcStdcall;
				else if (0) case C_TokenKind_MsvcVectorcall: flags = C_AstFlags_MsvcVectorcall;
				else if (0) case C_TokenKind_MsvcFastcall: flags = C_AstFlags_MsvcFastcall;
				
				C_AstAttribute* attrib = C_CreateNode(ctx, C_AstKind_AttributeCallconv, sizeof(C_AstAttribute));
				attrib->h.flags |= flags;
				attrib->h.next = (void*)*attrib_head;
				*attrib_head = attrib;
				
				C_StreamNextToken(ctx);
			} continue;
			
			case C_TokenKind_LeftParen:
			{
				C_StreamNextToken(ctx);
				*head = C_ParseRestOfDeclIt(ctx, head, &saved_flags, &saved_attrib, decl, type_only, is_global);
				C_StreamEatToken(ctx, C_TokenKind_RightParen);
				
				goto after_identifier;
			} break;
		}
		
		break;
	}
	
	if (!type_only && ctx->token->kind == C_TokenKind_Identifier)
	{
		decl->name = ctx->token->value_ident;
		C_StreamNextToken(ctx);
	}
	
	after_identifier:;
	
	for (;;)
	{
		switch (ctx->token->kind)
		{
			case C_TokenKind_LeftParen:
			{
				C_StreamNextToken(ctx);
				C_AstType* newtype = C_CreateNode(ctx, C_AstKind_TypeFunction, SizeofPoly(C_AstType, function));
				
				if (saved_attrib)
				{
					newtype->h.attributes = saved_attrib;
					
					C_AppendNode(ctx, (void*)&saved_attrib->h.next, *attrib_head);
					saved_attrib->h.next = (void*)*attrib_head;
					saved_attrib = NULL;
				}
				
				if (saved_flags)
				{
					newtype->h.flags |= saved_flags;
					saved_flags = 0;
				}
				
				newtype->as->function.ret = *head;
				*head = newtype;
				result = head;
				
				if (ctx->token->kind == C_TokenKind_RightParen)
					newtype->h.flags |= C_AstFlags_VarArgs;
				else
				{
					C_AstNode* last_param;
					newtype->as->function.params = C_ParseDecl(ctx, (void*)&last_param, 8, NULL);
					
					while (C_StreamTryEatToken(ctx, C_TokenKind_Comma))
					{
						if (C_StreamTryEatToken(ctx, C_TokenKind_VarArgs))
						{
							newtype->h.flags |= C_AstFlags_VarArgs;
							break;
						}
						
						C_Node* new_last_param;
						last_param->next = C_ParseDecl(ctx, &new_last_param, 8, NULL);
						last_param = new_last_param;
					}
				}
				
				C_StreamEatToken(ctx, C_TokenKind_RightParen);
			} continue;
			
			case C_TokenKind_LeftBrkt:
			{
				C_StreamNextToken(ctx);
				C_AstType* newtype = C_CreateNode(ctx, C_AstKind_TypeVlaArray, SizeofPolyAny(C_AstType));
				
				if (saved_attrib)
				{
					newtype->h.attributes = saved_attrib;
					
					C_AppendNode(ctx, (void*)&saved_attrib->h.next, *attrib_head);
					saved_attrib->h.next = (void*)*attrib_head;
					saved_attrib = NULL;
				}
				
				if (saved_flags)
				{
					newtype->h.flags |= saved_flags;
					saved_flags = 0;
				}
				
				newtype->as->array.of = *head;
				*head = newtype;
				result = head;
				
				if (ctx->token->kind != C_TokenKind_RightBrkt)
					newtype->as->vla_array.length = C_ParseExpr(ctx, 0, false);
				
				C_StreamEatToken(ctx, C_TokenKind_RightBrkt);
			} continue;
		}
		
		break;
	}
	
	return *result;
}

internal C_AstType*
C_ParseRestOfDecl(C_Context* ctx, C_AstType* base, C_AstDecl* decl, bool32 type_only, bool32 is_global)
{
	uint32 flags = 0;
	C_AstAttribute* attrib = NULL;
	C_AstType* head = base;
	
	C_AstType* result = C_ParseRestOfDeclIt(ctx, &head, &flags, &attrib, decl, type_only, is_global);
	
	if (flags || attrib)
	{
		C_AstType* tmp = C_CreateNode(ctx, base->h.kind, SizeofPolyAny(C_AstType));
		*tmp = *base;
		*tmp->as = *base->as;
		
		tmp->h.flags |= flags;
		
		if (attrib)
		{
			C_AstAttribute* saved_attrib = tmp->h.attributes;
			tmp->h.attributes = attrib;
			if (saved_attrib)
				C_AppendNode(ctx, (void*)&tmp->h.attributes, saved_attrib);
		}
		
		result = tmp;
	}
	
	return result;
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
internal C_Node*
C_ParseDecl(C_Context* ctx, C_Node** out_last, int32 options, bool32* out_should_eat_semicolon)
{
	Trace();
	Assert(!(options & 4) || out_should_eat_semicolon);
	
	C_AstDecl* decl = NULL;
	if (!(options & 1))
		decl = C_CreateNode(ctx, C_AstKind_Decl, sizeof(C_AstDecl));
	
	C_AstType* base = C_CreateNode(ctx, C_AstKind_Type, SizeofPolyAny(C_AstType));
	C_AstAttribute* callconv = NULL;
	
	beginning:;
	// Parse Base Type
	for (;;)
	{
		switch (ctx->token->kind)
		{
			case C_TokenKind_Auto:
			{
				if (options & 2)
					C_TraceError(ctx, &ctx->token->trace, "cannot use automatic storage for global object.");
				
				if (options & 1)
				{
					C_TraceError(ctx, &ctx->token->trace, "expected a type.");
					break;
				}
				
				if (decl->h.kind != C_AstKind_Decl)
					C_TraceError(ctx, &ctx->token->trace, "invalid use of multiple storage modifiers.");
				else
					decl->h.kind = C_AstKind_DeclAuto;
			} break;
			
			case C_TokenKind_Char:
			{
				if (base->h.kind != C_AstKind_Type)
					C_TraceError(ctx, &ctx->token->trace, "object cannot have more than one type; did you miss a semicolon?");
				else
					base->h.kind |= C_AstKind_TypeChar;
			} break;
			
			case C_TokenKind_Const:
			{
				if (base->h.flags & C_AstFlags_Const)
					C_TraceError(ctx, &ctx->token->trace, "too much constness.");
				
				base->h.flags |= C_AstFlags_Const;
			} break;
			
			case C_TokenKind_Double:
			{
				if (base->h.kind != C_AstKind_Type)
					C_TraceError(ctx, &ctx->token->trace, "object cannot have more than one type.");
				else
					base->h.kind = C_AstKind_TypeDouble;
			} break;
			
			case C_TokenKind_Extern:
			{
				if (options & 1)
				{
					C_TraceError(ctx, &ctx->token->trace, "expected a type.");
					break;
				}
				
				if (decl->h.kind != C_AstKind_Decl)
					C_TraceError(ctx, &ctx->token->trace, "invalid use of multiple storage modifiers.");
				else
					decl->h.flags |= C_AstKind_DeclExtern;
			} break;
			
			case C_TokenKind_Enum:
			{
				if (base->h.kind != C_AstKind_Type)
					C_TraceError(ctx, &ctx->token->trace, "object cannot have more than one type.");
				
				base->h.kind = C_AstKind_TypeEnum;
				
				C_StreamNextToken(ctx);
				if (ctx->token->kind == C_TokenKind_Identifier)
				{
					base->as->enumerator.name = ctx->token->value_ident;
					
					C_StreamNextToken(ctx);
				}
				
				if (ctx->token->kind == C_TokenKind_LeftCurl)
					base->as->enumerator.entries = C_ParseEnumBody(ctx);
				
				C_ParsePossibleGnuAttribute(ctx, (void*)base);
			} continue;  // ignore the C_NextToken() at the end.
			
			case C_TokenKind_Float:
			{
				if (base->h.kind != C_AstKind_Type)
					C_TraceError(ctx, &ctx->token->trace, "object cannot have more than one type.");
				else
					base->h.kind = C_AstKind_TypeFloat;
			} break;
			
			case C_TokenKind_GccExtension:
			{
				// TODO(ljre):
			} break;
			
			case C_TokenKind_Int:
			{
				if (base->h.kind != C_AstKind_Type)
					C_TraceError(ctx, &ctx->token->trace, "object cannot have more than one type.");
				else
					base->h.kind = C_AstKind_TypeInt;
			} break;
			
			case C_TokenKind_Inline:
			{
				if (options & 1)
				{
					C_TraceError(ctx, &ctx->token->trace, "expected a type.");
					break;
				}
				
				if (decl->h.flags & C_AstFlags_Inline)
					C_TraceError(ctx, &ctx->token->trace, "repeated use of keyword 'inline'.");
				
				decl->h.flags |= C_AstFlags_Inline;
			} break;
			
			case C_TokenKind_Long:
			{
				if (base->h.flags & C_AstFlags_LongLong)
					C_TraceError(ctx, &ctx->token->trace, "too long for me.");
				else if (base->h.flags & C_AstFlags_Short)
					C_TraceError(ctx, &ctx->token->trace, "'long' does not work with 'short'.");
				else if (base->h.flags & C_AstFlags_Long)
				{
					// combination of long and short flags is long long
					base->h.flags &= ~C_AstFlags_Long;
					base->h.flags |= C_AstFlags_LongLong;
				}
				else
					base->h.flags |= C_AstFlags_Long;
			} break;
			
			case C_TokenKind_MsvcDeclspec:
			{
				if (options & 1)
				{
					C_TraceError(ctx, &ctx->token->trace, "expected a type.");
					
					C_AstNode dummy = { 0 };
					C_ParsePossibleMsvcDeclspec(ctx, &dummy);
					continue;
				}
				
				C_ParsePossibleMsvcDeclspec(ctx, (void*)decl);
			} continue;
			
			// NOTE(ljre): MSVC calling convention keywords
			{
				uint32 flags;
				
				/**/ if (0) case C_TokenKind_MsvcCdecl: flags = C_AstFlags_MsvcCdecl;
				else if (0) case C_TokenKind_MsvcStdcall: flags = C_AstFlags_MsvcStdcall;
				else if (0) case C_TokenKind_MsvcVectorcall: flags = C_AstFlags_MsvcVectorcall;
				else if (0) case C_TokenKind_MsvcFastcall: flags = C_AstFlags_MsvcFastcall;
				
				if (!callconv)
				{
					callconv = C_CreateNode(ctx, C_AstKind_AttributeCallconv, sizeof(*callconv));
					callconv->h.flags |= flags;
				}
				else
				{
					// TODO(ljre): Error
				}
			} break;
			
			case C_TokenKind_MsvcForceinline:
			{
				if (options & 1)
				{
					C_TraceError(ctx, &ctx->token->trace, "expected a type.");
					break;
				}
				
				C_AstAttribute* attrib = C_CreateNode(ctx, C_AstKind_AttributeMsvcForceinline, sizeof(C_AstAttribute));
				C_AppendNode(ctx, (void*)&decl->h.attributes, attrib);
			} break;
			
			case C_TokenKind_Register:
			{
				if (options & 1)
				{
					C_TraceError(ctx, &ctx->token->trace, "expected a type.");
					break;
				}
				
				if (decl->h.kind != C_AstKind_Decl)
					C_TraceError(ctx, &ctx->token->trace, "invalid use of multiple storage modifiers.");
				else
					decl->h.flags |= C_AstKind_DeclRegister;
			} break;
			
			case C_TokenKind_Restrict:
			{
				if (base->h.flags & C_AstFlags_Restrict)
					C_TraceError(ctx, &ctx->token->trace, "repeated use of keyword 'restrict'.");
				
				base->h.flags |= C_AstFlags_Restrict;
			} break;
			
			case C_TokenKind_Short:
			{
				if (base->h.flags & (C_AstFlags_Long | C_AstFlags_LongLong))
					C_TraceError(ctx, &ctx->token->trace, "'short' does not work with 'long'.");
				else if (base->h.flags & C_AstFlags_Short)
					C_TraceError(ctx, &ctx->token->trace, "too short for me.\n");
				else
					base->h.flags |= C_AstFlags_Short;
			} break;
			
			case C_TokenKind_Signed:
			{
				if (base->h.flags & C_AstFlags_Signed)
					C_TraceError(ctx, &ctx->token->trace, "repeated use of keyword 'signed'.");
				else if (base->h.flags & C_AstFlags_Unsigned)
					C_TraceError(ctx, &ctx->token->trace, "'signed' does not work with 'unsigned'.");
				else
					base->h.flags |= C_AstFlags_Signed;
			} break;
			
			case C_TokenKind_Static:
			{
				if (options & 1)
				{
					C_TraceError(ctx, &ctx->token->trace, "expected a type.");
					break;
				}
				
				if (decl->h.kind != C_AstKind_Decl)
					C_TraceError(ctx, &ctx->token->trace, "invalid use of multiple storage modifiers.");
				else
					decl->h.kind = C_AstKind_DeclStatic;
			} break;
			
			case C_TokenKind_Struct:
			{
				if (base->h.kind != C_AstKind_Type)
					C_TraceError(ctx, &ctx->token->trace, "object cannot have more than one type.");
				
				base->h.kind = C_AstKind_TypeStruct;
				
				C_StreamNextToken(ctx);
				if (ctx->token->kind == C_TokenKind_Identifier)
				{
					base->as->structure.name = ctx->token->value_ident;
					
					C_StreamNextToken(ctx);
				}
				
				if (ctx->token->kind == C_TokenKind_LeftCurl)
					base->as->structure.body = C_ParseStructBody(ctx);
				
				C_ParsePossibleGnuAttribute(ctx, (void*)base);
			} continue;  // ignore the C_NextToken() at the end.
			
			case C_TokenKind_Typedef:
			{
				if (options & 1)
				{
					C_TraceError(ctx, &ctx->token->trace, "expected a type.");
					break;
				}
				
				if (decl->h.kind != C_AstKind_Decl)
					C_TraceError(ctx, &ctx->token->trace, "invalid use of multiple storage modifiers.");
				else
					decl->h.kind = C_AstKind_DeclTypedef;
			} break;
			
			case C_TokenKind_Union:
			{
				if (base->h.kind != C_AstKind_Type)
					C_TraceError(ctx, &ctx->token->trace, "object cannot have more than one type.");
				
				base->h.kind = C_AstKind_TypeUnion;
				
				C_StreamNextToken(ctx);
				if (ctx->token->kind == C_TokenKind_Identifier)
				{
					base->as->structure.name = ctx->token->value_ident;
					
					C_StreamNextToken(ctx);
				}
				
				if (ctx->token->kind == C_TokenKind_LeftCurl)
					base->as->structure.body = C_ParseStructBody(ctx);
				
				C_ParsePossibleGnuAttribute(ctx, (void*)base);
			} continue; // ignore the C_NextToken() at the end.
			
			case C_TokenKind_Unsigned:
			{
				if (base->h.flags & C_AstFlags_Unsigned)
					C_TraceError(ctx, &ctx->token->trace, "repeated use of keyword 'unsigned'.");
				else if (base->h.flags & C_AstFlags_Signed)
					C_TraceError(ctx, &ctx->token->trace, "'unsigned' does not work with 'signed'.");
				else
					base->h.flags |= C_AstFlags_Unsigned;
			} break;
			
			case C_TokenKind_Void:
			{
				if (base->h.kind != C_AstKind_Type)
					C_TraceError(ctx, &ctx->token->trace, "object cannot have more than one type.");
				else
					base->h.kind |= C_AstKind_TypeVoid;
			} break;
			
			case C_TokenKind_Volatile:
			{
				if (base->h.flags & C_AstFlags_Volatile)
					C_TraceError(ctx, &ctx->token->trace, "repeated use of keyword 'volatile'.");
				
				base->h.flags |= C_AstFlags_Volatile;
			} break;
			
			case C_TokenKind_Bool:
			{
				if (base->h.kind != C_AstKind_Type)
					C_TraceError(ctx, &ctx->token->trace, "object cannot have more than one type.");
				else
					base->h.kind = C_AstKind_TypeBool;
			} break;
			
			case C_TokenKind_Complex:
			{
				if (base->h.flags & C_AstFlags_Complex)
					C_TraceError(ctx, &ctx->token->trace, "repeated use of keyword '_Complex'.");
				
				base->h.flags |= C_AstFlags_Complex;
			} break;
			
			case C_TokenKind_Identifier:
			{
				if (C_IsBeginningOfDeclOrType(ctx))
				{
					if (base->h.kind != C_AstKind_Type)
						C_TraceError(ctx, &ctx->token->trace, "object cannot have more than one type.");
					
					base->h.kind = C_AstKind_TypeTypename;
					base->as->typedefed.name = ctx->token->value_ident;
					break;
				}
				else if (options & 1)
					C_TraceError(ctx, &ctx->token->trace, "'%S' is not a typename", StrFmt(ctx->token->value_ident));
			} /* fallthrough */
			
			default: goto out_of_loop;
		}
		
		C_StreamNextToken(ctx);
	}
	
	out_of_loop:;
	
	bool32 implicit_int = false;
	if (base->h.kind == C_AstKind_Type)
	{
		if (base->h.flags & (C_AstFlags_Unsigned |
							 C_AstFlags_Signed |
							 C_AstFlags_Short |
							 C_AstFlags_Long |
							 C_AstFlags_LongLong))
		{
			base->h.kind = C_AstKind_TypeInt;
		}
		else
		{
			implicit_int = true;
		}
	}
	
	right_before_parsing_rest_of_decl:;
	C_AstType* type = C_ParseRestOfDecl(ctx, base, decl, options & 1, options & 2);
	
	if (implicit_int)
	{
		bool32 reported = false;
		
		if (type->h.kind == C_AstKind_Type)
		{
			if (ctx->token->kind == C_TokenKind_Identifier)
			{
				reported = true;
				C_TraceError(ctx, &ctx->token->trace, "'%S' is not a typename", StrFmt(decl->name));
				C_StreamNextToken(ctx);
				
				goto beginning;
			}
			else if (C_IsBeginningOfDeclOrType(ctx))
			{
				reported = true;
				C_TraceError(ctx, &ctx->token->trace, "'%S' is not a typename", StrFmt(decl->name));
				
				goto beginning;
			}
			else if (ctx->token->kind == C_TokenKind_Identifier ||
					 ctx->token->kind == C_TokenKind_Mul)
			{
				reported = true;
				C_TraceError(ctx, &ctx->token->trace, "'%S' is not a typename", StrFmt(decl->name));
				
				goto right_before_parsing_rest_of_decl;
			}
		}
		
		if (!reported)
		{
			if (type->h.kind == C_AstKind_TypeFunction)
				C_TraceWarning(ctx, &ctx->token->trace, C_Warning_ImplicitInt, "implicit return type 'int'.");
			else
				C_TraceWarning(ctx, &ctx->token->trace, C_Warning_ImplicitInt, "implicit type 'int'.");
		}
		
		base->h.kind = C_AstKind_TypeInt;
	}
	
	// TODO(ljre): Apply 'callconv'
	
	if (options & 8)
	{
		if (type->h.kind == C_AstKind_TypeArray)
		{
			type->h.kind = C_AstKind_TypePointer;
			type->h.flags |= C_AstFlags_Decayed;
		}
		else if (type->h.kind == C_AstKind_TypeFunction)
		{
			C_AstType* newtype = C_CreateNode(ctx, C_AstKind_TypePointer, SizeofPoly(C_AstType, ptr));
			newtype->as->ptr.to = type;
			type = newtype;
			
			type->h.flags |= C_AstFlags_Decayed;
		}
	}
	
	if (options & 1)
		return type;
	
	// This is a declaration!
	C_AstNode* last = (void*)decl;
	decl->type = type;
	
	if (decl->h.kind == C_AstKind_DeclTypedef)
		C_TypedefDecl(ctx, decl);
	
	if (ctx->token->kind == C_TokenKind_LeftCurl)
	{
		if (type->h.kind != C_AstKind_TypeFunction)
			C_TraceError(ctx, &ctx->token->trace, "invalid block for non-function declaration.");
		
		if (!(options & 2))
			C_TraceError(ctx, &ctx->token->trace, "function definitions are only allowed in global scope.");
		
		decl->body = C_ParseBlock(ctx, NULL);
	}
	else if (options & 4)
	{
		if (C_StreamTryEatToken(ctx, C_TokenKind_Assign))
		{
			if (decl->h.kind == C_AstKind_DeclTypedef)
				C_TraceError(ctx, &ctx->token->trace, "cannot assign to types.");
			else if (decl->h.kind == C_AstKind_DeclExtern)
				C_TraceError(ctx, &ctx->token->trace, "cannot initialize a declaration with 'extern' storage modifier.");
			
			decl->init = C_ParseExpr(ctx, 1, true);
		}
		
		while (C_StreamTryEatToken(ctx, C_TokenKind_Comma))
		{
			C_AstDecl* new_node = C_CreateNode(ctx, decl->h.kind, sizeof(C_AstDecl));
			new_node->type = C_ParseRestOfDecl(ctx, base, new_node, false, options & 2);
			last = last->next = (void*)new_node;
			
			// TODO(ljre): Apply 'callconv'
			
			if (options & 8)
			{
				if (new_node->type->h.kind == C_AstKind_TypeArray)
				{
					new_node->type->h.kind = C_AstKind_TypePointer;
					new_node->type->h.flags |= C_AstFlags_Decayed;
				}
				else if (new_node->type->h.kind == C_AstKind_TypeFunction)
				{
					C_AstType* newtype = C_CreateNode(ctx, C_AstKind_TypePointer, SizeofPoly(C_AstType, ptr));
					newtype->as->ptr.to = new_node->type;
					new_node->type = newtype;
					
					new_node->type->h.flags |= C_AstFlags_Decayed;
				}
			}
			
			if (decl->h.kind == C_AstKind_DeclTypedef)
				C_TypedefDecl(ctx, new_node);
			
			if (C_StreamTryEatToken(ctx, C_TokenKind_Assign))
			{
				if (decl->h.kind == C_AstKind_DeclTypedef)
					C_TraceError(ctx, &ctx->token->trace, "cannot assign to types.");
				else if (decl->h.kind == C_AstKind_DeclExtern)
					C_TraceError(ctx, &ctx->token->trace, "cannot initialize a declaration with 'extern' storage modifier.");
				
				new_node->init = C_ParseExpr(ctx, 1, true);
			}
		}
		
		*out_should_eat_semicolon = true;
	}
	
	if (out_last)
		*out_last = last;
	
	return decl;
}

internal C_Node*
C_ParseDeclAndSemicolonIfNeeded(C_Context* ctx, C_Node** out_last, int32 options)
{
	bool32 b = false;
	C_AstNode* result = C_ParseDecl(ctx, out_last, options, &b);
	
	if (b)
		C_StreamEatToken(ctx, C_TokenKind_Semicolon);
	
	return result;
}

internal bool32
C_ParseFile(C_Context* ctx)
{
	Trace();
	
	C_AstDecl* first_node = NULL;
	C_AstDecl* last_node;
	
	ctx->token = &ctx->tokens->tokens[0];
	C_StreamDealWithPragmaToken(ctx);
	
	C_PushSymbolScope(ctx);
	
	ctx->scope->names = LittleMap_Create(ctx->persistent_arena, 1 << 14);
	ctx->scope->types = LittleMap_Create(ctx->persistent_arena, 1 << 14);
	ctx->scope->structs = LittleMap_Create(ctx->persistent_arena, 1 << 13);
	ctx->scope->unions = LittleMap_Create(ctx->persistent_arena, 1 << 13);
	ctx->scope->enums = LittleMap_Create(ctx->persistent_arena, 1 << 13);
	
	while (ctx->token->kind == C_TokenKind_Semicolon)
		C_StreamNextToken(ctx);
	first_node = C_ParseDeclAndSemicolonIfNeeded(ctx, (void*)&last_node, 2 | 4);
	
	while (ctx->token->kind != C_TokenKind_Eof)
	{
		while (ctx->token->kind == C_TokenKind_Semicolon)
			C_StreamNextToken(ctx);
		
		C_Node* new_last = NULL;
		last_node->h.next = C_ParseDeclAndSemicolonIfNeeded(ctx, &new_last, 2 | 4);
		last_node = new_last;
	}
	
	ctx->ast = first_node;
	return ctx->error_count == 0;
}
