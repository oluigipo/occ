internal LangC_SymbolScope* LangC_ResolveBlock(LangC_Context* ctx, LangC_AstStmt* block);
internal LangC_Node* LangC_ResolveExpr(LangC_Context* ctx, LangC_AstExpr* expr);
internal LangC_Node* LangC_AddCastToExprIfNeeded(LangC_Context* ctx, LangC_AstExpr* expr, LangC_AstType* type);
internal bool32 LangC_ResolveInitializerOfType(LangC_Context* ctx, LangC_AstExpr* init, LangC_AstType* type);

internal LangC_Node*
LangC_CreateNodeFrom(LangC_Context* ctx, LangC_Node* other_, LangC_AstKind kind, uintsize size)
{
	LangC_AstNode* other = other_;
	LangC_AstNode* result = Arena_Push(ctx->persistent_arena, size);
	
	result->kind = kind;
	result->flags = LangC_AstFlags_Implicit;
	result->line = other->line;
	result->col = other->col;
	result->lexfile = other->lexfile;
	
	return result;
}

internal LangC_AstType*
LangC_CreateArrayType(LangC_Context* ctx, LangC_AstType* of, uint64 len)
{
	LangC_AstType* result = LangC_CreateNodeFrom(ctx, of, LangC_AstKind_TypeArray, SizeofPoly(LangC_AstType, array));
	result->as->array.of = of;
	result->as->array.length = len;
	result->size = len * of->size;
	result->alignment_mask = of->alignment_mask;
	
	return result;
}

internal LangC_AstType*
LangC_CreatePointerType(LangC_Context* ctx, LangC_AstType* of)
{
	LangC_AstType* result = LangC_CreateNodeFrom(ctx, of, LangC_AstKind_TypePointer, SizeofPoly(LangC_AstType, ptr));
	result->as->ptr.to = of;
	result->size = ctx->abi->t_ptr.size;
	result->alignment_mask = ctx->abi->t_ptr.alignment_mask;
	
	return result;
}

internal LangC_Symbol*
LangC_FindSimilarSymbolByName(LangC_Context* ctx, String name, LangC_SymbolKind specific)
{
	// TODO(ljre): Try to find similar names to 'name' for suggestion in warnings.
	//
	//             This seems to be the way: https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance
	
	Unreachable();
	
	return NULL;
}

internal LangC_Symbol*
LangC_SymbolAlreadyDefinedInThisScope(LangC_Context* ctx, String name, LangC_SymbolKind specific, LangC_SymbolScope* scope)
{
	LittleMap* map = NULL;
	
	switch (specific)
	{
		case LangC_SymbolKind_Var:
		case LangC_SymbolKind_VarDecl:
		case LangC_SymbolKind_StaticVar:
		case LangC_SymbolKind_Function:
		case LangC_SymbolKind_FunctionDecl:
		case LangC_SymbolKind_Parameter:
		case LangC_SymbolKind_Field:
		case LangC_SymbolKind_EnumConstant: map = scope->names; break;
		case LangC_SymbolKind_Typename: map = scope->types; break;
		case LangC_SymbolKind_Struct: map = scope->structs; break;
		case LangC_SymbolKind_Union: map = scope->unions; break;
		case LangC_SymbolKind_Enum: map = scope->enums; break;
		
		default: Unreachable(); break;
	}
	
	if (map)
		return LittleMap_Fetch(map, name);
	else
		return NULL;
}

internal LangC_AstType*
LangC_TypeFromTypename(LangC_Context* ctx, LangC_AstType* type)
{
	while (type->h.kind == LangC_AstKind_TypeTypename ||
		   (type->h.kind == LangC_AstKind_TypeStruct || type->h.kind == LangC_AstKind_TypeUnion) &&
		   !type->as->structure.body && type->h.symbol)
	{
		type = type->h.symbol->decl->type;
	}
	
	return type;
}

