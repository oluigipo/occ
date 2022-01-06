internal inline void
LangC_UpdateNode(LangC_Context* ctx, LangC_AstKind kind, LangC_AstNode* node)
{
	Assert(node);
	
	if (kind)
		node->kind = kind;
	node->line = ctx->lex.token.line;
	node->col = ctx->lex.token.col;
	node->lexfile = ctx->lex.file;
}

internal inline LangC_Node*
LangC_CreateNode(LangC_Context* ctx, LangC_AstKind kind, uintsize size)
{
	LangC_AstNode* result = Arena_Push(ctx->persistent_arena, size);
	LangC_UpdateNode(ctx, kind, result);
	
	return result;
}

internal void
LangC_PoisonNode(LangC_Context* ctx, LangC_AstNode* node)
{
	if (node->flags & LangC_AstFlags_Poisoned)
		return;
	
	node->flags |= LangC_AstFlags_Poisoned;
	
	// TODO(ljre): Poison children.
}

internal void
LangC_AppendNode(LangC_Context* ctx, LangC_Node** to_, LangC_Node* node)
{
	LangC_AstNode** to = (void*)to_;
	
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
LangC_NodeError(LangC_Context* ctx, LangC_Node* node_, const char* fmt, ...)
{
	LangC_AstNode* node = node_;
	
	// NOTE(ljre): If this node is already fucked up, no need to report more errors.
	if (node->flags & LangC_AstFlags_Poisoned)
		return;
	
	ctx->error_count++;
	LangC_PoisonNode(ctx, node);
	
	LangC_LexerFile* lexfile = node->lexfile;
	Print("\n");
	
	if (lexfile->included_from)
		LangC_PrintIncludeStack(lexfile->included_from, lexfile->included_line);
	
	Print("%C1%S%C0(%i:%i): %C2error%C0: ", StrFmt(lexfile->path), node->line, node->col);
	
	va_list args;
	va_start(args, fmt);
	PrintVarargs(fmt, args);
	va_end(args);
	Print("\n");
}

internal void
LangC_NodeWarning(LangC_Context* ctx, LangC_Node* node_, LangC_Warning warning, const char* fmt, ...)
{
	if (!LangC_IsWarningEnabled(ctx, warning))
		return;
	
	LangC_AstNode* node = node_;
	LangC_LexerFile* lexfile = node->lexfile;
	char* buf = Arena_End(global_arena);
	
	Arena_PushMemory(global_arena, 1, "\n");
	if (lexfile->included_from)
		LangC_PrintIncludeStackToArena(lexfile->included_from, lexfile->included_line, global_arena);
	
	Arena_Printf(global_arena, "%C1%S%C0(%i:%i): %C3warning%C0: ", StrFmt(node->lexfile->path), node->line, node->col);
	
	va_list args;
	va_start(args, fmt);
	Arena_VPrintf(global_arena, fmt, args);
	va_end(args);
	
	Arena_PushMemory(global_arena, 1, "");
	
	LangC_PushWarning(ctx, warning, buf);
}

internal LangC_SymbolScope*
LangC_PushSymbolScope(LangC_Context* ctx)
{
	LangC_SymbolScope* scope = Arena_Push(ctx->persistent_arena, sizeof(*scope));
	
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
LangC_PopSymbolScope(LangC_Context* ctx)
{
	ctx->previous_scope = ctx->scope;
	ctx->scope = ctx->scope->up;
}

internal void
LangC_TypedefDecl(LangC_Context* ctx, LangC_AstDecl* decl)
{
	LangC_Symbol* sym = Arena_Push(ctx->persistent_arena, sizeof(*sym));
	String name = decl->name;
	
	sym->kind = LangC_SymbolKind_Typename;
	sym->decl = decl;
	sym->name = name;
	
	if (!ctx->scope->types)
		ctx->scope->types = LittleMap_Create(ctx->persistent_arena, 128);
	
	LittleMap_Insert(ctx->scope->types, name, sym);
}

internal bool32
LangC_IsBeginningOfDeclOrType(LangC_Context* ctx)
{
	switch (ctx->lex.token.kind)
	{
		case LangC_TokenKind_Identifier:
		{
			String ident = ctx->lex.token.value_ident;
			uint64 hash = SimpleHash(ident);
			LangC_SymbolScope* scope = ctx->scope;
			
			while (scope)
			{
				if (scope->types && LittleMap_FetchWithCachedHash(scope->types, ident, hash))
					return true;
				
				scope = scope->up;
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

internal LangC_Node* LangC_ParseStmt(LangC_Context* ctx, LangC_Node** out_last, bool32 allow_decl);
internal LangC_Node* LangC_ParseBlock(LangC_Context* ctx, LangC_Node** out_last);
internal LangC_Node* LangC_ParseExpr(LangC_Context* ctx, int32 level, bool32 allow_init);
internal LangC_Node* LangC_ParseDecl(LangC_Context* ctx, LangC_Node** out_last, int32 options, bool32* out_should_eat_semicolon);
internal LangC_Node* LangC_ParseDeclOrExpr(LangC_Context* ctx, LangC_Node** out_last, bool32 type_only, int32 level);
internal LangC_Node* LangC_ParseDeclAndSemicolonIfNeeded(LangC_Context* ctx, LangC_Node** out_last, int32 options);
internal bool32 LangC_ParsePossibleGnuAttribute(LangC_Context* ctx, LangC_AstNode* apply_to);

internal LangC_AstDecl*
LangC_ParseEnumBody(LangC_Context* ctx)
{
	LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftCurl);
	LangC_AstDecl* result = LangC_CreateNode(ctx, LangC_AstKind_DeclEnumEntry, sizeof(LangC_AstDecl));
	LangC_AstNode* last = &result->h;
	
	if (LangC_AssertToken(&ctx->lex, LangC_TokenKind_Identifier))
		result->name = ctx->lex.token.value_ident;
	
	LangC_ParsePossibleGnuAttribute(ctx, &result->h);
	
	if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Equals))
		result->init = LangC_ParseExpr(ctx, 1, false);
	
	while (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Comma) &&
		   ctx->lex.token.kind != LangC_TokenKind_RightCurl)
	{
		LangC_AstDecl* new_node = LangC_CreateNode(ctx, LangC_AstKind_DeclEnumEntry, sizeof(LangC_AstDecl));
		
		if (LangC_AssertToken(&ctx->lex, LangC_TokenKind_Identifier))
			new_node->name = ctx->lex.token.value_ident;
		
		LangC_ParsePossibleGnuAttribute(ctx, &new_node->h);
		
		if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Equals))
			new_node->init = LangC_ParseExpr(ctx, 1, false);
		
		last = last->next = &new_node->h;
	}
	
	LangC_EatToken(&ctx->lex, LangC_TokenKind_RightCurl);
	
	return result;
}

