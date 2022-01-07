internal C_SymbolScope* C_ResolveBlock(C_Context* ctx, C_AstStmt* block);
internal C_Node* C_ResolveExpr(C_Context* ctx, C_AstExpr* expr);
internal C_Node* C_AddCastToExprIfNeeded(C_Context* ctx, C_AstExpr* expr, C_AstType* type);
internal bool32 C_ResolveInitializerOfType(C_Context* ctx, C_AstExpr* init, C_AstType* type);

internal C_Node*
C_CreateNodeFrom(C_Context* ctx, C_Node* other_, C_AstKind kind, uintsize size)
{
	C_AstNode* other = other_;
	C_AstNode* result = Arena_Push(ctx->persistent_arena, size);
	
	result->kind = kind;
	result->flags = C_AstFlags_Implicit;
	result->line = other->line;
	result->col = other->col;
	result->lexfile = other->lexfile;
	
	return result;
}

internal C_AstType*
C_CreateArrayType(C_Context* ctx, C_AstType* of, uint64 len)
{
	C_AstType* result = C_CreateNodeFrom(ctx, of, C_AstKind_TypeArray, SizeofPoly(C_AstType, array));
	result->as->array.of = of;
	result->as->array.length = len;
	result->size = len * of->size;
	result->alignment_mask = of->alignment_mask;
	
	return result;
}

internal C_AstType*
C_CreatePointerType(C_Context* ctx, C_AstType* of)
{
	C_AstType* result = C_CreateNodeFrom(ctx, of, C_AstKind_TypePointer, SizeofPoly(C_AstType, ptr));
	result->as->ptr.to = of;
	result->size = ctx->abi->t_ptr.size;
	result->alignment_mask = ctx->abi->t_ptr.alignment_mask;
	
	return result;
}

internal C_Symbol*
C_FindSimilarSymbolByName(C_Context* ctx, String name, C_SymbolKind specific)
{
	// TODO(ljre): Try to find similar names to 'name' for suggestion in warnings.
	//
	//             This seems to be the way: https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance
	
	Unreachable();
	
	return NULL;
}

internal C_Symbol*
C_SymbolAlreadyDefinedInThisScope(C_Context* ctx, String name, C_SymbolKind specific, C_SymbolScope* scope)
{
	LittleMap* map = NULL;
	
	switch (specific)
	{
		case C_SymbolKind_Var:
		case C_SymbolKind_VarDecl:
		case C_SymbolKind_StaticVar:
		case C_SymbolKind_Function:
		case C_SymbolKind_FunctionDecl:
		case C_SymbolKind_Parameter:
		case C_SymbolKind_Field:
		case C_SymbolKind_EnumConstant: map = scope->names; break;
		case C_SymbolKind_Typename: map = scope->types; break;
		case C_SymbolKind_Struct: map = scope->structs; break;
		case C_SymbolKind_Union: map = scope->unions; break;
		case C_SymbolKind_Enum: map = scope->enums; break;
		
		default: Unreachable(); break;
	}
	
	if (map)
		return LittleMap_Fetch(map, name);
	else
		return NULL;
}

internal C_AstType*
C_TypeFromTypename(C_Context* ctx, C_AstType* type)
{
	while (type->h.kind == C_AstKind_TypeTypename ||
		   (type->h.kind == C_AstKind_TypeStruct || type->h.kind == C_AstKind_TypeUnion) &&
		   !type->as->structure.body && type->h.symbol)
	{
		type = type->h.symbol->decl->type;
	}
	
	return type;
}

