internal C_AstExpr* C_ResolveExpr(C_Context* ctx, C_AstExpr* expr);
internal C_AstExpr* C_AddCastToExprIfNeeded(C_Context* ctx, C_AstExpr* expr, C_AstType* type);
internal void C_ResolveLocalDecl(C_Context* ctx, C_AstDecl* decl);
internal bool32 C_ResolveTypeWithInitializer(C_Context* ctx, C_AstType** ptype, C_AstExpr** pinit, uint32 flags);

internal C_Node*
C_CreateNodeFrom(C_Context* ctx, C_Node* other_, C_AstKind kind, uintsize size)
{
	C_AstNode* other = other_;
	C_AstNode* result = Arena_Push(ctx->persistent_arena, size);
	
	result->kind = kind;
	result->flags = C_AstFlags_Implicit;
	result->trace = other->trace;
	
	return result;
}

internal C_AstType*
C_CreateArrayType(C_Context* ctx, C_AstType* of, uint64 len)
{
	C_AstType* result = C_CreateNodeFrom(ctx, of, C_AstKind_TypeArray, sizeof(C_AstType));
	result->array.of = of;
	result->array.length = C_CreateNodeFrom(ctx, of, C_AstKind_ExprULLInt, sizeof(C_AstExpr));
	result->array.length->h.flags |= C_AstFlags_ComptimeKnown;
	result->array.length->value_uint = len;
	result->size = len * of->size;
	result->alignment_mask = of->alignment_mask;
	
	return result;
}

internal C_AstType*
C_CreatePointerType(C_Context* ctx, C_AstType* of)
{
	C_AstType* result = C_CreateNodeFrom(ctx, of, C_AstKind_TypePointer, sizeof(C_AstType));
	result->ptr.to = of;
	result->size = ctx->abi->t_ptr.size;
	result->alignment_mask = ctx->abi->t_ptr.alignment_mask;
	
	return result;
}

internal Map*
C_GetMapFromSymbolKind(C_Context* ctx, C_SymbolScope* scope, C_SymbolKind kind, bool32 create_new)
{
	Map** map = NULL;
	
	switch (kind)
	{
		default:
		case C_SymbolKind_GlobalVarDecl:
		case C_SymbolKind_GlobalVar:
		case C_SymbolKind_StaticVar:
		case C_SymbolKind_LocalVar:
		case C_SymbolKind_Function:
		case C_SymbolKind_FunctionDecl:
		case C_SymbolKind_Parameter:
		case C_SymbolKind_Field:
		case C_SymbolKind_EnumConstant: map = &scope->names; break;
		case C_SymbolKind_Typename: map = &scope->types; break;
		case C_SymbolKind_Struct: map = &scope->structs; break;
		case C_SymbolKind_Union: map = &scope->unions; break;
		case C_SymbolKind_Enum: map = &scope->enums; break;
	}
	
	if (create_new && !*map)
		*map = Map_Create(ctx->persistent_arena, 32);
	
	return *map;
}

internal C_Symbol*
C_CreateSymbolInScope(C_Context* ctx, String name, C_SymbolKind kind, C_AstDecl* decl, C_SymbolScope* scope)
{
	C_Symbol* sym = Arena_Push(ctx->persistent_arena, sizeof(*sym));
	
	sym->kind = kind;
	sym->decl = decl;
	sym->name = name;
	
	decl->h.symbol = sym;
	
	Map* map = C_GetMapFromSymbolKind(ctx, scope, kind, true);
	uint64 hash;
	
	if (name.size > 0)
		hash = SimpleHash(name);
	else
		hash = ++ctx->unnamed_count;
	
	Map_InsertWithHash(map, name, sym, hash);
	
	return sym;
}

internal inline C_Symbol*
C_CreateSymbol(C_Context* ctx, String name, C_SymbolKind kind, C_AstDecl* decl)
{
	return C_CreateSymbolInScope(ctx, name, kind, decl, ctx->working_scope);
}

internal C_Symbol*
C_FindSymbolWithSimilarName(C_Context* ctx, String name, C_SymbolKind specific)
{
	// TODO(ljre): Try to find similar names to 'name' for suggestion in warnings.
	//
	//             This seems to be the way: https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance
	
	Unreachable();
	
	return NULL;
}