internal LangC_AstDecl*
LangC_ParseStructBody(LangC_Context* ctx)
{
	LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftCurl);
	bool32 should_eat_semicolon = false;
	LangC_AstNode* last;
	LangC_AstDecl* result = LangC_ParseDecl(ctx, (void*)&last, 4, &should_eat_semicolon);
	
	// NOTE(ljre): Bitfields
	if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Colon))
	{
		LangC_AstAttribute* attrib = LangC_CreateNode(ctx, LangC_AstKind_AttributeBitfield, SizeofPoly(LangC_AstAttribute, bitfield));
		attrib->as->bitfield.expr = LangC_ParseExpr(ctx, 1, false);
		attrib->h.next = (void*)result->h.attributes;
		result->h.attributes = attrib;
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
			LangC_AstAttribute* attrib = LangC_CreateNode(ctx, LangC_AstKind_AttributeBitfield, SizeofPoly(LangC_AstAttribute, bitfield));
			attrib->as->bitfield.expr = LangC_ParseExpr(ctx, 1, false);
			attrib->h.next = (void*)last->attributes;
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
	void* result = NULL;
	
	if (LangC_IsBeginningOfDeclOrType(ctx))
		result = LangC_ParseDeclAndSemicolonIfNeeded(ctx, out_last, type_only);
	else
		result = LangC_ParseExpr(ctx, level, false);
	
	return result;
}

internal void*
LangC_ParseInitializerField(LangC_Context* ctx)
{
	LangC_AstExpr* result = NULL;
	LangC_AstExpr* head = NULL;
	
	for (;;)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_Dot:
			{
				LangC_AstExpr* newnode = LangC_CreateNode(ctx, LangC_AstKind_ExprInitializerMember, SizeofPoly(LangC_AstExpr, desig_entry));
				if (head)
					head = head->as->desig_entry.next = newnode;
				else
					result = head = newnode;
				
				LangC_NextToken(&ctx->lex);
				if (LangC_AssertToken(&ctx->lex, LangC_TokenKind_Identifier))
					newnode->as->desig_entry.name = ctx->lex.token.value_ident;
			} continue;
			
			case LangC_TokenKind_LeftBrkt:
			{
				LangC_AstExpr* newnode = LangC_CreateNode(ctx, LangC_AstKind_ExprInitializerIndex, SizeofPoly(LangC_AstExpr, desig_entry));
				if (head)
					head = head->as->desig_entry.next = newnode;
				else
					result = head = newnode;
				
				LangC_NextToken(&ctx->lex);
				newnode->as->desig_entry.index = LangC_ParseExpr(ctx, 0, false);
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightBrkt);
			} continue;
			
			default:
			{
				if (result)
				{
					LangC_EatToken(&ctx->lex, LangC_TokenKind_Assign);
					
					LangC_AstExpr* newnode = LangC_CreateNode(ctx, LangC_AstKind_ExprInitializerIndex, SizeofPoly(LangC_AstExpr, desig_init));
					newnode->as->desig_init.desig = result;
					newnode->as->desig_init.expr = LangC_ParseExpr(ctx, 1, true);
					
					result = newnode;
				}
				else
					result = LangC_ParseExpr(ctx, 1, true);
			} break;
		}
		
		break;
	}
	
	return result;
}

