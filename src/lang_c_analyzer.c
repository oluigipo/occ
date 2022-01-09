internal C_SymbolScope* C_ResolveBlock(C_Context* ctx, C_AstStmt* block);
internal C_AstExpr* C_ResolveExpr(C_Context* ctx, C_AstExpr* expr);
internal C_AstExpr* C_AddCastToExprIfNeeded(C_Context* ctx, C_AstExpr* expr, C_AstType* type);
internal bool32 C_ResolveInitializerOfType(C_Context* ctx, C_AstExpr* init, C_AstType* type);

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
	C_AstType* result = C_CreateNodeFrom(ctx, of, C_AstKind_TypeArray, SizeofPoly(C_AstType, array));
	result->as->array.of = of;
	result->as->array.length = C_CreateNodeFrom(ctx, of, C_AstKind_ExprULLInt, sizeof(C_AstExpr));
	result->as->array.length->h.flags |= C_AstFlags_ComptimeKnown;
	result->as->array.length->value_uint = len;
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

internal LittleMap*
C_GetMapFromSymbolKind(C_Context* ctx, C_SymbolScope* scope, C_SymbolKind kind, bool32 create_new)
{
	LittleMap** map = NULL;
	
	switch (kind)
	{
		case C_SymbolKind_Var:
		case C_SymbolKind_VarDecl:
		case C_SymbolKind_StaticVar:
		case C_SymbolKind_Function:
		case C_SymbolKind_FunctionDecl:
		case C_SymbolKind_Parameter:
		case C_SymbolKind_Field:
		case C_SymbolKind_EnumConstant: map = &scope->names; break;
		case C_SymbolKind_Typename: map = &scope->types; break;
		case C_SymbolKind_Struct: map = &scope->structs; break;
		case C_SymbolKind_Union: map = &scope->unions; break;
		case C_SymbolKind_Enum: map = &scope->enums; break;
		
		default: Unreachable(); break;
	}
	
	if (create_new && !*map)
		*map = LittleMap_Create(ctx->persistent_arena, 32);
	
	return *map;
}

internal uintsize
C_SizeForSymbolKind(C_SymbolKind kind)
{
	switch (kind)
	{
		case C_SymbolKind_Typename:
		case C_SymbolKind_Var:
		case C_SymbolKind_VarDecl:
		case C_SymbolKind_StaticVar: return sizeof(C_Symbol);
		case C_SymbolKind_Function:
		case C_SymbolKind_FunctionDecl: return SizeofPoly(C_Symbol, function);
		case C_SymbolKind_Parameter: return SizeofPoly(C_Symbol, parameter);
		case C_SymbolKind_Field: return SizeofPoly(C_Symbol, field);
		case C_SymbolKind_EnumConstant: return SizeofPoly(C_Symbol, enum_const);
		case C_SymbolKind_Struct:
		case C_SymbolKind_Union: return SizeofPoly(C_Symbol, structure);
		case C_SymbolKind_Enum: return SizeofPoly(C_Symbol, enumerator);
		
		default: Unreachable(); break;
	}
	
	return 0;
}

internal C_Symbol*
C_CreateSymbolInScope(C_Context* ctx, String name, C_SymbolKind kind, C_AstDecl* decl, C_SymbolScope* scope)
{
	C_Symbol* sym = Arena_Push(ctx->persistent_arena, C_SizeForSymbolKind(kind));
	
	sym->kind = kind;
	sym->decl = decl;
	sym->name = name;
	
	LittleMap* map = C_GetMapFromSymbolKind(ctx, scope, kind, true);
	uint64 hash;
	
	if (name.size > 0)
		hash = SimpleHash(name);
	else
		hash = ++ctx->unnamed_count;
	
	LittleMap_InsertWithHash(map, name, sym, hash);
	
	return sym;
}

internal inline C_Symbol*
C_CreateSymbol(C_Context* ctx, String name, C_SymbolKind kind, C_AstDecl* decl)
{
	return C_CreateSymbolInScope(ctx, name, kind, decl, ctx->working_scope);
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
C_SymbolDefinedInScope(C_Context* ctx, String name, C_SymbolKind specific, C_SymbolScope* scope)
{
	LittleMap* map = C_GetMapFromSymbolKind(ctx, scope, specific, false);
	
	if (map)
		return LittleMap_Fetch(map, name);
	else
		return NULL;
}

internal C_Symbol*
C_FindSymbolInScope(C_Context* ctx, String name, C_SymbolKind kind, C_SymbolScope* scope)
{
	uint64 hash = SimpleHash(name);
	
	for (; scope; scope = scope->up)
	{
		LittleMap* map = C_GetMapFromSymbolKind(ctx, scope, kind, false);
		if (!map)
			continue;
		
		C_Symbol* sym = LittleMap_FetchWithCachedHash(map, name, hash);
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
		   !type->as->structure.body && type->h.symbol)
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
		case C_AstKind_TypeVlaArray:
		case C_AstKind_TypeFunction:
		{
			C_AstType* newtype = C_CreateNodeFrom(ctx, type, C_AstKind_TypePointer, SizeofPoly(C_AstType, ptr));
			newtype->as->ptr.to = type;
			newtype->size = ctx->abi->t_ptr.size;
			newtype->alignment_mask = ctx->abi->t_ptr.alignment_mask;
			newtype->is_unsigned = ctx->abi->t_ptr.unsig;
			
			type = newtype;
		} break;
	}
	
	return type;
}