internal C_Symbol*
C_SymbolDefinedInScope(C_Context* ctx, String name, C_SymbolKind specific, C_SymbolScope* scope)
{
	Map* map = C_GetMapFromSymbolKind(ctx, scope, specific, false);
	
	if (map)
		return Map_Fetch(map, name);
	else
		return NULL;
}

internal C_Symbol*
C_FindSymbolInScope(C_Context* ctx, String name, C_SymbolKind kind, C_SymbolScope* scope)
{
	uint64 hash = SimpleHash(name);
	
	for (; scope; scope = scope->up)
	{
		Map* map = C_GetMapFromSymbolKind(ctx, scope, kind, false);
		if (!map)
			continue;
		
		C_Symbol* sym = Map_FetchWithHash(map, name, hash);
		if (sym)
			return sym;
	}
	
	return NULL;
}

internal inline C_Symbol*
C_FindSymbol(C_Context* ctx, String name, C_SymbolKind kind)
{
	return C_FindSymbolInScope(ctx, name, kind, ctx->working_scope);
}

internal C_AstType*
C_TypeFromTypename(C_Context* ctx, C_AstType* type)
{
	while (type->h.kind == C_AstKind_TypeTypename ||
		   (type->h.kind == C_AstKind_TypeStruct || type->h.kind == C_AstKind_TypeUnion) &&
		   !type->structure.body && type->h.symbol)
	{
		type = type->h.symbol->decl->type;
	}
	
	return type;
}

internal bool32
C_IsTypeComplete(C_Context* ctx, C_AstType* type)
{
	return type->size > 0;
}

internal bool32
C_IsIntegerType(C_Context* ctx, C_AstType* type)
{
	switch (type->h.kind)
	{
		case C_AstKind_TypeChar:
		case C_AstKind_TypeInt:
		case C_AstKind_TypeBool:
		case C_AstKind_TypeEnum:
		case C_AstKind_TypePointer: return true;
	}
	
	return false;
}

internal C_AstType*
C_DecayType(C_Context* ctx, C_AstType* type)
{
	switch (type->h.kind)
	{
		case C_AstKind_TypeArray:
		case C_AstKind_TypeVlaArray: type = type->array.of;
		case C_AstKind_TypeFunction:
		{
			C_AstType* newtype = C_CreateNodeFrom(ctx, type, C_AstKind_TypePointer, sizeof(C_AstType));
			newtype->ptr.to = type;
			newtype->size = ctx->abi->t_ptr.size;
			newtype->alignment_mask = ctx->abi->t_ptr.alignment_mask;
			newtype->is_unsigned = ctx->abi->t_ptr.unsig;
			
			type = newtype;
		} break;
	}
	
	return type;
}