internal LangC_Symbol*
LangC_CreateSymbol(LangC_Context* ctx, String name, LangC_SymbolKind kind, LangC_Node* decl, LangC_SymbolScope* scope)
{
	LittleMap* map = NULL;
	uintsize size = 0;
	
	switch (kind)
	{
		if (0)
		{
			case LangC_SymbolKind_Var:
			case LangC_SymbolKind_VarDecl:
			case LangC_SymbolKind_Parameter:
			case LangC_SymbolKind_StaticVar: size = sizeof(LangC_Symbol);
		}
		if (0)
		{
			case LangC_SymbolKind_Function:
			case LangC_SymbolKind_FunctionDecl: size = SizeofPoly(LangC_Symbol, function);
		}
		if (0)
			case LangC_SymbolKind_Field: size = SizeofPoly(LangC_Symbol, field);
		if (0)
			case LangC_SymbolKind_EnumConstant: size = SizeofPoly(LangC_Symbol, enum_const);
		
		map = ctx->scope->names; break;
		
		case LangC_SymbolKind_Typename: map = scope->types; size = sizeof(LangC_Symbol); break;
		case LangC_SymbolKind_Struct: map = scope->structs; size = SizeofPoly(LangC_Symbol, structure); break;
		case LangC_SymbolKind_Union: map = scope->unions; size = SizeofPoly(LangC_Symbol, structure); break;
		case LangC_SymbolKind_Enum: map = scope->enums; size = SizeofPoly(LangC_Symbol, enumerator); break;
		
		default: Unreachable(); break;
	}
	
	LangC_Symbol* result = Arena_Push(ctx->persistent_arena, size);
	
	result->kind = kind;
	result->name = name;
	result->decl = decl;
	
	LittleMap_Insert(map, name, result);
	
	return result;
}

