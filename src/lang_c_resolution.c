// NOTE(ljre): This file contains code for symbol and type resolution.

internal LangC_SymbolStack*
LangC_PushSymbolStack(LangC_Context* ctx)
{
	LangC_SymbolStack* stack = Arena_Push(ctx->persistent_arena, sizeof *stack);
	
	if (ctx->previous_symbol_stack)
	{
		ctx->previous_symbol_stack->next = stack;
		ctx->previous_symbol_stack = NULL;
		ctx->symbol_stack->down = stack;
	}
	
	stack->up = ctx->symbol_stack;
	stack->symbols = NULL;
	
	ctx->symbol_stack = stack;
	return stack;
}

internal void
LangC_PopSymbolStack(LangC_Context* ctx)
{
	ctx->previous_symbol_stack = ctx->symbol_stack;
	ctx->symbol_stack = ctx->symbol_stack->up;
}

internal LangC_Symbol*
LangC_FindSymbol(LangC_Context* ctx, String name)
{
	uint64 hash = SimpleHash(name);
	LangC_SymbolStack* stack = ctx->symbol_stack;
	
	while (stack)
	{
		LangC_Symbol* it = stack->symbols;
		
		for (; it; it = it->next)
		{
			if (it->name_hash == hash)
				return it;
		}
		
		stack = stack->up;
	}
	
	return NULL;
}

internal LangC_Symbol*
LangC_SymbolAlreadyDefinedInThisScope(LangC_Context* ctx, String name)
{
	uint64 hash = SimpleHash(name);
	LangC_Symbol* it = ctx->symbol_stack->symbols;
	
	for (; it; it = it->next)
	{
		if (it->name_hash == hash)
			return it;
	}
	
	return NULL;
}

internal LangC_Symbol*
LangC_CreateSymbol(LangC_Context* ctx, String name, LangC_SymbolKind kind, LangC_Node* type)
{
	LangC_Symbol* result = Arena_Push(ctx->persistent_arena, sizeof *result);
	LangC_SymbolStack* stack = ctx->symbol_stack;
	
	result->next = stack->symbols;
	stack->symbols = result;
	
	result->kind = kind;
	result->name = name;
	result->type = type;
	result->name_hash = SimpleHash(name);
	
	return result;
}

#define LangC_INT ((LangC_Node*)1)
#define LangC_LINT ((LangC_Node*)2)
#define LangC_LLINT ((LangC_Node*)3)
#define LangC_UINT ((LangC_Node*)4)
#define LangC_LUINT ((LangC_Node*)5)
#define LangC_LLUINT ((LangC_Node*)6)
#define LangC_CHAR ((LangC_Node*)7)
#define LangC_SCHAR ((LangC_Node*)8)
#define LangC_UCHAR ((LangC_Node*)9)
#define LangC_FLOAT ((LangC_Node*)10)
#define LangC_DOUBLE ((LangC_Node*)11)
#define LangC_SHORT ((LangC_Node*)12)
#define LangC_USHORT ((LangC_Node*)13)
#define LangC_VOID ((LangC_Node*)14)

// NOTE(ljre): Compares two types.
//             Returns 0 if both are the same type, 1 if 'left' is compatible with 'right', and -1 if
//             they are incompatible.
//
//             Important: if 'right' isn't compatible with 'left' just because 'left' is compatible with 'right'.
internal int32
LangC_MatchTypes(LangC_Context* ctx, LangC_Node* left, LangC_Node* right)
{
	// TODO
	
	return 0;
}

internal bool32
LangC_IsExprLValue(LangC_Context* ctx, LangC_Node* expr)
{
	return false;
}

internal void
LangC_ResolveExpr(LangC_Context* ctx, LangC_Node* expr)
{
	// TODO
}

internal void
LangC_ResolveDecl(LangC_Context* ctx, LangC_Node* decl)
{
	Assert(decl->kind == LangC_NodeKind_Decl);
	
	// TODO
}