internal C_AstType*
C_TypeFromAbiType(C_Context* ctx, const C_ABIType* abitype, C_Node* for_trace)
{
	C_AstType* result = C_CreateNodeFrom(ctx, for_trace, C_AstKind_Type, sizeof(C_AstType));
	
	uint32 offset = (uint8*)abitype - (uint8*)ctx->abi->t;
	switch (offset)
	{
		case offsetof(C_ABI, t_bool): result->h.kind = C_AstKind_TypeBool; break;
		case offsetof(C_ABI, t_schar): result->h.kind = C_AstKind_TypeChar; result->h.flags |= C_AstFlags_Signed; break;
		case offsetof(C_ABI, t_char): result->h.kind = C_AstKind_TypeChar; break;
		case offsetof(C_ABI, t_uchar): result->h.kind = C_AstKind_TypeChar; result->h.flags |= C_AstFlags_Unsigned; break;
		case offsetof(C_ABI, t_short): result->h.kind = C_AstKind_TypeInt; result->h.flags |= C_AstFlags_Short; break;
		case offsetof(C_ABI, t_ushort): result->h.kind = C_AstKind_TypeInt; result->h.flags |= C_AstFlags_Unsigned|C_AstFlags_Short; break;
		case offsetof(C_ABI, t_int): result->h.kind = C_AstKind_TypeInt; break;
		case offsetof(C_ABI, t_uint): result->h.kind = C_AstKind_TypeInt; result->h.flags |= C_AstFlags_Unsigned; break;
		case offsetof(C_ABI, t_long): result->h.kind = C_AstKind_TypeInt; result->h.flags |= C_AstFlags_Long; break;
		case offsetof(C_ABI, t_ulong): result->h.kind = C_AstKind_TypeInt; result->h.flags |= C_AstFlags_Unsigned|C_AstFlags_Long; break;
		case offsetof(C_ABI, t_longlong): result->h.kind = C_AstKind_TypeInt; result->h.flags |= C_AstFlags_LongLong; break;
		case offsetof(C_ABI, t_ulonglong): result->h.kind = C_AstKind_TypeInt; result->h.flags |= C_AstFlags_Unsigned|C_AstFlags_LongLong; break;
		case offsetof(C_ABI, t_float): result->h.kind = C_AstKind_TypeFloat; break;
		case offsetof(C_ABI, t_double): result->h.kind = C_AstKind_TypeDouble; break;
		
		case offsetof(C_ABI, t_ptr):
		default: Unreachable(); break;
	}
	
	result->size = abitype->size;
	result->alignment_mask = abitype->alignment_mask;
	result->is_unsigned = abitype->unsig;
	
	return result;
}

internal bool32
C_TryToEval(C_Context* ctx, C_AstExpr* expr)
{
	if (expr->h.flags & C_AstFlags_ComptimeKnown)
		return true;
	
	switch (expr->h.kind)
	{
		// TODO
	}
	
	return false;
}