internal String
LangC_PrepareStringLiteral(LangC_Context* ctx, bool32* restrict is_wide)
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
	LangC_AstExpr* result = LangC_CreateNode(ctx, LangC_AstKind_ExprFactor, SizeofPolyAny(LangC_AstExpr));
	LangC_AstExpr* head = result;
	bool32 has_unary = false;
	bool32 dont_parse_postfix = true;
	
	for (;; has_unary = true)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_Plus: LangC_NextToken(&ctx->lex); continue; // do nothing????
			
			case LangC_TokenKind_Minus:
			{
				head->h.kind = LangC_AstKind_Expr1Negative;
				head = head->as->unary.expr = LangC_CreateNode(ctx, LangC_AstKind_ExprFactor, SizeofPolyAny(LangC_AstExpr));
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Mul:
			{
				head->h.kind = LangC_AstKind_Expr1Deref;
				head = head->as->unary.expr = LangC_CreateNode(ctx, LangC_AstKind_ExprFactor, SizeofPolyAny(LangC_AstExpr));
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_And:
			{
				head->h.kind = LangC_AstKind_Expr1Ref;
				head = head->as->unary.expr = LangC_CreateNode(ctx, LangC_AstKind_ExprFactor, SizeofPolyAny(LangC_AstExpr));
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Not:
			{
				head->h.kind = LangC_AstKind_Expr1Not;
				head = head->as->unary.expr = LangC_CreateNode(ctx, LangC_AstKind_ExprFactor, SizeofPolyAny(LangC_AstExpr));
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_LNot:
			{
				head->h.kind = LangC_AstKind_Expr1LogicalNot;
				head = head->as->unary.expr = LangC_CreateNode(ctx, LangC_AstKind_ExprFactor, SizeofPolyAny(LangC_AstExpr));
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Inc:
			{
				head->h.kind = LangC_AstKind_Expr1PrefixInc;
				head = head->as->unary.expr = LangC_CreateNode(ctx, LangC_AstKind_ExprFactor, SizeofPolyAny(LangC_AstExpr));
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Dec:
			{
				head->h.kind = LangC_AstKind_Expr1PrefixDec;
				head = head->as->unary.expr = LangC_CreateNode(ctx, LangC_AstKind_ExprFactor, SizeofPolyAny(LangC_AstExpr));
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Sizeof:
			{
				head->h.kind = LangC_AstKind_Expr1Sizeof;
				
				LangC_NextToken(&ctx->lex);
				if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_LeftParen))
				{
					if (LangC_IsBeginningOfDeclOrType(ctx))
					{
						head->h.kind = LangC_AstKind_Expr1SizeofType;
						
						LangC_UpdateNode(ctx, LangC_AstKind_Decl, &head->h);
						head->as->sizeof_type.type = LangC_ParseDecl(ctx, NULL, 1, NULL);
					}
					else
					{
						head->as->sizeof_expr.expr = LangC_ParseExpr(ctx, 0, false);
						LangC_UpdateNode(ctx, 0, &head->h);
						head = head->as->sizeof_expr.expr;
					}
					
					LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
					goto ignore_factor;
				}
				else
				{
					head = head->as->sizeof_expr.expr = LangC_CreateNode(ctx, LangC_AstKind_ExprFactor, SizeofPolyAny(LangC_AstExpr));
				}
			} continue;
			
			case LangC_TokenKind_LeftParen:
			{
				LangC_NextToken(&ctx->lex);
				if (LangC_IsBeginningOfDeclOrType(ctx))
				{
					LangC_AstType* type = LangC_ParseDecl(ctx, NULL, 1, NULL);
					
					LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
					if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_LeftCurl))
					{
						LangC_UpdateNode(ctx, 0, &head->h);
						head->h.kind = LangC_AstKind_ExprCompoundLiteral;
						head->as->compound.type = type;
						
						goto parse_init;
					}
					else
					{
						head->h.kind = LangC_AstKind_Expr1Cast;
						head->as->cast.to = type;
						head = head->as->cast.expr = LangC_CreateNode(ctx, LangC_AstKind_ExprFactor, SizeofPolyAny(LangC_AstExpr));
					}
				}
				else
				{
					// yes. this is a "leak"
					OurMemCopy(head, LangC_ParseExpr(ctx, 0, false), SizeofPolyAny(LangC_AstExpr));
					
					LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
					goto ignore_factor;
				}
			} continue;
		}
		
		break;
	}
	
	if (allow_init && ctx->lex.token.kind == LangC_TokenKind_LeftCurl)
	{
		LangC_UpdateNode(ctx, 0, &head->h);
		head->h.kind = LangC_AstKind_ExprInitializer;
		if (has_unary)
			LangC_LexerError(&ctx->lex, "expected expression.");
		
		LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftCurl);
		
		if (0)
		{
			parse_init: dont_parse_postfix = false;
		}
		
		head->as->init.exprs = LangC_ParseInitializerField(ctx);
		LangC_AstNode* last_init = (void*)head->as->init.exprs;
		
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
				LangC_UpdateNode(ctx, LangC_AstKind_ExprInt, &head->h);
				head->value_int = ctx->lex.token.value_int;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_LIntLiteral:
			{
				LangC_UpdateNode(ctx, LangC_AstKind_ExprLInt, &head->h);
				head->value_int = ctx->lex.token.value_int;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_LLIntLiteral:
			{
				LangC_UpdateNode(ctx, LangC_AstKind_ExprLLInt, &head->h);
				head->value_int = ctx->lex.token.value_int;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_UintLiteral:
			{
				LangC_UpdateNode(ctx, LangC_AstKind_ExprUInt, &head->h);
				head->value_uint = ctx->lex.token.value_uint;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_LUintLiteral:
			{
				LangC_UpdateNode(ctx, LangC_AstKind_ExprULInt, &head->h);
				head->value_uint = ctx->lex.token.value_uint;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_LLUintLiteral:
			{
				LangC_UpdateNode(ctx, LangC_AstKind_ExprULLInt, &head->h);
				head->value_uint = ctx->lex.token.value_uint;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_StringLiteral:
			case LangC_TokenKind_WideStringLiteral:
			{
				bool32 is_wide;
				
				LangC_UpdateNode(ctx, LangC_AstKind_ExprString, &head->h);
				head->value_str = LangC_PrepareStringLiteral(ctx, &is_wide);
				
				if (is_wide)
					head->h.kind = LangC_AstKind_ExprWideString;
			} break;
			
			case LangC_TokenKind_FloatLiteral:
			{
				LangC_UpdateNode(ctx, LangC_AstKind_ExprFloat, &head->h);
				head->value_float = ctx->lex.token.value_float;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_DoubleLiteral:
			{
				LangC_UpdateNode(ctx, LangC_AstKind_ExprDouble, &head->h);
				head->value_double = ctx->lex.token.value_double;
				
				LangC_NextToken(&ctx->lex);
			} break;
			
			case LangC_TokenKind_Identifier:
			{
				if (LangC_IsBeginningOfDeclOrType(ctx))
					LangC_LexerError(&ctx->lex, "unexpected typename.");
				
				LangC_UpdateNode(ctx, LangC_AstKind_ExprIdent, &head->h);
				head->as->ident.name = ctx->lex.token.value_ident;
				
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
				LangC_AstExpr* new_node = LangC_CreateNode(ctx, LangC_AstKind_Expr1PostfixInc, SizeofPolyAny(LangC_AstExpr));
				new_node->as->unary.expr = head;
				head = new_node;
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_Dec:
			{
				LangC_AstExpr* new_node = LangC_CreateNode(ctx, LangC_AstKind_Expr1PostfixInc, SizeofPolyAny(LangC_AstExpr));
				new_node->as->unary.expr = head;
				head = new_node;
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_LeftParen:
			{
				LangC_AstExpr* new_node = LangC_CreateNode(ctx, LangC_AstKind_Expr2Call, SizeofPolyAny(LangC_AstExpr));
				new_node->as->binary.left = head;
				head = new_node;
				
				LangC_NextToken(&ctx->lex);
				if (ctx->lex.token.kind != LangC_TokenKind_RightParen)
				{
					LangC_AstNode* last_param = LangC_ParseExpr(ctx, 1, false);
					head->as->binary.right = (void*)last_param;
					
					while (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Comma))
					{
						last_param = last_param->next = LangC_ParseExpr(ctx, 1, false);
					}
				}
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			} continue;
			
			case LangC_TokenKind_LeftBrkt:
			{
				LangC_AstExpr* new_node = LangC_CreateNode(ctx, LangC_AstKind_Expr2Index, SizeofPolyAny(LangC_AstExpr));
				new_node->as->binary.left = head;
				head = new_node;
				
				LangC_NextToken(&ctx->lex);
				head->as->binary.right = LangC_ParseExpr(ctx, 0, false);
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightBrkt);
			} continue;
			
			case LangC_TokenKind_Dot:
			{
				LangC_AstKind kind = LangC_AstKind_Expr2Access;
				
				if (0)
				{
					case LangC_TokenKind_Arrow: kind = LangC_AstKind_Expr2DerefAccess;
				}
				
				LangC_AstExpr* new_node = LangC_CreateNode(ctx, kind, SizeofPolyAny(LangC_AstExpr));
				new_node->as->access.expr = head;
				head = new_node;
				
				LangC_NextToken(&ctx->lex);
				if (LangC_AssertToken(&ctx->lex, LangC_TokenKind_Identifier))
				{
					head->as->access.field = ctx->lex.token.value_ident;
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
	LangC_AstExpr* result = LangC_ParseExprFactor(ctx, allow_init);
	
	LangC_OperatorPrecedence prec;
	while (prec = LangC_operators_precedence[ctx->lex.token.kind],
		   prec.level > level)
	{
		int32 op = LangC_token_to_op[ctx->lex.token.kind];
		LangC_NextToken(&ctx->lex);
		LangC_AstExpr* right = LangC_ParseExprFactor(ctx, false);
		
		LangC_TokenKind lookahead = ctx->lex.token.kind;
		LangC_OperatorPrecedence lookahead_prec = LangC_operators_precedence[lookahead];
		
		while (lookahead_prec.level > 0 &&
			   (lookahead_prec.level > prec.level ||
				(lookahead_prec.level == prec.level && lookahead_prec.right2left))) {
			LangC_NextToken(&ctx->lex);
			
			LangC_AstExpr* tmp;
			
			if (lookahead == LangC_TokenKind_QuestionMark) {
				tmp = LangC_CreateNode(ctx, LangC_AstKind_Expr3Condition, SizeofPolyAny(LangC_AstExpr));
				tmp->as->ternary.left = right;
				tmp->as->ternary.middle = LangC_ParseExpr(ctx, 0, false);
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_Colon);
				tmp->as->ternary.right = LangC_ParseExpr(ctx, level - 1, false);
			} else {
				tmp = LangC_CreateNode(ctx, LangC_token_to_op[lookahead], SizeofPolyAny(LangC_AstExpr));
				tmp->as->binary.left = right;
				tmp->as->binary.right = LangC_ParseExpr(ctx, level + 1, false);
			}
			
			lookahead = ctx->lex.token.kind;
			lookahead_prec = LangC_operators_precedence[lookahead];
			right = tmp;
		}
		
		LangC_AstExpr* tmp = LangC_CreateNode(ctx, op, SizeofPolyAny(LangC_AstExpr));
		
		if (op == LangC_AstKind_Expr3Condition)
		{
			tmp->as->ternary.left = result;
			tmp->as->ternary.middle = right;
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Colon);
			tmp->as->ternary.right = LangC_ParseExpr(ctx, prec.level, false);
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
LangC_ParsePossibleGnuAttribute(LangC_Context* ctx, LangC_AstNode* apply_to)
{
	if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_GccAttribute))
	{
		LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
		LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
		
		if (ctx->lex.token.kind != LangC_TokenKind_RightParen)
		{
			do
			{
				if (!LangC_IsKeyword(ctx->lex.token.kind) && ctx->lex.token.kind != LangC_TokenKind_Identifier)
					break;
				
				LangC_AstAttribute* attrib = NULL;
				String name = ctx->lex.token.value_ident;
				if (MatchCString("packed", name) || MatchCString("__packed__", name))
				{
					attrib = LangC_CreateNode(ctx, LangC_AstKind_AttributePacked, SizeofPoly(LangC_AstAttribute, aligned));
					
					LangC_NextToken(&ctx->lex);
				}
				else if (MatchCString("aligned", name))
				{
					attrib = LangC_CreateNode(ctx, LangC_AstKind_AttributePacked, SizeofPoly(LangC_AstAttribute, aligned));
					
					LangC_NextToken(&ctx->lex);
					if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_LeftParen))
					{
						attrib->as->aligned.expr = LangC_ParseExpr(ctx, 0, false);
						
						LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
					}
				}
				
				if (attrib)
				{
					attrib->h.next = (void*)apply_to->attributes;
					apply_to->attributes = attrib;
				}
			}
			while (!LangC_EatToken(&ctx->lex, LangC_TokenKind_Comma));
		}
		
		LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
		LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
		
		return true;
	}
	
	return false;
}

internal bool32
LangC_ParsePossibleMsvcDeclspec(LangC_Context* ctx, LangC_AstNode* apply_to)
{
	if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_MsvcDeclspec))
	{
		LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
		
		// TODO(ljre): Proper parsing
		while (ctx->lex.token.kind != LangC_TokenKind_RightParen)
			LangC_NextToken(&ctx->lex);
		
		LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
		
		return true;
	}
	
	return false;
}

internal void*
LangC_ParseStmt(LangC_Context* ctx, LangC_Node** out_last, bool32 allow_decl)
{
	LangC_AstStmt* result = NULL;
	LangC_Node* last = NULL;
	
	switch (ctx->lex.token.kind)
	{
		case LangC_TokenKind_LeftCurl:
		{
			result = LangC_ParseBlock(ctx, NULL);
		} break;
		
		case LangC_TokenKind_If:
		{
			last = result = LangC_CreateNode(ctx, LangC_AstKind_StmtIf, SizeofPoly(LangC_AstStmt, generic));
			
			LangC_NextToken(&ctx->lex);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			result->as->generic.leafs[0] = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			result->as->generic.leafs[1] = LangC_ParseStmt(ctx, NULL, false);
			
			if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Else))
				result->as->generic.leafs[2] = LangC_ParseStmt(ctx, NULL, false);
		} break;
		
		case LangC_TokenKind_While:
		{
			last = result = LangC_CreateNode(ctx, LangC_AstKind_StmtWhile, SizeofPoly(LangC_AstStmt, generic));
			
			LangC_NextToken(&ctx->lex);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			result->as->generic.leafs[0] = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			result->as->generic.leafs[1] = LangC_ParseStmt(ctx, NULL, false);
		} break;
		
		case LangC_TokenKind_For:
		{
			last = result = LangC_CreateNode(ctx, LangC_AstKind_StmtFor, SizeofPoly(LangC_AstStmt, generic));
			
			LangC_NextToken(&ctx->lex);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			if (ctx->lex.token.kind != LangC_TokenKind_Semicolon)
				result->as->generic.leafs[0] = LangC_ParseDeclOrExpr(ctx, NULL, false, 0);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
			if (ctx->lex.token.kind != LangC_TokenKind_Semicolon)
				result->as->generic.leafs[1] = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
			if (ctx->lex.token.kind != LangC_TokenKind_RightParen)
				result->as->generic.leafs[2] = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			result->as->generic.leafs[3] = LangC_ParseStmt(ctx, NULL, false);
		} break;
		
		case LangC_TokenKind_Do:
		{
			last = result = LangC_CreateNode(ctx, LangC_AstKind_StmtDoWhile, SizeofPoly(LangC_AstStmt, generic));
			
			LangC_NextToken(&ctx->lex);
			result->as->generic.leafs[0] = LangC_ParseStmt(ctx, NULL, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_While);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			result->as->generic.leafs[1] = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
		} break;
		
		case LangC_TokenKind_GccAttribute:
		{
			last = result = LangC_CreateNode(ctx, LangC_AstKind_StmtEmpty, sizeof(LangC_AstStmt));
			LangC_ParsePossibleGnuAttribute(ctx, &result->h);
			
			if (allow_decl && LangC_IsBeginningOfDeclOrType(ctx))
			{
				LangC_AstAttribute* attribs = result->h.attributes;
				result = LangC_ParseDeclAndSemicolonIfNeeded(ctx, (void*)&last, 4);
				
				for (LangC_AstNode* it = (void*)result; it; it = it->next)
					LangC_AppendNode(ctx, (void*)&it->attributes, attribs);
			}
			else
				LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
		} break;
		
		case LangC_TokenKind_Semicolon:
		{
			last = result = LangC_CreateNode(ctx, LangC_AstKind_StmtEmpty, sizeof(LangC_AstStmt));
			
			LangC_NextToken(&ctx->lex);
		} break;
		
		case LangC_TokenKind_Switch:
		{
			last = result = LangC_CreateNode(ctx, LangC_AstKind_StmtSwitch, SizeofPoly(LangC_AstStmt, generic));
			
			LangC_NextToken(&ctx->lex);
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			result->as->generic.leafs[0] = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
			result->as->generic.leafs[1] = LangC_ParseStmt(ctx, NULL, false);
		} break;
		
		case LangC_TokenKind_Return:
		{
			last = result = LangC_CreateNode(ctx, LangC_AstKind_StmtReturn, SizeofPoly(LangC_AstStmt, generic));
			
			LangC_NextToken(&ctx->lex);
			result->as->generic.leafs[0] = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
		} break;
		
		case LangC_TokenKind_Case:
		{
			result = LangC_CreateNode(ctx, LangC_AstKind_StmtCase, SizeofPoly(LangC_AstStmt, generic));
			
			LangC_NextToken(&ctx->lex);
			result->as->generic.leafs[0] = LangC_ParseExpr(ctx, 0, false);
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Colon);
			result->as->generic.leafs[1] = LangC_ParseStmt(ctx, NULL, false);
		} break;
		
		case LangC_TokenKind_Goto:
		{
			last = result = LangC_CreateNode(ctx, LangC_AstKind_StmtGoto, SizeofPoly(LangC_AstStmt, go_to));
			
			LangC_NextToken(&ctx->lex);
			if (LangC_AssertToken(&ctx->lex, LangC_TokenKind_Identifier))
			{
				result->as->go_to.label_name = ctx->lex.token.value_ident;
				
				LangC_NextToken(&ctx->lex);
			}
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
		} break;
		
		case LangC_TokenKind_MsvcAsm:
		{
			// TODO(ljre): Proper __asm parsing. WTF is this syntax??????????
			last = result = LangC_CreateNode(ctx, LangC_AstKind_StmtEmpty, sizeof(LangC_AstStmt));
			
			LangC_NextToken(&ctx->lex);
			if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_LeftCurl))
			{
				while (ctx->lex.token.kind != LangC_TokenKind_RightCurl)
					LangC_NextToken(&ctx->lex);
				
				LangC_NextToken(&ctx->lex);
			}
			
			LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Semicolon);
		} break;
		
		case LangC_TokenKind_GccAsm:
		{
			last = result = LangC_CreateNode(ctx, LangC_AstKind_StmtGccAsm, SizeofPoly(LangC_AstStmt, gcc_asm));
			
			LangC_NextToken(&ctx->lex);
			for (;; LangC_NextToken(&ctx->lex))
			{
				switch (ctx->lex.token.kind)
				{
					case LangC_TokenKind_Volatile: result->h.flags |= LangC_AstFlags_GccAsmVolatile; continue;
					case LangC_TokenKind_Inline: result->h.flags |= LangC_AstFlags_GccAsmInline; continue;
					case LangC_TokenKind_Goto: result->h.flags |= LangC_AstFlags_GccAsmGoto; continue;
				}
				
				break;
			}
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_LeftParen);
			if (LangC_AssertToken(&ctx->lex, LangC_TokenKind_StringLiteral))
			{
				result->as->gcc_asm.leafs[0] = LangC_ParseExprFactor(ctx, false);
				
				for (int32 i = 1; i <= 4; ++i)
				{
					if (!LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Colon))
						break;
					
					if (ctx->lex.token.kind == LangC_TokenKind_StringLiteral ||
						ctx->lex.token.kind == LangC_TokenKind_Identifier)
					{
						LangC_AstNode* head = LangC_ParseExprFactor(ctx, false);
						result->as->gcc_asm.leafs[i] = (void*)head;
						
						while (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Comma))
							head = head->next = LangC_ParseExprFactor(ctx, false);
					}
				}
			}
			
			LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
		} break;
		
		default:
		{
			if (allow_decl && LangC_IsBeginningOfDeclOrType(ctx))
				result = LangC_ParseDeclAndSemicolonIfNeeded(ctx, &last, 4);
			else
			{
				last = result = LangC_ParseExpr(ctx, 0, false);
				
				if (result->h.kind == LangC_AstKind_ExprIdent && LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Colon))
				{
					result->h.kind = LangC_AstKind_StmtLabel;
					result->as->label.stmt = LangC_ParseStmt(ctx, NULL, false);
					
					if (result->as->label.stmt->h.kind == LangC_AstKind_StmtEmpty)
						result->h.attributes = result->as->label.stmt->h.attributes; // NOTE(ljre): For GCC __attribute__ only.
				}
				else
				{
					LangC_AstStmt* s = LangC_CreateNode(ctx, LangC_AstKind_StmtExpr, SizeofPoly(LangC_AstStmt, expr));
					s->as->expr.expr = (void*)result;
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
	LangC_AstStmt* result = LangC_CreateNode(ctx, LangC_AstKind_StmtCompound, SizeofPoly(LangC_AstStmt, compound));
	LangC_AstNode* last = NULL;
	
	if (ctx->lex.token.kind == LangC_TokenKind_RightCurl)
	{
		// NOTE(ljre): Not needed
		//result->stmt = LangC_CreateNode(ctx, LangC_NodeKind_EmptyStmt);
		
		LangC_EatToken(&ctx->lex, LangC_TokenKind_RightCurl);
	}
	else
	{
		LangC_PushSymbolScope(ctx);
		result->as->compound.stmts = LangC_ParseStmt(ctx, (void*)&last, true);
		
		while (!LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_RightCurl))
		{
			LangC_Node* new_last;
			LangC_AstNode* new_node = LangC_ParseStmt(ctx, &new_last, true);
			last->next = new_node;
			last = new_last;
		}
		
		LangC_PopSymbolScope(ctx);
	}
	
	if (out_last)
		*out_last = result;
	
	return result;
}