internal bool32
C_TryToEval(C_Context* ctx, C_AstExpr* expr)
{
	return false;
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
			type->h.symbol = C_FindSymbol(ctx, type->as->typedefed.name, C_SymbolKind_Typename);
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
				bool32 named = (type->as->structure.name.size > 0);
				
				if (named)
				{
					sym = C_SymbolDefinedInScope(ctx, type->as->structure.name, C_SymbolKind_Struct, ctx->working_scope);
					
					if (!sym)
						sym = C_FindSymbol(ctx, type->as->structure.name, C_SymbolKind_Struct);
					else
						same_scope = true;
				}
				
				if (type->as->structure.body)
				{
					if (same_scope)
						C_NodeError(ctx, type, "struct redefinition in the same scope is not allowed.");
					else
						sym = C_CreateSymbol(ctx, type->as->structure.name, kind, (void*)type);
					
					C_AstDecl* decl = type->as->structure.body;
					is_complete = true;
					
					C_SymbolScope* scope;
					
					if (flags & 2)
						scope = aux;
					else
						scope = Arena_Push(ctx->persistent_arena, sizeof(C_SymbolScope));
					
					sym->as->structure.fields = scope;
					
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
							field->as->field.offset = size;
						}
						else
						{
							size = Max(size, decl->type->size);
							field->as->field.offset = 0;
						}
					}
					
					type->size = size;
				}
				
				type->h.symbol = sym;
			} break;
		}
		
		case C_AstKind_TypeFunction:
		{
			C_ResolveType(ctx, type->as->function.ret, NULL, 0, NULL);
			
			for (C_AstDecl* param = type->as->function.params; param; param = (void*)param->h.next)
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
			C_ResolveType(ctx, type->as->ptr.to, NULL, 0, NULL);
			
			type->size = ctx->abi->t_ptr.size;
			type->alignment_mask = ctx->abi->t_ptr.alignment_mask;
			type->is_unsigned = true;
			is_complete = true;
		} break;
		
		case C_AstKind_TypeArray:
		{
			bool32 complete;
			C_ResolveType(ctx, type->as->array.of, &complete, 0, NULL);
			
			if (!complete)
				C_NodeError(ctx, type->as->array.of, "arrays cannot be of incomplete type.");
			if (type->as->array.length->value_uint == 0)
				C_NodeError(ctx, type->as->array.length, "arrays cannot have length of 0.");
			
			type->size = type->as->array.length->value_uint * type->as->array.of->size;
			type->alignment_mask = type->as->array.of->alignment_mask;
			is_complete = true;
		} break;
		
		case C_AstKind_TypeVlaArray:
		{
			bool32 complete;
			C_ResolveType(ctx, type->as->array.of, &complete, 0, NULL);
			
			if (!complete)
				C_NodeError(ctx, type->as->array.of, "arrays cannot be of incomplete type.");
			if (type->as->array.length)
			{
				C_ResolveExpr(ctx, type->as->array.length);
				
				if (!C_IsIntegerType(ctx, type->as->array.length->type))
					C_NodeError(ctx, type->as->array.length, "array length needs to be of integral type.");
				else if (C_TryToEval(ctx, type->as->array.length))
				{
					if (!type->as->array.length->type->is_unsigned && type->as->array.length->value_int < 0)
						C_NodeError(ctx, type->as->array.length, "arrays cannot have negative length.");
					else if (type->as->array.length->value_uint == 0)
						C_NodeError(ctx, type->as->array.length, "arrays cannot have length of 0.");
					else
					{
						type->h.kind = C_AstKind_TypeArray;
						type->size = type->as->array.length->value_uint * type->as->array.of->size;
						type->alignment_mask = type->as->array.of->alignment_mask;
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
	// TODO(ljre)
	
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
C_ResolveGlobalDecl(C_Context* ctx, C_AstDecl* decl)
{
	bool32 complete = false;
	C_ResolveType(ctx, decl->type, &complete, 1, NULL);
	
	// TODO(ljre)
	switch (decl->h.kind)
	{
		case C_AstKind_Decl:
		{
			
		} break;
		
		case C_AstKind_DeclStatic:
		{
			
		} break;
		
		case C_AstKind_DeclExtern:
		{
			
		} break;
		
		case C_AstKind_DeclTypedef:
		{
			
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