internal void
LangC_WriteTypeToPersistentArena(LangC_Context* ctx, LangC_AstType* type)
{
	int32 count = 0;
	for (LangC_AstType* it = type; it; it = it->as->not_base.base, ++count);
	if (count == 0)
		return;
	
	LangC_AstType** stack = Arena_Push(ctx->stage_arena, count * sizeof(*stack));
	LangC_AstType* it = type;
	for (int32 i = 0; it; it = it->as->not_base.base, ++i)
		stack[i] = it;
	
	// NOTE(ljre): Print base type
	{
		LangC_AstType* base = stack[count - 1];
		
		if (base->h.flags & LangC_AstFlags_Const)
			Arena_PushMemory(ctx->persistent_arena, 6, "const ");
		if (base->h.flags & LangC_AstFlags_Volatile)
			Arena_PushMemory(ctx->persistent_arena, 9, "volatile ");
		
		switch (base->h.kind)
		{
			case LangC_AstKind_TypeChar:
			{
				if (base->h.flags & LangC_AstFlags_Signed)
					Arena_PushMemory(ctx->persistent_arena, 7, "signed ");
				else if (base->h.flags & LangC_AstFlags_Unsigned)
					Arena_PushMemory(ctx->persistent_arena, 9, "unsigned ");
				
				Arena_PushMemory(ctx->persistent_arena, 4, "char");
			} break;
			
			case LangC_AstKind_TypeInt:
			{
				if (base->h.flags & LangC_AstFlags_Unsigned)
					Arena_PushMemory(ctx->persistent_arena, 9, "unsigned ");
				
				if (base->h.flags & LangC_AstFlags_LongLong)
					Arena_PushMemory(ctx->persistent_arena, 10, "long long ");
				else if (base->h.flags & LangC_AstFlags_Long)
					Arena_PushMemory(ctx->persistent_arena, 5, "long ");
				else if (base->h.flags & LangC_AstFlags_Short)
					Arena_PushMemory(ctx->persistent_arena, 6, "short ");
				
				Arena_PushMemory(ctx->persistent_arena, 3, "int");
			} break;
			
			case LangC_AstKind_TypeFloat:
			{
				Arena_PushMemory(ctx->persistent_arena, 5, "float");
			} break;
			
			case LangC_AstKind_TypeDouble:
			{
				Arena_PushMemory(ctx->persistent_arena, 6, "double");
			} break;
			
			case LangC_AstKind_TypeVoid:
			{
				Arena_PushMemory(ctx->persistent_arena, 4, "void");
			} break;
			
			case LangC_AstKind_TypeBool:
			{
				Arena_PushMemory(ctx->persistent_arena, 6, "_Bool");
			} break;
			
			case LangC_AstKind_TypeTypename:
			{
				Arena_PushMemory(ctx->persistent_arena, StrFmt(base->as->typedefed.name));
			} break;
			
			if (0) case LangC_AstKind_TypeStruct: Arena_PushMemory(ctx->persistent_arena, 7, "struct ");
			if (0) case LangC_AstKind_TypeUnion: Arena_PushMemory(ctx->persistent_arena, 6, "union ");
			if (0) case LangC_AstKind_TypeEnum: Arena_PushMemory(ctx->persistent_arena, 5, "enum ");
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
		LangC_AstType* curr = stack[i];
		LangC_AstType* prev = stack[i+1];
		
		if (curr->h.kind != LangC_AstKind_TypePointer)
			continue;
		
		if (prev->h.kind == LangC_AstKind_TypeArray || prev->h.kind == LangC_AstKind_TypeFunction)
			Arena_PushMemory(ctx->persistent_arena, 2, "(*");
		else
			Arena_PushMemory(ctx->persistent_arena, 1, "*");
		
		if (curr->h.flags & LangC_AstFlags_Const)
			Arena_PushMemory(ctx->persistent_arena, 6, " const");
		if (curr->h.flags & LangC_AstFlags_Volatile)
			Arena_PushMemory(ctx->persistent_arena, 9, " volatile");
	}
	
	// NOTE(ljre): Print postfixes
	it = type;
	for (int32 i = count - 2; i >= 0; it = it->as->not_base.base, --i)
	{
		LangC_AstType* curr = stack[i];
		LangC_AstType* next = i > 0 ? stack[i-1] : NULL;
		
		switch (curr->h.kind)
		{
			case LangC_AstKind_TypeFunction:
			{
				if (next && next->h.kind == LangC_AstKind_TypePointer)
					Arena_PushMemory(ctx->persistent_arena, 1, ")");
				
				Arena_PushMemory(ctx->persistent_arena, 1, "(");
				for (LangC_AstDecl* param = curr->as->function.params;
					 param;
					 (param = (void*)param->h.next) && Arena_PushMemory(ctx->persistent_arena, 2, ", "))
				{
					LangC_WriteTypeToPersistentArena(ctx, param->type);
				}
				Arena_PushMemory(ctx->persistent_arena, 1, ")");
			} break;
			
			case LangC_AstKind_TypeArray:
			case LangC_AstKind_TypeVlaArray:
			{
				if (next && next->h.kind == LangC_AstKind_TypePointer)
					Arena_PushMemory(ctx->persistent_arena, 1, ")");
				
				Arena_PushMemory(ctx->persistent_arena, 1, "[");
				
				if (curr->h.kind == LangC_AstKind_TypeVlaArray)
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
LangC_CStringFromType(LangC_Context* ctx, LangC_AstType* type)
{
	char* result = Arena_End(ctx->persistent_arena);
	
	LangC_WriteTypeToPersistentArena(ctx, type);
	Arena_PushMemory(ctx->persistent_arena, 1, "");
	
	return result;
}

internal const char*
LangC_CStringFromAstKind(LangC_AstKind kind)
{
	static const char* const table[LangC_AstKind__CategoryCount][33] = {
		// TODO(ljre): Rest of the table.
		//             Search doesn't need to be that fast since this function should only
		//             be called when we are reporting warnings or errors.
		
		[(LangC_AstKind_Expr2>>LangC_AstKind__Category) - 1] = {
			"+", "-", "*", "/", "%", "<", ">", "<=", ">=", "==", "!=",
			"<<", ">>", "&", "|", "^", "&&", "||", "=", "+=", "-=", "*=",
			"/=", "<<=", ">>=", "&=", "|=", "^=", ",", "function call()",
			"indexing[]", ".", "->",
		},
	};
	
	uintsize cat = (kind>>LangC_AstKind__Category) - 1;
	uintsize index = (kind & ~LangC_AstKind__CategoryMask) - 1;
	
	Assert(cat < ArrayLength(table));
	Assert(index < ArrayLength(table[cat]));
	
	return table[cat][index];
}

internal uintsize
LangC_NodeCount(LangC_Node* node)
{
	LangC_AstNode* head = node;
	uintsize count = 0;
	
	while (head)
	{
		++count;
		head = head->next;
	}
	
	return count;
}