internal LangC_AstType*
LangC_ParseRestOfDeclIt(LangC_Context* ctx, LangC_AstType** head, uint32* flags_head, LangC_AstAttribute** attrib_head, LangC_AstDecl* decl, bool32 type_only, bool32 is_global)
{
	LangC_AstType** result = head;
	uint32 saved_flags = 0;
	LangC_AstAttribute* saved_attrib = NULL;
	
	for (;;)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_Mul:
			{
				LangC_NextToken(&ctx->lex);
				
				LangC_AstType* newtype = LangC_CreateNode(ctx, LangC_AstKind_TypePointer, SizeofPoly(LangC_AstType, ptr));
				*head = newtype;
				
				head = &newtype->as->ptr.to;
				flags_head = &newtype->h.flags;
				attrib_head = &newtype->h.attributes;
			} continue;
			
			case LangC_TokenKind_Const:
			{
				LangC_NextToken(&ctx->lex);
				
				*flags_head |= LangC_AstFlags_Const;
			} continue;
			
			case LangC_TokenKind_Restrict:
			{
				LangC_NextToken(&ctx->lex);
				
				*flags_head |= LangC_AstFlags_Restrict;
			} continue;
			
			// NOTE(ljre): MSVC calling convention keywords
			{
				uint32 flags;
				
				/**/ if (0) case LangC_TokenKind_MsvcCdecl: flags = LangC_AstFlags_MsvcCdecl;
				else if (0) case LangC_TokenKind_MsvcStdcall: flags = LangC_AstFlags_MsvcStdcall;
				else if (0) case LangC_TokenKind_MsvcVectorcall: flags = LangC_AstFlags_MsvcVectorcall;
				else if (0) case LangC_TokenKind_MsvcFastcall: flags = LangC_AstFlags_MsvcFastcall;
				
				LangC_AstAttribute* attrib = LangC_CreateNode(ctx, LangC_AstKind_AttributeCallconv, sizeof(LangC_AstAttribute));
				attrib->h.flags |= flags;
				attrib->h.next = (void*)*attrib_head;
				*attrib_head = attrib;
				
				LangC_NextToken(&ctx->lex);
			} continue;
			
			case LangC_TokenKind_LeftParen:
			{
				LangC_NextToken(&ctx->lex);
				*head = LangC_ParseRestOfDeclIt(ctx, head, &saved_flags, &saved_attrib, decl, type_only, is_global);
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightParen);
				
				goto after_identifier;
			} break;
		}
		
		break;
	}
	
	if (!type_only && ctx->lex.token.kind == LangC_TokenKind_Identifier)
	{
		decl->name = ctx->lex.token.value_ident;
		LangC_NextToken(&ctx->lex);
	}
	
	after_identifier:;
	
	for (;;)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_LeftParen:
			{
				LangC_NextToken(&ctx->lex);
				LangC_AstType* newtype = LangC_CreateNode(ctx, LangC_AstKind_TypeFunction, SizeofPoly(LangC_AstType, function));
				
				if (saved_attrib)
				{
					newtype->h.attributes = saved_attrib;
					
					LangC_AppendNode(ctx, (void*)&saved_attrib->h.next, *attrib_head);
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
				
				if (ctx->lex.token.kind == LangC_TokenKind_RightParen)
					newtype->h.flags |= LangC_AstFlags_VarArgs;
				else
				{
					LangC_AstNode* last_param;
					newtype->as->function.params = LangC_ParseDecl(ctx, (void*)&last_param, 8, NULL);
					
					while (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Comma))
					{
						if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_VarArgs))
						{
							newtype->h.flags |= LangC_AstFlags_VarArgs;
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
				LangC_AstType* newtype = LangC_CreateNode(ctx, LangC_AstKind_TypeVlaArray, SizeofPolyAny(LangC_AstType));
				
				if (saved_attrib)
				{
					newtype->h.attributes = saved_attrib;
					
					LangC_AppendNode(ctx, (void*)&saved_attrib->h.next, *attrib_head);
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
				
				if (ctx->lex.token.kind != LangC_TokenKind_RightBrkt)
					newtype->as->vla_array.length = LangC_ParseExpr(ctx, 0, false);
				
				LangC_EatToken(&ctx->lex, LangC_TokenKind_RightBrkt);
			} continue;
		}
		
		break;
	}
	
	return *result;
}