internal C_Symbol*
C_CreateSymbol(C_Context* ctx, String name, C_SymbolKind kind, C_Node* decl, C_SymbolScope* scope)
{
	LittleMap* map = NULL;
	uintsize size = 0;
	
	switch (kind)
	{
		if (0)
		{
			case C_SymbolKind_Var:
			case C_SymbolKind_VarDecl:
			case C_SymbolKind_Parameter:
			case C_SymbolKind_StaticVar: size = sizeof(C_Symbol);
		}
		if (0)
		{
			case C_SymbolKind_Function:
			case C_SymbolKind_FunctionDecl: size = SizeofPoly(C_Symbol, function);
		}
		if (0)
			case C_SymbolKind_Field: size = SizeofPoly(C_Symbol, field);
		if (0)
			case C_SymbolKind_EnumConstant: size = SizeofPoly(C_Symbol, enum_const);
		
		map = ctx->scope->names; break;
		
		case C_SymbolKind_Typename: map = scope->types; size = sizeof(C_Symbol); break;
		case C_SymbolKind_Struct: map = scope->structs; size = SizeofPoly(C_Symbol, structure); break;
		case C_SymbolKind_Union: map = scope->unions; size = SizeofPoly(C_Symbol, structure); break;
		case C_SymbolKind_Enum: map = scope->enums; size = SizeofPoly(C_Symbol, enumerator); break;
		
		default: Unreachable(); break;
	}
	
	C_Symbol* result = Arena_Push(ctx->persistent_arena, size);
	
	result->kind = kind;
	result->name = name;
	result->decl = decl;
	
	LittleMap_Insert(map, name, result);
	
	return result;
}

internal void
C_WriteTypeToPersistentArena(C_Context* ctx, C_AstType* type)
{
	int32 count = 0;
	for (C_AstType* it = type; it; it = it->as->not_base.base, ++count);
	if (count == 0)
		return;
	
	C_AstType** stack = Arena_Push(ctx->stage_arena, count * sizeof(*stack));
	C_AstType* it = type;
	for (int32 i = 0; it; it = it->as->not_base.base, ++i)
		stack[i] = it;
	
	// NOTE(ljre): Print base type
	{
		C_AstType* base = stack[count - 1];
		
		if (base->h.flags & C_AstFlags_Const)
			Arena_PushMemory(ctx->persistent_arena, 6, "const ");
		if (base->h.flags & C_AstFlags_Volatile)
			Arena_PushMemory(ctx->persistent_arena, 9, "volatile ");
		
		switch (base->h.kind)
		{
			case C_AstKind_TypeChar:
			{
				if (base->h.flags & C_AstFlags_Signed)
					Arena_PushMemory(ctx->persistent_arena, 7, "signed ");
				else if (base->h.flags & C_AstFlags_Unsigned)
					Arena_PushMemory(ctx->persistent_arena, 9, "unsigned ");
				
				Arena_PushMemory(ctx->persistent_arena, 4, "char");
			} break;
			
			case C_AstKind_TypeInt:
			{
				if (base->h.flags & C_AstFlags_Unsigned)
					Arena_PushMemory(ctx->persistent_arena, 9, "unsigned ");
				
				if (base->h.flags & C_AstFlags_LongLong)
					Arena_PushMemory(ctx->persistent_arena, 10, "long long ");
				else if (base->h.flags & C_AstFlags_Long)
					Arena_PushMemory(ctx->persistent_arena, 5, "long ");
				else if (base->h.flags & C_AstFlags_Short)
					Arena_PushMemory(ctx->persistent_arena, 6, "short ");
				
				Arena_PushMemory(ctx->persistent_arena, 3, "int");
			} break;
			
			case C_AstKind_TypeFloat:
			{
				Arena_PushMemory(ctx->persistent_arena, 5, "float");
			} break;
			
			case C_AstKind_TypeDouble:
			{
				Arena_PushMemory(ctx->persistent_arena, 6, "double");
			} break;
			
			case C_AstKind_TypeVoid:
			{
				Arena_PushMemory(ctx->persistent_arena, 4, "void");
			} break;
			
			case C_AstKind_TypeBool:
			{
				Arena_PushMemory(ctx->persistent_arena, 6, "_Bool");
			} break;
			
			case C_AstKind_TypeTypename:
			{
				Arena_PushMemory(ctx->persistent_arena, StrFmt(base->as->typedefed.name));
			} break;
			
			if (0) case C_AstKind_TypeStruct: Arena_PushMemory(ctx->persistent_arena, 7, "struct ");
			if (0) case C_AstKind_TypeUnion: Arena_PushMemory(ctx->persistent_arena, 6, "union ");
			if (0) case C_AstKind_TypeEnum: Arena_PushMemory(ctx->persistent_arena, 5, "enum ");
			{
				if (base->as->structure.name.size > 0)
					Arena_PushMemory(ctx->persistent_arena, StrFmt(base->as->structure.name));
				else
					Arena_PushMemory(ctx->persistent_arena, 11, "(anonymous)");
			} break;
		}
	}
	
	// NOTE(ljre): Print prefixes
	it = type;
	for (int32 i = count - 2; i >= 0; it = it->as->not_base.base, --i)
	{
		C_AstType* curr = stack[i];
		C_AstType* prev = stack[i+1];
		
		if (curr->h.kind != C_AstKind_TypePointer)
			continue;
		
		if (prev->h.kind == C_AstKind_TypeArray || prev->h.kind == C_AstKind_TypeFunction)
			Arena_PushMemory(ctx->persistent_arena, 2, "(*");
		else
			Arena_PushMemory(ctx->persistent_arena, 1, "*");
		
		if (curr->h.flags & C_AstFlags_Const)
			Arena_PushMemory(ctx->persistent_arena, 6, " const");
		if (curr->h.flags & C_AstFlags_Volatile)
			Arena_PushMemory(ctx->persistent_arena, 9, " volatile");
	}
	
	// NOTE(ljre): Print postfixes
	it = type;
	for (int32 i = count - 2; i >= 0; it = it->as->not_base.base, --i)
	{
		C_AstType* curr = stack[i];
		C_AstType* next = i > 0 ? stack[i-1] : NULL;
		
		switch (curr->h.kind)
		{
			case C_AstKind_TypeFunction:
			{
				if (next && next->h.kind == C_AstKind_TypePointer)
					Arena_PushMemory(ctx->persistent_arena, 1, ")");
				
				Arena_PushMemory(ctx->persistent_arena, 1, "(");
				for (C_AstDecl* param = curr->as->function.params;
					 param;
					 (param = (void*)param->h.next) && Arena_PushMemory(ctx->persistent_arena, 2, ", "))
				{
					C_WriteTypeToPersistentArena(ctx, param->type);
				}
				Arena_PushMemory(ctx->persistent_arena, 1, ")");
			} break;
			
			case C_AstKind_TypeArray:
			case C_AstKind_TypeVlaArray:
			{
				if (next && next->h.kind == C_AstKind_TypePointer)
					Arena_PushMemory(ctx->persistent_arena, 1, ")");
				
				Arena_PushMemory(ctx->persistent_arena, 1, "[");
				
				if (curr->h.kind == C_AstKind_TypeVlaArray)
					Arena_PushMemory(ctx->persistent_arena, 3, "VLA");
				else
					Arena_Printf(ctx->persistent_arena, "%llu", curr->as->array.length);
				
				Arena_PushMemory(ctx->persistent_arena, 1, "]");
			} break;
		}
	}
	
	Arena_Pop(ctx->stage_arena, stack);
}