internal void
LangC_ResolveStmt(LangC_Context* ctx, LangC_Node* stmt)
{
	switch (stmt->kind)
	{
		case LangC_NodeKind_EmptyStmt: break;
		
		case LangC_NodeKind_ReturnStmt:
		case LangC_NodeKind_ExprStmt:
		{
			LangC_ResolveExpr(ctx, stmt->expr);
		} break;
		
		case LangC_NodeKind_ForStmt:
		{
			if (stmt->init)
			{
				if (stmt->init->kind == LangC_NodeKind_Decl)
					LangC_ResolveDecl(ctx, stmt->init);
				else
					LangC_ResolveExpr(ctx, stmt->init);
			}
			
			if (stmt->iter)
				LangC_ResolveExpr(ctx, stmt->iter);
		} /* fallthrough */
		case LangC_NodeKind_DoWhileStmt:
		case LangC_NodeKind_WhileStmt:
		case LangC_NodeKind_IfStmt:
		{
			LangC_ResolveExpr(ctx, stmt->condition);
			if (LangC_MatchTypes(ctx, stmt->condition->type, LangC_INT) < 0)
			{
				LangC_NodeError(stmt->condition, "condition should be of numeric type.");
			}
			
			LangC_ResolveStmt(ctx, stmt->branch1);
			if (stmt->branch2)
				LangC_ResolveStmt(ctx, stmt->branch2);
		} break;
		
		case LangC_NodeKind_SwitchStmt:
		{
			LangC_ResolveExpr(ctx, stmt->expr);
			if (LangC_MatchTypes(ctx, stmt->expr->type, LangC_INT) < 0)
			{
				LangC_NodeError(stmt->condition, "condition should be of numeric type.");
			}
			
			LangC_ResolveStmt(ctx, stmt->stmt);
		} break;
		
		case LangC_NodeKind_Label:
		{
			LangC_ResolveStmt(ctx, stmt->stmt);
			
			// TODO(ljre): Add to the "labels table"
		} break;
		
		case LangC_NodeKind_CaseLabel:
		{
			LangC_ResolveExpr(ctx, stmt->expr);
			if (LangC_MatchTypes(ctx, stmt->expr->type, LangC_INT) < 0)
			{
				LangC_NodeError(stmt->condition, "condition should be of numeric type.");
			}
			
			LangC_ResolveStmt(ctx, stmt->stmt);
			
			// TODO(ljre): Find the host switch statement
		} break;
		
		// TODO(ljre): Other statements
		
		default: Unreachable(); break;
	}
}

internal LangC_SymbolStack*
LangC_ResolveBlock(LangC_Context* ctx, LangC_Node* block)
{
	Assert(block->kind == LangC_NodeKind_CompoundStmt);
	
	LangC_SymbolStack* result = LangC_PushSymbolStack(ctx);
	
	for (LangC_Node* stmt = block->stmt; stmt; stmt = stmt->next)
	{
		if (stmt->kind == LangC_NodeKind_Decl)
			LangC_ResolveDecl(ctx, stmt);
		else
			LangC_ResolveStmt(ctx, stmt);
	}
	
	LangC_PopSymbolStack(ctx);
	return result;
}

internal void
LangC_ResolveGlobalDecl(LangC_Context* ctx, LangC_Node* decl)
{
	Assert(decl->kind == LangC_NodeKind_Decl);
	
	if (decl->flags & LangC_Node_Typedef)
	{
		if (LangC_SymbolAlreadyDefinedInThisScope(ctx, decl->name))
			LangC_NodeError(decl, "redefinition of type '%.*s'.", StrFmt(decl->name));
		
		LangC_CreateSymbol(ctx, decl->name, LangC_SymbolKind_Typename, decl->type);
		return;
	}
	
	// TODO(ljre): Rest of the stuff
	if (decl->type->kind == LangC_NodeKind_FunctionType)
	{
		LangC_SymbolKind kind = LangC_SymbolKind_GlobalFunctionDecl;
		if (decl->body)
			kind = LangC_SymbolKind_GlobalFunction;
		
		LangC_Symbol* sym = LangC_CreateSymbol(ctx, decl->name, kind, decl->type);
		
		if (decl->body)
			sym->locals = LangC_ResolveBlock(ctx, decl->body);
	}
}

internal bool32
LangC_ResolveAst(LangC_Context* ctx)
{
	LangC_PushSymbolStack(ctx);
	
	LangC_Node* global_decl = ctx->ast;
	while (global_decl)
	{
		LangC_ResolveGlobalDecl(ctx, global_decl);
		global_decl = global_decl->next;
	}
	
	LangC_PopSymbolStack(ctx);
	
	return LangC_error_count == 0;
}