internal void
C_WriteTypeToPersistentArena(C_Context* ctx, C_AstType* type)
{
	int32 count = 0;
	for (C_AstType* it = type; it; it = it->not_base.base)
	{
		++count;
		if (C_IsBaseType(it->h.kind))
			break;
	}
	
	if (count == 0)
		return;
	
	C_AstType** stack = Arena_Push(ctx->stage_arena, count * sizeof(*stack));
	C_AstType* it = type;
	for (int32 i = 0; i < count; it = it->not_base.base, ++i)
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
				Arena_PushMemory(ctx->persistent_arena, StrFmt(base->typedefed.name));
			} break;
			
			if (0) case C_AstKind_TypeStruct: Arena_PushMemory(ctx->persistent_arena, 7, "struct ");
			if (0) case C_AstKind_TypeUnion: Arena_PushMemory(ctx->persistent_arena, 6, "union ");
			if (0) case C_AstKind_TypeEnum: Arena_PushMemory(ctx->persistent_arena, 5, "enum ");
			{
				if (base->structure.name.size > 0)
					Arena_PushMemory(ctx->persistent_arena, StrFmt(base->structure.name));
				else
					Arena_PushMemory(ctx->persistent_arena, 11, "(anonymous)");
			} break;
		}
	}
	
	// NOTE(ljre): Print prefixes
	it = type;
	for (int32 i = count - 2; i >= 0; it = it->not_base.base, --i)
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
	for (int32 i = count - 2; i >= 0; it = it->not_base.base, --i)
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
				for (C_AstDecl* param = curr->function.params;
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
				
				if (curr->h.kind == C_AstKind_TypeArray)
					Arena_Printf(ctx->persistent_arena, "%U", curr->array.length);
				
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

// NOTE(ljre): Resolve type
//
//             Flags:
//                 1 - is global decl
//                 2 - inlined struct/union -- pass 'C_SymbolScope*' to argument aux
internal void
C_ResolveType(C_Context* ctx, C_AstType* type, bool32* out_is_complete, uint32 flags, void* aux)
{
	bool32 is_complete = false;
	const C_ABIType* abitype = NULL;
	
	switch (type->h.kind)
	{
		case C_AstKind_TypeTypename:
		{
			type->h.symbol = C_FindSymbol(ctx, type->typedefed.name, C_SymbolKind_Typename);
			Assert(type->h.symbol);
			
			C_AstType* typedefed = type->h.symbol->decl->type;
			is_complete = C_IsTypeComplete(ctx, typedefed);
			if (is_complete)
			{
				type->size = typedefed->size;
				type->alignment_mask = typedefed->alignment_mask;
				type->is_unsigned = typedefed->is_unsigned;
			}
		} break;
		
		// NOTE(ljre): Set primitives
		{
			case C_AstKind_TypeChar:
			{
				if (type->h.flags & C_AstFlags_Signed)
					abitype = &ctx->abi->t_schar;
				else if (type->h.flags & C_AstFlags_Unsigned)
					abitype = &ctx->abi->t_uchar;
				else
					abitype = &ctx->abi->t_schar;
			} break;
			
			case C_AstKind_TypeInt:
			{
				if (type->h.flags & C_AstFlags_LongLong)
					abitype = &ctx->abi->t_longlong;
				else if (type->h.flags & C_AstFlags_Long)
					abitype = &ctx->abi->t_long;
				else if (type->h.flags & C_AstFlags_Short)
					abitype = &ctx->abi->t_short;
				else
					abitype = &ctx->abi->t_int;
				
				if (type->h.flags & C_AstFlags_Unsigned)
					abitype++; // NOTE(ljre): Make unsigned.
			} break;
			
			case C_AstKind_TypeFloat: abitype = &ctx->abi->t_float; break;
			case C_AstKind_TypeDouble: abitype = &ctx->abi->t_double; break;
			case C_AstKind_TypeBool: abitype = &ctx->abi->t_bool; break;
		}
		
		case C_AstKind_TypeVoid:
		{
		} break;
		
		// NOTE(ljre): Structs and Unions
		{
			C_SymbolKind kind;
			
			/*  */ case C_AstKind_TypeUnion: kind = C_SymbolKind_Union;
			if (0) case C_AstKind_TypeStruct: kind = C_SymbolKind_Struct;
			{
				C_Symbol* sym = NULL;
				bool32 same_scope = false;
				bool32 named = (type->structure.name.size > 0);
				
				if (named)
				{
					sym = C_SymbolDefinedInScope(ctx, type->structure.name, C_SymbolKind_Struct, ctx->working_scope);
					
					if (!sym)
						sym = C_FindSymbol(ctx, type->structure.name, C_SymbolKind_Struct);
					else
						same_scope = true;
				}
				
				if (type->structure.body)
				{
					if (same_scope)
					{
						// TODO(ljre): Check layout of structs and, if they are not exactly the same, error.
					}
					else
						sym = C_CreateSymbol(ctx, type->structure.name, kind, (void*)type);
					
					C_AstDecl* decl = type->structure.body;
					is_complete = true;
					
					C_SymbolScope* scope;
					
					if (flags & 2)
						scope = aux;
					else
						scope = Arena_Push(ctx->persistent_arena, sizeof(C_SymbolScope));
					
					sym->structure.fields = scope;
					
					uint64 size = 0;
					uint64 alignment_mask = 0;
					
					for (; decl; decl = (void*)decl->h.next)
					{
						bool32 complete;
						
						C_ResolveType(ctx, decl->type, &complete, decl->name.size == 0 ? 2 : 0, NULL);
						if (!complete)
						{
							C_NodeError(ctx, decl->type, "struct member needs to be of complete type.");
							continue;
						}
						
						alignment_mask = Max(alignment_mask, decl->type->alignment_mask);
						size = AlignUp(size, alignment_mask);
						
						// TODO(ljre): Check for repeated names.
						
						C_Symbol* field = C_CreateSymbolInScope(ctx, decl->name, C_SymbolKind_Field, decl, scope);
						
						if (kind == C_SymbolKind_Struct)
						{
							size += decl->type->size;
							field->field.offset = size;
						}
						else
						{
							size = Max(size, decl->type->size);
							field->field.offset = 0;
						}
					}
					
					type->size = size;
				}
				
				type->h.symbol = sym;
			} break;
		}
		
		case C_AstKind_TypeFunction:
		{
			C_ResolveType(ctx, type->function.ret, NULL, 0, NULL);
			
			for (C_AstDecl* param = type->function.params; param; param = (void*)param->h.next)
			{
				C_ResolveType(ctx, param->type, NULL, 0, NULL);
				param->type = C_DecayType(ctx, param->type);
				
				if (param->init)
					C_NodeError(ctx, param->init, "default arguments are not a C feature.");
			}
			
			type->size = 0;
			type->alignment_mask = 0;
		} break;
		
		case C_AstKind_TypePointer:
		{
			C_ResolveType(ctx, type->ptr.to, NULL, 0, NULL);
			
			type->size = ctx->abi->t_ptr.size;
			type->alignment_mask = ctx->abi->t_ptr.alignment_mask;
			type->is_unsigned = true;
			is_complete = true;
		} break;
		
		case C_AstKind_TypeArray:
		{
			bool32 complete;
			C_ResolveType(ctx, type->array.of, &complete, 0, NULL);
			
			if (!complete)
				C_NodeError(ctx, type->array.of, "arrays cannot be of incomplete type.");
			if (type->array.length->value_uint == 0)
				C_NodeError(ctx, type->array.length, "arrays cannot have length of 0.");
			
			type->size = type->array.length->value_uint * type->array.of->size;
			type->alignment_mask = type->array.of->alignment_mask;
			is_complete = true;
		} break;
		
		case C_AstKind_TypeVlaArray:
		{
			bool32 complete;
			C_ResolveType(ctx, type->array.of, &complete, 0, NULL);
			
			if (!complete)
				C_NodeError(ctx, type->array.of, "arrays cannot be of incomplete type.");
			if (type->array.length)
			{
				C_ResolveExpr(ctx, type->array.length);
				
				if (!C_IsIntegerType(ctx, type->array.length->type))
					C_NodeError(ctx, type->array.length, "array length needs to be of integral type.");
				else if (C_TryToEval(ctx, type->array.length))
				{
					if (!type->array.length->type->is_unsigned && type->array.length->value_int < 0)
						C_NodeError(ctx, type->array.length, "arrays cannot have negative length.");
					else if (type->array.length->value_uint == 0)
						C_NodeError(ctx, type->array.length, "arrays cannot have length of 0.");
					else
					{
						type->h.kind = C_AstKind_TypeArray;
						type->size = type->array.length->value_uint * type->array.of->size;
						type->alignment_mask = type->array.of->alignment_mask;
						is_complete = true;
					}
				}
			}
		} break;
		
		case C_AstKind_TypeEnum:
		{
			type->size = ctx->abi->t_int.size;
			type->alignment_mask = ctx->abi->t_int.alignment_mask;
			type->is_unsigned = ctx->abi->t_int.unsig;
			is_complete = true;
		} break;
		
		default: Unreachable(); break;
	}
	
	if (abitype)
	{
		type->size = abitype->size;
		type->alignment_mask = abitype->alignment_mask;
		type->is_unsigned = abitype->unsig;
		
		is_complete = true;
	}
	
	if (out_is_complete)
		*out_is_complete = is_complete;
}

internal C_AstExpr*
C_ResolveExpr(C_Context* ctx, C_AstExpr* expr)
{
	switch (expr->h.kind)
	{
		case C_AstKind_ExprIdent:
		{
			expr->h.symbol = C_FindSymbol(ctx, expr->ident.name, 0);
			if (!expr->h.symbol)
				C_NodeError(ctx, expr, "'%S' is not defined.", expr->ident.name);
			else
			{
				if(expr->h.symbol->flags & C_AstFlags_Poisoned)
					C_PoisonNode(ctx, expr);
				
				expr->type = expr->h.symbol->decl->type;
				
				if (expr->h.symbol->kind == C_SymbolKind_EnumConstant)
				{
					expr->h.flags |= C_AstFlags_ComptimeKnown;
					expr->value_int = expr->h.symbol->enum_const.value;
				}
			}
		} break;
		
		case C_AstKind_ExprInt:
		{
			expr->h.flags |= C_AstFlags_ComptimeKnown;
			expr->type = C_TypeFromAbiType(ctx, &ctx->abi->t_int, expr);
		} break;
		
		case C_AstKind_ExprLInt:
		{
			expr->h.flags |= C_AstFlags_ComptimeKnown;
			expr->type = C_TypeFromAbiType(ctx, &ctx->abi->t_long, expr);
		} break;
		
		case C_AstKind_ExprLLInt:
		{
			expr->h.flags |= C_AstFlags_ComptimeKnown;
			expr->type = C_TypeFromAbiType(ctx, &ctx->abi->t_longlong, expr);
		} break;
		
		case C_AstKind_ExprUInt:
		{
			expr->h.flags |= C_AstFlags_ComptimeKnown;
			expr->type = C_TypeFromAbiType(ctx, &ctx->abi->t_uint, expr);
		} break;
		
		case C_AstKind_ExprULInt:
		{
			expr->h.flags |= C_AstFlags_ComptimeKnown;
			expr->type = C_TypeFromAbiType(ctx, &ctx->abi->t_ulong, expr);
		} break;
		
		case C_AstKind_ExprULLInt:
		{
			expr->h.flags |= C_AstFlags_ComptimeKnown;
			expr->type = C_TypeFromAbiType(ctx, &ctx->abi->t_ulonglong, expr);
		} break;
		
		case C_AstKind_ExprFloat:
		{
			expr->h.flags |= C_AstFlags_ComptimeKnown;
			expr->type = C_TypeFromAbiType(ctx, &ctx->abi->t_float, expr);
		} break;
		
		case C_AstKind_ExprDouble:
		{
			expr->h.flags |= C_AstFlags_ComptimeKnown;
			expr->type = C_TypeFromAbiType(ctx, &ctx->abi->t_double, expr);
		} break;
		
		case C_AstKind_ExprString:
		{
			expr->h.flags |= C_AstFlags_ComptimeKnown;
			expr->type = C_CreateArrayType(ctx, C_TypeFromAbiType(ctx, &ctx->abi->t_char, expr), expr->value_str.size + 1);
		} break;
		
		case C_AstKind_ExprWideString:
		{
			expr->h.flags |= C_AstFlags_ComptimeKnown;
			expr->type = C_CreateArrayType(ctx, C_TypeFromAbiType(ctx, &ctx->abi->t_ushort, expr), expr->value_str.size + 1);
		} break;
		
		case C_AstKind_ExprCompoundLiteral:
		{
			expr->type = expr->compound.type;
			C_ResolveTypeWithInitializer(ctx, &expr->type, &expr->compound.init, 0);
		} break;
		
		case C_AstKind_ExprInitializer:
		{
			
		} break;
		
		case C_AstKind_ExprInitializerMember:
		{
			
		} break;
		
		case C_AstKind_ExprInitializerIndex:
		{
			
		} break;
		
		// TODO(ljre)
		case C_AstKind_Expr1Plus:
		case C_AstKind_Expr1Negative:
		case C_AstKind_Expr1Not:
		case C_AstKind_Expr1LogicalNot:
		case C_AstKind_Expr1Deref:
		case C_AstKind_Expr1Ref:
		case C_AstKind_Expr1PrefixInc:
		case C_AstKind_Expr1PrefixDec:
		case C_AstKind_Expr1PostfixInc:
		case C_AstKind_Expr1PostfixDec:
		case C_AstKind_Expr1Sizeof:
		case C_AstKind_Expr1SizeofType:
		case C_AstKind_Expr1Cast:
		{
			
		} break;
		
		case C_AstKind_Expr2Add:
		case C_AstKind_Expr2Sub:
		case C_AstKind_Expr2Mul:
		case C_AstKind_Expr2Div:
		case C_AstKind_Expr2Mod:
		case C_AstKind_Expr2LThan:
		case C_AstKind_Expr2GThan:
		case C_AstKind_Expr2LEqual:
		case C_AstKind_Expr2GEqual:
		case C_AstKind_Expr2Equals:
		case C_AstKind_Expr2NotEquals:
		case C_AstKind_Expr2LeftShift:
		case C_AstKind_Expr2RightShift:
		case C_AstKind_Expr2And:
		case C_AstKind_Expr2Or:
		case C_AstKind_Expr2Xor:
		case C_AstKind_Expr2LogicalAnd:
		case C_AstKind_Expr2LogicalOr:
		case C_AstKind_Expr2Assign:
		case C_AstKind_Expr2AssignAdd:
		case C_AstKind_Expr2AssignSub:
		case C_AstKind_Expr2AssignMul:
		case C_AstKind_Expr2AssignDiv:
		case C_AstKind_Expr2AssignMod:
		case C_AstKind_Expr2AssignLeftShift:
		case C_AstKind_Expr2AssignRightShift:
		case C_AstKind_Expr2AssignAnd:
		case C_AstKind_Expr2AssignOr:
		case C_AstKind_Expr2AssignXor:
		case C_AstKind_Expr2Comma:
		case C_AstKind_Expr2Call:
		case C_AstKind_Expr2Index:
		case C_AstKind_Expr2Access:
		case C_AstKind_Expr2DerefAccess:
		{
			
		} break;
		
		case C_AstKind_Expr3Condition:
		{
			
		} break;
		
		default: Unreachable();
	}
	
	return expr;
}

// NOTE(ljre): Same flags as C_ResolveType
internal bool32
C_ResolveTypeWithInitializer(C_Context* ctx, C_AstType** ptype, C_AstExpr** pinit, uint32 flags)
{
	// TODO(ljre)
	
	return true;
}

internal void
C_ResolveStmt(C_Context* ctx, C_AstStmt* stmt)
{
	switch (stmt->h.kind)
	{
		case C_AstKind_StmtEmpty: break;
		
		case C_AstKind_StmtCompound:
		{
			ctx->working_scope = stmt->compound.scope;
			
			for (C_AstNode* stmt2 = (void*)stmt->compound.stmts; stmt2; stmt2 = stmt2->next)
			{
				C_AstKind cat = stmt2->kind & C_AstKind__CategoryMask;
				
				if (cat == C_AstKind_Decl)
					C_ResolveLocalDecl(ctx, (void*)stmt2);
				else
					C_ResolveStmt(ctx, (void*)stmt2);
			}
			
			ctx->working_scope = ctx->working_scope->up;
		} break;
		
		// TODO(ljre)
		case C_AstKind_StmtExpr:
		case C_AstKind_StmtIf:
		case C_AstKind_StmtDoWhile:
		case C_AstKind_StmtWhile:
		case C_AstKind_StmtFor:
		case C_AstKind_StmtSwitch:
		case C_AstKind_StmtReturn:
		case C_AstKind_StmtGoto:
		case C_AstKind_StmtLabel:
		case C_AstKind_StmtCase:
		case C_AstKind_StmtBreak:
		case C_AstKind_StmtContinue:
		case C_AstKind_StmtGccAsm:
		{
			
		} break;
		
		default: Unreachable(); break;
	}
}

internal void
C_ResolveLocalDecl(C_Context* ctx, C_AstDecl* decl)
{
	bool32 complete = false;
	C_ResolveType(ctx, decl->type, &complete, 0, NULL);
	
	C_Symbol* sym;
	
	switch (decl->h.kind)
	{
		if(0) case C_AstKind_DeclStatic: sym = C_CreateSymbol(ctx, decl->name, C_SymbolKind_StaticVar, decl);
		if(0) case C_AstKind_DeclRegister:
		/* */ case C_AstKind_Decl:
		/* */ case C_AstKind_DeclAuto: sym = C_CreateSymbol(ctx, decl->name, C_SymbolKind_LocalVar, decl);
		{
			if (!complete)
			{
				C_NodeError(ctx, decl, "variable cannot have incomplete type '%s'.", C_CStringFromType(ctx, decl->type));
				sym->flags |= C_AstFlags_Poisoned;
			}
			
			if (decl->init)
				C_ResolveExpr(ctx, decl->init);
		} break;
		
		case C_AstKind_DeclExtern:
		{
			sym = C_CreateSymbol(ctx, decl->name, C_SymbolKind_GlobalVarDecl, decl);
			
			if (decl->init)
				C_NodeError(ctx, decl->init, "cannot initialize extern declaration.");
		} break;
		
		// NOTE(ljre): Already handled at parsing stage.
		case C_AstKind_DeclTypedef:
		{} break;
		
		default: Unreachable(); break;
	}
}

internal void
C_FunctionRedeclaration(C_Context* ctx, C_Symbol* sym, C_AstDecl* new_decl)
{
	
}

internal void
C_GlobalVarRedeclaration(C_Context* ctx, C_Symbol* sym, C_AstDecl* new_decl)
{
	
}

internal void
C_ResolveGlobalDecl(C_Context* ctx, C_AstDecl* decl)
{
	bool32 complete = false;
	C_ResolveType(ctx, decl->type, &complete, 1, NULL);
	
	C_Symbol* sym = NULL;
	
	if (decl->name.size == 0)
		return;
	
	switch (decl->h.kind)
	{
		case C_AstKind_Decl:
		{
			if (decl->type->h.kind == C_AstKind_TypeFunction)
			{
				func_decl:;
				sym = C_FindSymbolInScope(ctx, decl->name, 0, ctx->scope);
				
				if (sym)
					C_FunctionRedeclaration(ctx, sym, decl);
				else if (decl->body)
				{
					sym = C_CreateSymbol(ctx, decl->name, C_SymbolKind_Function, decl);
					C_ResolveStmt(ctx, decl->body);
				}
				else
					sym = C_CreateSymbol(ctx, decl->name, C_SymbolKind_FunctionDecl, decl);
			}
			else
			{
				sym = C_FindSymbolInScope(ctx, decl->name, C_SymbolKind_GlobalVarDecl, ctx->scope);
				
				if (sym)
					C_GlobalVarRedeclaration(ctx, sym, decl);
				else
				{
					sym = C_CreateSymbol(ctx, decl->name, C_SymbolKind_GlobalVar, decl);
					
					if (decl->init)
						C_ResolveTypeWithInitializer(ctx, &decl->type, &decl->init, 1);
				}
			}
		} break;
		
		case C_AstKind_DeclStatic:
		{
			if (decl->type->h.kind == C_AstKind_TypeFunction)
			{
				sym = C_FindSymbolInScope(ctx, decl->name, 0, ctx->scope);
				
				if (sym)
					C_FunctionRedeclaration(ctx, sym, decl);
				else
				{
					sym = C_CreateSymbol(ctx, decl->name, C_SymbolKind_StaticFunction, decl);
					
					if (decl->body)
						C_ResolveStmt(ctx, decl->body);
				}
			}
			else
			{
				sym = C_FindSymbolInScope(ctx, decl->name, 0, ctx->scope);
				
				if (sym)
					C_GlobalVarRedeclaration(ctx, sym, decl);
				else
				{
					sym = C_CreateSymbol(ctx, decl->name, C_SymbolKind_StaticVar, decl);
					
					if (decl->init)
						C_ResolveTypeWithInitializer(ctx, &decl->type, &decl->init, 1);
				}
			}
		} break;
		
		case C_AstKind_DeclExtern:
		{
			if (decl->type->h.kind == C_AstKind_TypeFunction)
				goto func_decl;
			
			sym = C_FindSymbolInScope(ctx, decl->name, 0, ctx->scope);
			
			if (sym)
				C_GlobalVarRedeclaration(ctx, sym, decl);
			else
				sym = C_CreateSymbol(ctx, decl->name, C_SymbolKind_GlobalVarDecl, decl);
			
			if (decl->init)
				C_NodeError(ctx, decl, "cannot initialize extern declaration.");
		} break;
		
		case C_AstKind_DeclTypedef:
		{
			// NOTE(ljre): Symbol should already be defined by parser.
		} break;
		
		case C_AstKind_DeclRegister:
		{
			C_NodeError(ctx, decl, "'register' storage class is now allowed in global scope.");
		} break;
		
		case C_AstKind_DeclAuto:
		{
			C_NodeError(ctx, decl, "'auto' storage class is now allowed in global scope.");
		} break;
		
		case C_AstKind_DeclEnumEntry:
		default: Unreachable(); break;
	}
}

internal bool32
C_ResolveAst(C_Context* ctx)
{
	ctx->working_scope = ctx->scope;
	
	for (C_AstDecl* decl = ctx->ast; decl; decl = (void*)decl->h.next)
		C_ResolveGlobalDecl(ctx, decl);
	
	return ctx->error_count == 0;
}