internal LangC_AstType*
LangC_ParseRestOfDecl(LangC_Context* ctx, LangC_AstType* base, LangC_AstDecl* decl, bool32 type_only, bool32 is_global)
{
	uint32 flags = 0;
	LangC_AstAttribute* attrib = NULL;
	LangC_AstType* head = base;
	
	LangC_AstType* result = LangC_ParseRestOfDeclIt(ctx, &head, &flags, &attrib, decl, type_only, is_global);
	
	if (flags || attrib)
	{
		LangC_AstType* tmp = LangC_CreateNode(ctx, base->h.kind, SizeofPolyAny(LangC_AstType));
		*tmp = *base;
		*tmp->as = *base->as;
		
		tmp->h.flags |= flags;
		
		if (attrib)
		{
			LangC_AstAttribute* saved_attrib = tmp->h.attributes;
			tmp->h.attributes = attrib;
			if (saved_attrib)
				LangC_AppendNode(ctx, (void*)&tmp->h.attributes, saved_attrib);
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
internal LangC_Node*
LangC_ParseDecl(LangC_Context* ctx, LangC_Node** out_last, int32 options, bool32* out_should_eat_semicolon)
{
	Assert(!(options & 4) || out_should_eat_semicolon);
	
	LangC_AstDecl* decl = NULL;
	if (!(options & 1))
		decl = LangC_CreateNode(ctx, LangC_AstKind_Decl, sizeof(LangC_AstDecl));
	
	LangC_AstType* base = LangC_CreateNode(ctx, LangC_AstKind_Type, SizeofPolyAny(LangC_AstType));
	LangC_AstAttribute* callconv = NULL;
	
	beginning:;
	// Parse Base Type
	for (;;)
	{
		switch (ctx->lex.token.kind)
		{
			case LangC_TokenKind_Auto:
			{
				if (options & 2)
					LangC_LexerError(&ctx->lex, "cannot use automatic storage for global object.");
				
				if (options & 1)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->h.kind != LangC_AstKind_Decl)
					LangC_LexerError(&ctx->lex, "invalid use of multiple storage modifiers.");
				else
					decl->h.kind = LangC_AstKind_DeclAuto;
			} break;
			
			case LangC_TokenKind_Char:
			{
				if (base->h.kind != LangC_AstKind_Type)
					LangC_LexerError(&ctx->lex, "object cannot have more than one type; did you miss a semicolon?");
				else
					base->h.kind |= LangC_AstKind_TypeChar;
			} break;
			
			case LangC_TokenKind_Const:
			{
				if (base->h.flags & LangC_AstFlags_Const)
					LangC_LexerError(&ctx->lex, "too much constness.");
				
				base->h.flags |= LangC_AstFlags_Const;
			} break;
			
			case LangC_TokenKind_Double:
			{
				if (base->h.kind != LangC_AstKind_Type)
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				else
					base->h.kind = LangC_AstKind_TypeDouble;
			} break;
			
			case LangC_TokenKind_Extern:
			{
				if (options & 1)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->h.kind != LangC_AstKind_Decl)
					LangC_LexerError(&ctx->lex, "invalid use of multiple storage modifiers.");
				else
					decl->h.flags |= LangC_AstKind_DeclExtern;
			} break;
			
			case LangC_TokenKind_Enum:
			{
				if (base->h.kind != LangC_AstKind_Type)
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				
				base->h.kind = LangC_AstKind_TypeEnum;
				
				LangC_NextToken(&ctx->lex);
				if (ctx->lex.token.kind == LangC_TokenKind_Identifier)
				{
					base->as->enumerator.name = ctx->lex.token.value_ident;
					
					LangC_NextToken(&ctx->lex);
				}
				
				if (ctx->lex.token.kind == LangC_TokenKind_LeftCurl)
					base->as->enumerator.entries = LangC_ParseEnumBody(ctx);
				
				LangC_ParsePossibleGnuAttribute(ctx, (void*)base);
			} continue;  // ignore the LangC_NextToken() at the end.
			
			case LangC_TokenKind_Float:
			{
				if (base->h.kind != LangC_AstKind_Type)
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				else
					base->h.kind = LangC_AstKind_TypeFloat;
			} break;
			
			case LangC_TokenKind_GccExtension:
			{
				// TODO(ljre):
			} break;
			
			case LangC_TokenKind_Int:
			{
				if (base->h.kind != LangC_AstKind_Type)
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				else
					base->h.kind = LangC_AstKind_TypeInt;
			} break;
			
			case LangC_TokenKind_Inline:
			{
				if (options & 1)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->h.flags & LangC_AstFlags_Inline)
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'inline'.");
				
				decl->h.flags |= LangC_AstFlags_Inline;
			} break;
			
			case LangC_TokenKind_Long:
			{
				if (base->h.flags & LangC_AstFlags_LongLong)
					LangC_LexerError(&ctx->lex, "too long for me.");
				else if (base->h.flags & LangC_AstFlags_Short)
					LangC_LexerError(&ctx->lex, "'long' does not work with 'short'.");
				else if (base->h.flags & LangC_AstFlags_Long)
				{
					// combination of long and short flags is long long
					base->h.flags &= ~LangC_AstFlags_Long;
					base->h.flags |= LangC_AstFlags_LongLong;
				}
				else
					base->h.flags |= LangC_AstFlags_Long;
			} break;
			
			case LangC_TokenKind_MsvcDeclspec:
			{
				if (options & 1)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					
					LangC_AstNode dummy = { 0 };
					LangC_ParsePossibleMsvcDeclspec(ctx, &dummy);
					break;
				}
				
				LangC_ParsePossibleMsvcDeclspec(ctx, (void*)decl);
			} break;
			
			// NOTE(ljre): MSVC calling convention keywords
			{
				uint32 flags;
				
				/**/ if (0) case LangC_TokenKind_MsvcCdecl: flags = LangC_AstFlags_MsvcCdecl;
				else if (0) case LangC_TokenKind_MsvcStdcall: flags = LangC_AstFlags_MsvcStdcall;
				else if (0) case LangC_TokenKind_MsvcVectorcall: flags = LangC_AstFlags_MsvcVectorcall;
				else if (0) case LangC_TokenKind_MsvcFastcall: flags = LangC_AstFlags_MsvcFastcall;
				
				if (!callconv)
				{
					callconv = LangC_CreateNode(ctx, LangC_AstKind_AttributeCallconv, sizeof(*callconv));
					callconv->h.flags |= flags;
				}
				else
				{
					// TODO(ljre): Error
				}
			} break;
			
			case LangC_TokenKind_Register:
			{
				if (options & 1)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->h.kind != LangC_AstKind_Decl)
					LangC_LexerError(&ctx->lex, "invalid use of multiple storage modifiers.");
				else
					decl->h.flags |= LangC_AstKind_DeclRegister;
			} break;
			
			case LangC_TokenKind_Restrict:
			{
				if (base->h.flags & LangC_AstFlags_Restrict)
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'restrict'.");
				
				base->h.flags |= LangC_AstFlags_Restrict;
			} break;
			
			case LangC_TokenKind_Short:
			{
				if (base->h.flags & (LangC_AstFlags_Long | LangC_AstFlags_LongLong))
					LangC_LexerError(&ctx->lex, "'short' does not work with 'long'.");
				else if (base->h.flags & LangC_AstFlags_Short)
					LangC_LexerError(&ctx->lex, "too short for me.\n");
				else
					base->h.flags |= LangC_AstFlags_Short;
			} break;
			
			case LangC_TokenKind_Signed:
			{
				if (base->h.flags & LangC_AstFlags_Signed)
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'signed'.");
				else if (base->h.flags & LangC_AstFlags_Unsigned)
					LangC_LexerError(&ctx->lex, "'signed' does not work with 'unsigned'.");
				else
					base->h.flags |= LangC_AstFlags_Signed;
			} break;
			
			case LangC_TokenKind_Static:
			{
				if (options & 1)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->h.kind != LangC_AstKind_Decl)
					LangC_LexerError(&ctx->lex, "invalid use of multiple storage modifiers.");
				else
					decl->h.kind = LangC_AstKind_DeclStatic;
			} break;
			
			case LangC_TokenKind_Struct:
			{
				if (base->h.kind != LangC_AstKind_Type)
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				
				base->h.kind = LangC_AstKind_TypeStruct;
				
				LangC_NextToken(&ctx->lex);
				if (ctx->lex.token.kind == LangC_TokenKind_Identifier)
				{
					base->as->structure.name = ctx->lex.token.value_ident;
					
					LangC_NextToken(&ctx->lex);
				}
				
				if (ctx->lex.token.kind == LangC_TokenKind_LeftCurl)
					base->as->structure.body = LangC_ParseStructBody(ctx);
				
				LangC_ParsePossibleGnuAttribute(ctx, (void*)base);
			} continue;  // ignore the LangC_NextToken() at the end.
			
			case LangC_TokenKind_Typedef:
			{
				if (options & 1)
				{
					LangC_LexerError(&ctx->lex, "expected a type.");
					break;
				}
				
				if (decl->h.kind != LangC_AstKind_Decl)
					LangC_LexerError(&ctx->lex, "invalid use of multiple storage modifiers.");
				else
					decl->h.kind = LangC_AstKind_DeclTypedef;
			} break;
			
			case LangC_TokenKind_Union:
			{
				if (base->h.kind != LangC_AstKind_Type)
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				
				base->h.kind = LangC_AstKind_TypeUnion;
				
				LangC_NextToken(&ctx->lex);
				if (ctx->lex.token.kind == LangC_TokenKind_Identifier)
				{
					base->as->structure.name = ctx->lex.token.value_ident;
					
					LangC_NextToken(&ctx->lex);
				}
				
				if (ctx->lex.token.kind == LangC_TokenKind_LeftCurl)
					base->as->structure.body = LangC_ParseStructBody(ctx);
				
				LangC_ParsePossibleGnuAttribute(ctx, (void*)base);
			} continue; // ignore the LangC_NextToken() at the end.
			
			case LangC_TokenKind_Unsigned:
			{
				if (base->h.flags & LangC_AstFlags_Unsigned)
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'unsigned'.");
				else if (base->h.flags & LangC_AstFlags_Signed)
					LangC_LexerError(&ctx->lex, "'unsigned' does not work with 'signed'.");
				else
					base->h.flags |= LangC_AstFlags_Unsigned;
			} break;
			
			case LangC_TokenKind_Void:
			{
				if (base->h.kind != LangC_AstKind_Type)
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				else
					base->h.kind |= LangC_AstKind_TypeVoid;
			} break;
			
			case LangC_TokenKind_Volatile:
			{
				if (base->h.flags & LangC_AstFlags_Volatile)
					LangC_LexerError(&ctx->lex, "repeated use of keyword 'volatile'.");
				
				base->h.flags |= LangC_AstFlags_Volatile;
			} break;
			
			case LangC_TokenKind_Bool:
			{
				if (base->h.kind != LangC_AstKind_Type)
					LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
				else
					base->h.kind = LangC_AstKind_TypeBool;
			} break;
			
			case LangC_TokenKind_Complex:
			{
				if (base->h.flags & LangC_AstFlags_Complex)
					LangC_LexerError(&ctx->lex, "repeated use of keyword '_Complex'.");
				
				base->h.flags |= LangC_AstFlags_Complex;
			} break;
			
			case LangC_TokenKind_Identifier:
			{
				if (LangC_IsBeginningOfDeclOrType(ctx))
				{
					if (base->h.kind != LangC_AstKind_Type)
						LangC_LexerError(&ctx->lex, "object cannot have more than one type.");
					
					base->h.kind = LangC_AstKind_TypeTypename;
					base->as->typedefed.name = ctx->lex.token.value_ident;
					break;
				}
				else if (options & 1)
					LangC_LexerError(&ctx->lex, "'%S' is not a typename", StrFmt(ctx->lex.token.value_ident));
			} /* fallthrough */
			
			default: goto out_of_loop;
		}
		
		LangC_NextToken(&ctx->lex);
	}
	
	out_of_loop:;
	
	bool32 implicit_int = false;
	if (base->h.kind == LangC_AstKind_Type)
	{
		if (base->h.flags & (LangC_AstFlags_Unsigned |
							 LangC_AstFlags_Signed |
							 LangC_AstFlags_Short |
							 LangC_AstFlags_Long |
							 LangC_AstFlags_LongLong))
		{
			base->h.kind = LangC_AstKind_TypeInt;
		}
		else
			implicit_int = true;
	}
	
	right_before_parsing_rest_of_decl:;
	LangC_AstType* type = LangC_ParseRestOfDecl(ctx, base, decl, options & 1, options & 2);
	
	if (implicit_int)
	{
		bool32 reported = false;
		
		if (type->h.kind >= LangC_AstKind_TypeChar && type->h.kind <= LangC_AstKind_TypeEnum)
		{
			if (options & 1)
			{
				if (ctx->lex.token.kind == LangC_TokenKind_Identifier)
				{
					reported = true;
					String name = ctx->lex.token.value_ident;
					LangC_LexerError(&ctx->lex, "'%S' is not a typename", StrFmt(name));
					LangC_NextToken(&ctx->lex);
					
					goto beginning;
				}
			}
			else if (LangC_IsBeginningOfDeclOrType(ctx))
			{
				reported = true;
				LangC_LexerError(&ctx->lex, "'%S' is not a typename", StrFmt(decl->name));
				
				goto beginning;
			}
			else if (ctx->lex.token.kind == LangC_TokenKind_Identifier ||
					 ctx->lex.token.kind == LangC_TokenKind_Mul)
			{
				reported = true;
				LangC_LexerError(&ctx->lex, "'%S' is not a typename", StrFmt(decl->name));
				
				goto right_before_parsing_rest_of_decl;
			}
		}
		
		if (!reported)
		{
			if (type->h.kind == LangC_AstKind_TypeFunction)
				LangC_LexerWarning(&ctx->lex, LangC_Warning_ImplicitInt, "implicit return type 'int'.");
			else
				LangC_LexerWarning(&ctx->lex, LangC_Warning_ImplicitInt, "implicit type 'int'.");
		}
		
		base->h.kind = LangC_AstKind_TypeInt;
	}
	
	// TODO(ljre): Apply 'callconv'
	
	if (options & 8)
	{
		if (type->h.kind == LangC_AstKind_TypeArray)
		{
			type->h.kind = LangC_AstKind_TypePointer;
			type->h.flags |= LangC_AstFlags_Decayed;
		}
		else if (type->h.kind == LangC_AstKind_TypeFunction)
		{
			LangC_AstType* newtype = LangC_CreateNode(ctx, LangC_AstKind_TypePointer, SizeofPoly(LangC_AstType, ptr));
			newtype->as->ptr.to = type;
			type = newtype;
			
			type->h.flags |= LangC_AstFlags_Decayed;
		}
	}
	
	if (options & 1)
		return type;
	
	// This is a declaration!
	LangC_AstNode* last = (void*)decl;
	decl->type = type;
	
	if (decl->h.kind == LangC_AstKind_DeclTypedef)
		LangC_TypedefDecl(ctx, decl);
	
	if (ctx->lex.token.kind == LangC_TokenKind_LeftCurl)
	{
		if (type->h.kind != LangC_AstKind_TypeFunction)
			LangC_LexerError(&ctx->lex, "invalid block for non-function declaration.");
		
		if (!(options & 2))
			LangC_LexerError(&ctx->lex, "function definitions are only allowed in global scope.");
		
		decl->body = LangC_ParseBlock(ctx, NULL);
	}
	else if (options & 4)
	{
		if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Assign))
		{
			if (decl->h.kind == LangC_AstKind_DeclTypedef)
				LangC_LexerError(&ctx->lex, "cannot assign to types.");
			else if (decl->h.kind == LangC_AstKind_DeclExtern)
				LangC_LexerError(&ctx->lex, "cannot initialize a declaration with 'extern' storage modifier.");
			
			decl->init = LangC_ParseExpr(ctx, 1, true);
		}
		
		while (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Comma))
		{
			LangC_AstDecl* new_node = LangC_CreateNode(ctx, decl->h.kind, sizeof(LangC_AstDecl));
			new_node->type = LangC_ParseRestOfDecl(ctx, base, new_node, false, options & 2);
			last = last->next = (void*)new_node;
			
			// TODO(ljre): Apply 'callconv'
			
			if (options & 8)
			{
				if (new_node->type->h.kind == LangC_AstKind_TypeArray)
				{
					new_node->type->h.kind = LangC_AstKind_TypePointer;
					new_node->type->h.flags |= LangC_AstFlags_Decayed;
				}
				else if (new_node->type->h.kind == LangC_AstKind_TypeFunction)
				{
					LangC_AstType* newtype = LangC_CreateNode(ctx, LangC_AstKind_TypePointer, SizeofPoly(LangC_AstType, ptr));
					newtype->as->ptr.to = new_node->type;
					new_node->type = newtype;
					
					new_node->type->h.flags |= LangC_AstFlags_Decayed;
				}
			}
			
			if (decl->h.kind == LangC_AstKind_DeclTypedef)
				LangC_TypedefDecl(ctx, new_node);
			
			if (LangC_TryToEatToken(&ctx->lex, LangC_TokenKind_Assign))
			{
				if (decl->h.kind == LangC_AstKind_DeclTypedef)
					LangC_LexerError(&ctx->lex, "cannot assign to types.");
				else if (decl->h.kind == LangC_AstKind_DeclExtern)
					LangC_LexerError(&ctx->lex, "cannot initialize a declaration with 'extern' storage modifier.");
				
				new_node->init = LangC_ParseExpr(ctx, 1, true);
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
	LangC_AstNode* result = LangC_ParseDecl(ctx, out_last, options, &b);
	
	if (b)
		LangC_EatToken(&ctx->lex, LangC_TokenKind_Semicolon);
	
	return result;
}

internal bool32
LangC_ParseFile(LangC_Context* ctx)
{
	Trace();
	
	LangC_AstDecl* first_node = NULL;
	LangC_AstDecl* last_node;
	
	LangC_SetupLexer(&ctx->lex, ctx->pre_source, ctx->persistent_arena);
	ctx->lex.ctx = ctx;
	LangC_PushSymbolScope(ctx);
	
	ctx->scope->names = LittleMap_Create(ctx->persistent_arena, 1 << 14);
	ctx->scope->types = LittleMap_Create(ctx->persistent_arena, 1 << 14);
	ctx->scope->structs = LittleMap_Create(ctx->persistent_arena, 1 << 14);
	ctx->scope->unions = LittleMap_Create(ctx->persistent_arena, 1 << 14);
	ctx->scope->enums = LittleMap_Create(ctx->persistent_arena, 1 << 14);
	
	LangC_NextToken(&ctx->lex);
	
	while (ctx->lex.token.kind == LangC_TokenKind_Semicolon)
		LangC_NextToken(&ctx->lex);
	first_node = LangC_ParseDeclAndSemicolonIfNeeded(ctx, (void*)&last_node, 2 | 4);
	
	while (ctx->lex.token.kind != LangC_TokenKind_Eof)
	{
		while (ctx->lex.token.kind == LangC_TokenKind_Semicolon)
			LangC_NextToken(&ctx->lex);
		
		LangC_Node* new_last = NULL;
		last_node->h.next = LangC_ParseDeclAndSemicolonIfNeeded(ctx, &new_last, 2 | 4);
		last_node = new_last;
	}
	
	ctx->ast = first_node;
	return true;
}