internal const char*
C_CStringFromType(C_Context* ctx, C_AstType* type)
{
	char* result = Arena_End(ctx->persistent_arena);
	
	C_WriteTypeToPersistentArena(ctx, type);
	Arena_PushMemory(ctx->persistent_arena, 1, "");
	
	return result;
}

internal const char*
C_CStringFromAstKind(C_AstKind kind)
{
	static const char* const table[C_AstKind__CategoryCount][33] = {
		// TODO(ljre): Rest of the table.
		//             Search doesn't need to be that fast since this function should only
		//             be called when we are reporting warnings or errors.
		
		[(C_AstKind_Expr2>>C_AstKind__Category) - 1] = {
			"+", "-", "*", "/", "%", "<", ">", "<=", ">=", "==", "!=",
			"<<", ">>", "&", "|", "^", "&&", "||", "=", "+=", "-=", "*=",
			"/=", "<<=", ">>=", "&=", "|=", "^=", ",", "function call()",
			"indexing[]", ".", "->",
		},
	};
	
	uintsize cat = (kind>>C_AstKind__Category) - 1;
	uintsize index = (kind & ~C_AstKind__CategoryMask) - 1;
	
	Assert(cat < ArrayLength(table));
	Assert(index < ArrayLength(table[cat]));
	
	return table[cat][index];
}

internal uintsize
C_NodeCount(C_Node* node)
{
	C_AstNode* head = node;
	uintsize count = 0;
	
	while (head)
	{
		++count;
		head = head->next;
	}
	
	return count;
}

internal bool32
C_ResolveAst(C_Context* ctx)
{
	// TODO
	
	return true;
}
