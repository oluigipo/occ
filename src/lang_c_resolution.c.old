internal C_SymbolStack* C_ResolveBlock(C_Context* ctx, C_Node* block);
internal C_Node* C_ResolveExpr(C_Context* ctx, C_Node* expr);
internal C_Node* C_AddCastToExprIfNeeded(C_Context* ctx, C_Node* expr, C_Node* type);
internal void C_ResolveInitializerOfType(C_Context* ctx, C_Node* init, C_Node* type);

internal C_Node*
C_CreateNodeFrom(C_Context* ctx, C_Node* other, C_NodeKind kind)
{
	C_Node* result = Arena_Push(ctx->persistent_arena, sizeof *result);
	
	result->kind = kind;
	result->flags = C_NodeFlags_Implicit;
	result->line = other->line;
	result->col = other->col;
	result->lexfile = other->lexfile;
	result->leading_spaces = Str("");
	
	return result;
}

internal C_Node*
C_CreateArrayType(C_Context* ctx, C_Node* of, uintsize len)
{
	C_Node* result = C_CreateNodeFrom(ctx, of, C_NodeKind_TypeArray);
	result->type = of;
	result->length = len;
	result->size = len * of->size;
	result->alignment_mask = of->alignment_mask;
	
	return result;
}

internal C_Node*
C_CreatePointerType(C_Context* ctx, C_Node* of)
{
	C_Node* result = C_CreateNodeFrom(ctx, of, C_NodeKind_TypePointer);
	result->type = of;
	result->size = ctx->abi->t_ptr.size;
	result->alignment_mask = ctx->abi->t_ptr.alignment_mask;
	
	return result;
}

internal C_SymbolStack*
C_PushSymbolStack(C_Context* ctx)
{
	C_SymbolStack* stack = Arena_Push(ctx->persistent_arena, sizeof *stack);
	
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
C_PopSymbolStack(C_Context* ctx)
{
	ctx->previous_symbol_stack = ctx->symbol_stack;
	ctx->symbol_stack = ctx->symbol_stack->up;
}

internal inline bool32
C_IsSymbolType(C_Symbol* sym)
{
	return (sym->kind == C_SymbolKind_Typename ||
			sym->kind == C_SymbolKind_Struct ||
			sym->kind == C_SymbolKind_Union ||
			sym->kind == C_SymbolKind_Enum);
}

internal C_Symbol*
C_FindSymbol(C_Context* ctx, String name, C_SymbolKind specific)
{
	uint64 hash = SimpleHash(name);
	C_SymbolStack* stack = ctx->symbol_stack;
	
	while (stack)
	{
		C_Symbol* it = stack->symbols;
		
		for (; it; it = it->next)
		{
			if (it->name_hash == hash && (specific ? specific == it->kind : it->kind < C_SymbolKind__OwnNamespace))
				return it;
		}
		
		stack = stack->up;
	}
	
	return NULL;
}

internal C_Symbol*
C_FindSymbolInNamespaceHashed(C_Context* ctx, uint64 hash, C_SymbolKind specific, C_SymbolStack* namespace)
{
	for (C_Symbol* it = namespace->symbols; it; it = it->next)
	{
		if (it->name_hash == hash && (specific ? specific == it->kind : it->kind < C_SymbolKind__OwnNamespace))
			return it;
	}
	
	for (C_SymbolStack* nested = namespace->down; nested; nested = nested->next)
	{
		C_Symbol* sym = C_FindSymbolInNamespaceHashed(ctx, hash, specific, nested);
		if (sym)
			return sym;
	}
	
	return NULL;
}

internal C_Symbol*
C_FindSymbolInNamespace(C_Context* ctx, String name, C_SymbolKind specific, C_SymbolStack* namespace)
{
	return C_FindSymbolInNamespaceHashed(ctx, SimpleHash(name), specific, namespace);
}

internal C_Symbol*
C_FindSymbolOfIdent(C_Context* ctx, C_Node* ident, C_SymbolKind specific)
{
	if (!ident->symbol)
		ident->symbol = C_FindSymbol(ctx, ident->name, specific);
	
	return ident->symbol;
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
C_SymbolAlreadyDefinedInThisScope(C_Context* ctx, String name, C_SymbolKind specific)
{
	uint64 hash = SimpleHash(name);
	C_Symbol* it = ctx->symbol_stack->symbols;
	
	for (; it; it = it->next)
	{
		if (it->name_hash == hash && (specific ? specific == it->kind : it->kind < C_SymbolKind__OwnNamespace))
			return it;
	}
	
	return NULL;
}

internal C_Node*
C_TypeFromTypename(C_Context* ctx, C_Node* type)
{
	while (type->kind == C_NodeKind_TypeBaseTypename ||
		   !type->body && type->symbol && (type->kind == C_NodeKind_TypeBaseStruct ||
										   type->kind == C_NodeKind_TypeBaseUnion))
	{
		C_Symbol* sym = type->symbol;
		Assert(sym);
		
		type = sym->type;
	}
	
	return type;
}

internal C_Symbol*
C_UpdateSymbol(C_Symbol* sym, C_SymbolKind kind, C_Node* type, C_Node* decl)
{
	sym->kind = kind;
	sym->type = type;
	sym->decl = decl;
	
	return sym;
}

internal C_Symbol*
C_CreateSymbol(C_Context* ctx, String name, C_SymbolKind kind, C_Node* type, C_Node* decl)
{
	C_Symbol* result = Arena_Push(ctx->persistent_arena, sizeof *result);
	C_SymbolStack* stack = ctx->symbol_stack;
	
	result->next = stack->symbols;
	stack->symbols = result;
	
	result->kind = kind;
	result->name = name;
	result->type = type;
	result->name_hash = SimpleHash(name);
	result->decl = decl;
	
	return result;
}

internal void
C_WriteTypeToPersistentArena(C_Context* ctx, C_Node* type)
{
	int32 count = 0;
	for (C_Node* it = type; it; it = it->type, ++count);
	
	C_Node** stack = Arena_Push(ctx->stage_arena, count * sizeof(*stack));
	C_Node* it = type;
	for (int32 i = 0; it; it = it->type, ++i)
		stack[i] = it;
	
	// NOTE(ljre): Print base type
	{
		C_Node* base = type;
		
		while (!C_IsBaseType(base->kind))
			base = base->type;
		
		if (base->flags & C_NodeFlags_Const)
			Arena_PushMemory(ctx->persistent_arena, 6, "const ");
		if (base->flags & C_NodeFlags_Volatile)
			Arena_PushMemory(ctx->persistent_arena, 9, "volatile ");
		
		switch (base->kind)
		{
			case C_NodeKind_TypeBaseChar:
			{
				if (base->flags & C_NodeFlags_Signed)
					Arena_PushMemory(ctx->persistent_arena, 7, "signed ");
				else if (base->flags & C_NodeFlags_Unsigned)
					Arena_PushMemory(ctx->persistent_arena, 9, "unsigned ");
				
				Arena_PushMemory(ctx->persistent_arena, 4, "char");
			} break;
			
			case C_NodeKind_TypeBaseInt:
			{
				if (base->flags & C_NodeFlags_Unsigned)
					Arena_PushMemory(ctx->persistent_arena, 9, "unsigned ");
				
				if (base->flags & C_NodeFlags_LongLong)
					Arena_PushMemory(ctx->persistent_arena, 10, "long long ");
				else if (base->flags & C_NodeFlags_Long)
					Arena_PushMemory(ctx->persistent_arena, 5, "long ");
				else if (base->flags & C_NodeFlags_Short)
					Arena_PushMemory(ctx->persistent_arena, 6, "short ");
				
				Arena_PushMemory(ctx->persistent_arena, 3, "int");
			} break;
			
			case C_NodeKind_TypeBaseFloat:
			{
				Arena_PushMemory(ctx->persistent_arena, 5, "float");
			} break;
			
			case C_NodeKind_TypeBaseDouble:
			{
				Arena_PushMemory(ctx->persistent_arena, 6, "double");
			} break;
			
			case C_NodeKind_TypeBaseVoid:
			{
				Arena_PushMemory(ctx->persistent_arena, 4, "void");
			} break;
			
			case C_NodeKind_TypeBaseBool:
			{
				Arena_PushMemory(ctx->persistent_arena, 6, "_Bool");
			} break;
			
			case C_NodeKind_TypeBaseTypename:
			{
				Arena_PushMemory(ctx->persistent_arena, StrFmt(base->name));
			} break;
			
			if (0) case C_NodeKind_TypeBaseStruct: Arena_PushMemory(ctx->persistent_arena, 7, "struct ");
			if (0) case C_NodeKind_TypeBaseUnion: Arena_PushMemory(ctx->persistent_arena, 6, "union ");
			if (0) case C_NodeKind_TypeBaseEnum: Arena_PushMemory(ctx->persistent_arena, 5, "enum ");
			{
				if (base->name.size > 0)
					Arena_PushMemory(ctx->persistent_arena, StrFmt(base->name));
				else
					Arena_PushMemory(ctx->persistent_arena, 11, "(anonymous)");
			} break;
		}
	}
	
	// NOTE(ljre): Print prefixes
	it = type;
	for (int32 i = count - 2; i >= 0; it = it->type, --i)
	{
		C_Node* curr = stack[i];
		C_Node* prev = stack[i+1];
		
		if (curr->kind != C_NodeKind_TypePointer)
			continue;
		
		if (prev->kind == C_NodeKind_TypeArray || prev->kind == C_NodeKind_TypeFunction)
			Arena_PushMemory(ctx->persistent_arena, 2, "(*");
		else
			Arena_PushMemory(ctx->persistent_arena, 1, "*");
		
		if (curr->flags & C_NodeFlags_Const)
			Arena_PushMemory(ctx->persistent_arena, 6, " const");
		if (curr->flags & C_NodeFlags_Volatile)
			Arena_PushMemory(ctx->persistent_arena, 9, " volatile");
	}
	
	// NOTE(ljre): Print postfixes
	it = type;
	for (int32 i = count - 1; i >= 0; it = it->type, --i)
	{
		C_Node* curr = stack[i];
		C_Node* next = i > 0 ? stack[i-1] : NULL;
		
		switch (curr->kind)
		{
			case C_NodeKind_TypeFunction:
			{
				if (next && next->kind == C_NodeKind_TypePointer)
					Arena_PushMemory(ctx->persistent_arena, 1, ")");
				
				Arena_PushMemory(ctx->persistent_arena, 1, "(");
				for (C_Node* param = curr->params;
					 param;
					 (param = param->next) && Arena_PushMemory(ctx->persistent_arena, 2, ", "))
				{
					C_WriteTypeToPersistentArena(ctx, param->type);
				}
				Arena_PushMemory(ctx->persistent_arena, 1, ")");
			} break;
			
			case C_NodeKind_TypeArray:
			{
				if (next && next->kind == C_NodeKind_TypePointer)
					Arena_PushMemory(ctx->persistent_arena, 1, ")");
				
				Arena_PushMemory(ctx->persistent_arena, 1, "[");
				
				if (!curr->expr)
				{
					if (curr->expr->cannot_be_evaluated_at_compile_time)
						Arena_PushMemory(ctx->persistent_arena, 3, "VLA");
					else
						Arena_Printf(ctx->persistent_arena, "%llu", curr->expr->value_uint);
				}
				
				Arena_PushMemory(ctx->persistent_arena, 1, "]");
			} break;
			
			default: continue;
		}
	}
}

internal const char*
C_CStringFromType(C_Context* ctx, C_Node* type)
{
	// TODO(ljre): this is for warning and error messages.
	char* result = Arena_End(ctx->persistent_arena);
	
	C_WriteTypeToPersistentArena(ctx, type);
	Arena_PushMemory(ctx->persistent_arena, 1, "");
	
	return result;
}

internal const char*
C_CStringFromNodeKind(C_NodeKind kind)
{
	static const char* const table[C_NodeKind__CategoryCount][33] = {
		// TODO(ljre): Rest of the table.
		//             Search doesn't need to be that fast since this function should only
		//             be called when we are reporting warnings or errors.
		
		[(C_NodeKind_Expr2>>C_NodeKind__Category) - 1] = {
			"+", "-", "*", "/", "%", "<", ">", "<=", ">=", "==", "!=",
			"<<", ">>", "&", "|", "^", "&&", "||", "=", "+=", "-=", "*=",
			"/=", "<<=", ">>=", "&=", "|=", "^=", ",", "function call()",
			"indexing[]", ".", "->",
		},
	};
	
	uintsize cat = (kind>>C_NodeKind__Category) - 1;
	uintsize index = (kind & ~C_NodeKind__CategoryMask) - 1;
	
	Assert(cat < ArrayLength(table));
	Assert(index < ArrayLength(table[cat]));
	
	return table[cat][index];
}

internal int32
C_NodeCount(C_Node* node)
{
	int32 count = 0;
	
	while (node)
	{
		++count;
		node = node->next;
	}
	
	return count;
}

internal int32
C_RankOfType(C_Context* ctx, C_Node* type)
{
	Assert(ArrayLength(C_basic_types_table) >= ArrayLength(ctx->abi->t));
	
	for (int32 i = 0; i < ArrayLength(ctx->abi->t); ++i)
	{
		const C_Node* node = &C_basic_types_table[i];
		if (type->kind == node->kind && (type->flags & node->flags) == node->flags)
			return i;
	}
	
	return -1;
}

internal bool32
C_IsNumericType(C_Context* ctx, C_Node* type)
{
	// TODO(ljre): is this enough?
	return C_RankOfType(ctx, type) != -1;
}

internal bool32
C_IsIncompleteType(C_Context* ctx, C_Node* type)
{
	return type->size == 0;
}

internal bool32
C_IsVoidType(C_Context* ctx, C_Node* type)
{
	type = C_TypeFromTypename(ctx, type);
	
	return type->kind == C_NodeKind_TypeBaseVoid;
}

// NOTE(ljre): Compares two types.
//             Returns 0 if both are the same type, 1 if 'left' is "bigger" than 'right', -1 if
//             'right' is "bigger" than 'left', and -2 (aka C_INVALID) if both types are incompatible.
//
//            'options' bitset:
//                1 - needs to assign 'right' to 'left';
//                2 - needs to do basic arithmetic with 'left' and 'right';
//                4 - needs to do arithmetic with 'left' and 'right';
//
//                NOTE: "basic arithmetic" means *only* operations we can do with pointers and integers, like
//                      +, -, ++, and --.
//
//            'out_error' can be NULL.
internal int32
C_CompareTypes(C_Context* ctx, C_Node* left, C_Node* right, int32 options, bool32* out_error)
{
	// TODO(ljre): Make use of 'options' and 'out_error' to generate warnings or errors.
	
	left = C_TypeFromTypename(ctx, left);
	right = C_TypeFromTypename(ctx, right);
	
	// NOTE(ljre): Same definitions are from the same pointers.
	if (left == right)
		return 0;
	
	switch (left->kind)
	{
		case C_NodeKind_TypeBaseStruct:
		case C_NodeKind_TypeBaseUnion:
		{
			return C_INVALID;
		} break;
		
		case C_NodeKind_TypeBaseVoid:
		{
			if (right->kind == C_NodeKind_TypeBaseVoid)
				return 0;
		} /* fallthrough */
		
		case C_NodeKind_TypeBaseChar:
		case C_NodeKind_TypeBaseInt:
		case C_NodeKind_TypeBaseFloat:
		case C_NodeKind_TypeBaseDouble:
		case C_NodeKind_TypeBaseBool:
		case C_NodeKind_TypeBaseEnum:
		{
			int32 left_rank = C_RankOfType(ctx, left);
			int32 right_rank = C_RankOfType(ctx, right);
			
			if (left_rank == -1 || right_rank == -1)
				return C_INVALID;
			
			return (left_rank > right_rank) ? 1 : (left_rank < right_rank) ? -1 : 0;
		} break;
		
		case C_NodeKind_TypePointer:
		{
			if (right->kind == C_NodeKind_TypePointer)
			{
				if (right->type->kind == C_NodeKind_TypeBaseVoid || left->type->kind == C_NodeKind_TypeBaseVoid)
					return 1;
				
				if (C_CompareTypes(ctx, left->type, right->type, 0, out_error) == 0)
					return 0;
				
				return 1;
			}
			else if (C_IsBaseType(right->kind))
			{
				int32 right_rank = C_RankOfType(ctx, right);
				
				if (right_rank == -1)
					return C_INVALID;
				
				return 1;
			}
			else
				return C_INVALID;
		} break;
		
		case C_NodeKind_TypeFunction:
		{
			if (right->kind != C_NodeKind_TypeFunction)
				return C_INVALID;
			
			C_Node* lparam = left->params;
			C_Node* rparam = right->params;
			
			while (lparam && rparam)
			{
				if (C_CompareTypes(ctx, lparam->type, rparam->type, 0, out_error) != 0)
					return C_INVALID;
				
				lparam = lparam->next;
				rparam = rparam->next;
			}
			
			if (lparam != rparam) // NOTE(ljre): If one of them isn't NULL
				return C_INVALID;
			
			return 0;
		} break;
		
		case C_NodeKind_TypeArray:
		{
			if (right->kind != C_NodeKind_TypeArray ||
				right->length != left->length ||
				C_CompareTypes(ctx, left->type, right->type, 0, out_error) != 0)
			{
				return C_INVALID;
			}
			
			return 0;
		} break;
	}
	
	return C_INVALID;
}

internal bool32
C_IsExprLValue(C_Context* ctx, C_Node* expr)
{
	switch (expr->kind)
	{
		case C_NodeKind_Expr1Deref:
		case C_NodeKind_Expr2Index:
		case C_NodeKind_ExprCompoundLiteral:
		case C_NodeKind_Expr2Access:
		case C_NodeKind_Expr2DerefAccess:
		{
			return true;
		}
		
		case C_NodeKind_ExprIdent:
		{
			C_Symbol* sym = C_FindSymbol(ctx, expr->name, 0);
			return !(sym->kind == C_SymbolKind_Function ||
					 sym->kind == C_SymbolKind_FunctionDecl ||
					 sym->kind == C_SymbolKind_FunctionDecl ||
					 sym->kind == C_SymbolKind_EnumConstant);
		}
	}
	
	return false;
}

internal void
C_TryToEval(C_Context* ctx, C_Node* expr)
{
	if (expr->cannot_be_evaluated_at_compile_time)
		return;
	
	switch (expr->kind)
	{
		case C_NodeKind_ExprInt:
		case C_NodeKind_ExprLInt:
		case C_NodeKind_ExprLLInt:
		case C_NodeKind_ExprUInt:
		case C_NodeKind_ExprULInt:
		case C_NodeKind_ExprULLInt:
		case C_NodeKind_ExprFloat:
		case C_NodeKind_ExprDouble:
		{
			return;
		}
		
		case C_NodeKind_ExprIdent:
		{
			C_Symbol* sym = expr->symbol;
			
			if (sym)
			{
				switch (sym->kind)
				{
					case C_SymbolKind_EnumConstant: return;
				}
			}
		} break;
		
		case C_NodeKind_Expr1Sizeof:
		case C_NodeKind_Expr1SizeofType:
		{
			if (!(expr->flags & C_NodeFlags_Poisoned))
			{
				return;
			}
		} break;
		
		case C_NodeKind_Expr1Cast:
		{
			C_TryToEval(ctx, expr->expr);
			
			if (!expr->expr->cannot_be_evaluated_at_compile_time && C_IsNumericType(ctx, expr->type))
			{
				expr->value_uint = expr->expr->value_uint;
				return;
			}
		} break;
	}
	
	expr->cannot_be_evaluated_at_compile_time = true;
}

internal void
C_ResolveType(C_Context* ctx, C_Node* type, bool32* out_is_complete, bool32 inlined_symbols)
{
	bool32 is_complete = false;
	
	C_SymbolKind kind = C_SymbolKind_Struct;
	C_ABIType* abitype = NULL;
	
	if (type->symbol)
	{
		if (out_is_complete)
			*out_is_complete = !C_IsIncompleteType(ctx, type->symbol->type);
		
		type->size = type->symbol->type->size;
		type->alignment_mask = type->symbol->type->alignment_mask;
		
		return;
	}
	
	switch (type->kind)
	{
		case C_NodeKind_TypeBaseTypename:
		{
			type->symbol = C_FindSymbol(ctx, type->name, C_SymbolKind_Typename);
			is_complete = !C_IsIncompleteType(ctx, type->symbol->type);
		} break;
		
		case C_NodeKind_TypeBaseUnion: kind = C_SymbolKind_Union;
		case C_NodeKind_TypeBaseStruct:
		{
			if (type->name.size > 0)
			{
				C_Symbol* sym = C_FindSymbol(ctx, type->name, kind);
				
				if (sym)
				{
					type->symbol = sym;
					
					if (type->body)
						C_NodeError(ctx, type->body, "redefinition of '%s %S'.",
									kind == C_SymbolKind_Struct ? "struct" : "union",
									StrFmt(type->name));
					else
						is_complete = true;
				}
			}
			
			if (type->body)
			{
				is_complete = true;
				C_Symbol* sym;
				
				if (type->symbol)
					sym = C_CreateSymbol(ctx, StrNull, kind, type, NULL);
				else
				{
					sym = C_CreateSymbol(ctx, type->name, kind, type, NULL);
					type->symbol = sym;
				}
				
				if (inlined_symbols)
					sym->fields = ctx->symbol_stack;
				else
					sym->fields = C_PushSymbolStack(ctx);
				
				C_Node* needs_to_be_the_last = NULL;
				
				for (C_Node* member = type->body; member; member = member->next)
				{
					if (member->kind != C_NodeKind_Decl)
						continue;
					
					if (needs_to_be_the_last)
						C_NodeError(ctx, needs_to_be_the_last, "incomplete flexible array needs to be at the end of struct");
					
					bool32 c;
					C_ResolveType(ctx, member->type, &c, member->name.size == 0);
					
					C_Symbol* membersym = C_CreateSymbol(ctx, member->name, C_SymbolKind_Field, member->type, member);
					
					sym->size = AlignUp(sym->size, member->type->alignment_mask);
					
					if (!c)
					{
						if (member->type->kind == C_NodeKind_TypeArray)
							needs_to_be_the_last = member;
						else
							C_NodeError(ctx, member->type, "incomplete type not allowed in struct.");
					}
					else
					{
						membersym->size = member->type->size;
						membersym->offset = sym->size;
						
						if (kind == C_SymbolKind_Struct)
							sym->size += member->type->size;
						sym->alignment_mask = Max(sym->alignment_mask, member->type->alignment_mask);
					}
				}
				
				sym->size = AlignUp(sym->size, sym->alignment_mask);
				
				type->size = sym->size;
				type->alignment_mask = sym->alignment_mask;
				
				if (!inlined_symbols)
					C_PopSymbolStack(ctx);
			}
		} break;
		
		case C_NodeKind_TypeBaseVoid:
		{
			type->size = 0;
			type->alignment_mask = 0;
		} break;
		
		case C_NodeKind_TypeBaseChar:
		{
			if (type->flags & C_NodeFlags_Unsigned)
				abitype = &ctx->abi->t_uchar;
			else if (type->flags & C_NodeFlags_Signed)
				abitype = &ctx->abi->t_schar;
			else
				abitype = &ctx->abi->t_char;
		} goto set_abi_type;
		
		case C_NodeKind_TypeBaseInt:
		{
			if (type->flags & C_NodeFlags_LongLong)
				abitype = &ctx->abi->t_longlong;
			else if (type->flags & C_NodeFlags_Long)
				abitype = &ctx->abi->t_long;
			else if (type->flags & C_NodeFlags_Short)
				abitype = &ctx->abi->t_short;
			else
				abitype = &ctx->abi->t_int;
			
			if (type->flags & C_NodeFlags_Unsigned)
				abitype = abitype+1; // NOTE(ljre): 'abitype+1' is the same type, but unsigned.
		} goto set_abi_type;
		
		case C_NodeKind_TypeBaseFloat:  abitype = &ctx->abi->t_float;  goto set_abi_type;
		case C_NodeKind_TypeBaseDouble: abitype = &ctx->abi->t_double; goto set_abi_type;
		case C_NodeKind_TypeBaseBool:   abitype = &ctx->abi->t_bool;   goto set_abi_type;
		
		case C_NodeKind_TypePointer:
		{
			is_complete = true;
			
			type->size = ctx->abi->t_ptr.size;
			type->alignment_mask = ctx->abi->t_ptr.alignment_mask;
			C_ResolveType(ctx, type->type, NULL, false);
		} break;
		
		case C_NodeKind_TypeArray:
		{
			bool32 c;
			C_ResolveType(ctx, type->type, &c, false);
			
			if (!c)
				C_NodeError(ctx, type->type, "cannot have array of incomplete type.");
			else
			{
				type->alignment_mask = type->type->alignment_mask;
				type->length = 0;
				
				if (type->expr)
				{
					type->expr = C_ResolveExpr(ctx, type->expr);
					C_Node* casted = C_AddCastToExprIfNeeded(ctx, type->expr, C_SIZE_T);
					if (!casted)
						C_NodeError(ctx, type->expr, "array length needs to be of numeric type.");
					else
					{
						type->expr = casted;
						C_TryToEval(ctx, type->expr);
						
						if (type->expr->cannot_be_evaluated_at_compile_time)
						{
							// TODO(ljre): Maybe warn about VLA?
						}
						else
						{
							type->length = type->expr->value_uint;
							type->size = type->length * type->type->size;
							is_complete = true;
						}
					}
				}
			}
		} break;
		
		case C_NodeKind_TypeFunction:
		{
			bool32 c;
			C_ResolveType(ctx, type->type, &c, false);
			
			if (c)
			{
				C_Node* param = type->params;
				C_ResolveType(ctx, param->type, &c, false);
				if (!c)
				{
					if (C_IsVoidType(ctx, param->type))
						break;
					
					C_NodeError(ctx, param->type, "function parameter cannot be of incomplete type.");
				}
				
				param = param->next;
				
				for (; param; param = param->next)
				{
					C_ResolveType(ctx, param->type, &c, false);
					
					if (!c)
						C_NodeError(ctx, param->type, "function parameter cannot be of incomplete type.");
				}
			}
			else if (!C_IsVoidType(ctx, type->type))
				C_NodeError(ctx, type->type, "function cannot return incomplete type.");
		} break;
	}
	
	if (0) set_abi_type:
	{
		type->size = abitype->size;
		type->alignment_mask = abitype->alignment_mask;
		
		is_complete = true;
	}
	else if (type->symbol && !type->size)
	{
		type->size = type->symbol->type->size;
		type->alignment_mask = type->symbol->type->alignment_mask;
	}
	
	if (out_is_complete)
		*out_is_complete = is_complete;
}

// NOTE(ljre): Returns NULL if types are incompatible.
internal C_Node*
C_AddCastToExprIfNeeded(C_Context* ctx, C_Node* expr, C_Node* type)
{
	C_Node* result;
	
	int32 cmp = C_CompareTypes(ctx, type, expr->type, 0, NULL);
	if (cmp == C_INVALID)
		result = NULL;
	else if (cmp != 0)
	{
		result = C_CreateNodeFrom(ctx, expr, C_NodeKind_Expr1Cast);
		result->type = type;
		result->expr = expr;
	}
	else
		// NOTE(ljre): No cast needed
		result = expr;
	
	return result;
}

internal C_Node*
C_DecayExpr(C_Context* ctx, C_Node* expr)
{
	C_Node* result = expr;
	
	if (expr->type->kind == C_NodeKind_TypeArray || expr->type->kind == C_NodeKind_TypeFunction)
	{
		result = C_CreateNodeFrom(ctx, expr, C_NodeKind_Expr1Ref);
		result->expr = expr;
		result->type = C_CreatePointerType(ctx,
										   (expr->type->kind == C_NodeKind_TypeArray) ? expr->type->type : expr->type);
	}
	
	return result;
}

internal C_Node*
C_PromoteToAtLeast(C_Context* ctx, C_Node* expr, C_Node* type)
{
	if (!expr)
		return NULL;
	
	C_Node* result;
	int32 cmp = C_CompareTypes(ctx, expr->type, type, 0, NULL);
	
	if (cmp == C_INVALID)
		result = NULL;
	else if (cmp < 0)
	{
		result = C_CreateNodeFrom(ctx, expr, C_NodeKind_Expr1Cast);
		result->type = type;
		result->expr = expr;
	}
	else
		result = expr;
	
	return result;
}

internal C_Node*
C_ResolveBuiltinCall(C_Context* ctx, C_Node* expr)
{
	Assert(expr->left->kind == C_NodeKind_ExprIdent);
	
	String name = SliceString(expr->left->name, sizeof("__builtin_")-1);
	
	if (MatchCString("va_start", name))
	{
		// TODO(ljre)
		
		expr->type = C_CreateNodeFrom(ctx, expr, C_NodeKind_TypeBaseVoid);
	}
	else if (MatchCString("va_end", name))
	{
		// TODO(ljre)
		
		expr->type = C_CreateNodeFrom(ctx, expr, C_NodeKind_TypeBaseVoid);
	}
	else
	{
		C_NodeError(ctx, expr->left, "unknown builtin '%S'.", StrFmt(expr->left->name));
	}
	
	return expr;
}

internal C_Node*
C_ResolveExpr(C_Context* ctx, C_Node* expr)
{
	switch (expr->kind)
	{
		case C_NodeKind_Expr2Call:
		{
			if (expr->left->kind == C_NodeKind_ExprIdent &&
				StringStartsWith(expr->left->name, "__builtin_"))
			{
				expr = C_ResolveBuiltinCall(ctx, expr);
				break;
			}
			
			expr->left = C_ResolveExpr(ctx, expr->left);
			C_Node* functype = expr->left->type;
			
			if (expr->left->flags & C_NodeFlags_Poisoned)
				break;
			
			if (functype->kind == C_NodeKind_TypePointer)
				functype = functype->type;
			
			if (functype->kind != C_NodeKind_TypeFunction)
				C_NodeError(ctx, expr->left, "trying to call non-function object.");
			else
			{
				C_Node* funcargs = functype->params;
				
				for (C_Node** arg = &expr->right; *arg; arg = &(*arg)->next, funcargs = funcargs->next)
				{
					if (!funcargs)
					{
						C_NodeError(ctx, *arg, "excessive number of arguments being passed to function.");
						break;
					}
					
					*arg = C_DecayExpr(ctx, C_ResolveExpr(ctx, *arg));
					
					C_Node* c = C_AddCastToExprIfNeeded(ctx, *arg, funcargs->type);
					if (!c)
					{
						C_NodeError(ctx, *arg, "cannot convert from type '%s' to '%s' when passing argument to function.",
									C_CStringFromType(ctx, (*arg)->type),
									C_CStringFromType(ctx, funcargs->type));
					}
					else
					{
						*arg = c;
					}
				}
				
				if (funcargs)
					C_NodeError(ctx, expr, "missing %i arguments when calling function.", C_NodeCount(expr->right));
				
				expr->type = functype->type;
			}
		} break;
		
		case C_NodeKind_Expr2Index:
		{
			expr->left = C_DecayExpr(ctx, C_ResolveExpr(ctx, expr->left));
			expr->right = C_DecayExpr(ctx, C_ResolveExpr(ctx, expr->right));
			
			if ((expr->left->flags | expr->right->flags) & C_NodeFlags_Poisoned)
				break;
			
			if (!C_IsNumericType(ctx, expr->left->type))
				C_NodeError(ctx, expr->left, "expected numeric type when indexing.");
			else if (!C_IsNumericType(ctx, expr->right->type))
				C_NodeError(ctx, expr->right, "expected numeric type when indexing.");
			else
			{
				bool32 error = false;
				int32 cmp = C_CompareTypes(ctx, expr->left->type, expr->right->type, 2, &error);
				
				if (!error)
				{
					// NOTE(ljre): Swap, make sure pointer is 'expr->left'.
					if (cmp <= 0)
					{
						C_Node* temp = expr->left;
						expr->left = expr->right;
						expr->right = temp;
					}
					
					if (expr->left->type->kind != C_NodeKind_TypePointer)
						C_NodeError(ctx, expr, "expected a pointer type when indexing.");
					else if (expr->right->type->kind == C_NodeKind_TypePointer)
						// TODO(ljre): maybe void* + T*?
						C_NodeError(ctx, expr, "expected a numeric type when indexing.");
					else if (C_IsIncompleteType(ctx, expr->left->type->type))
						C_NodeError(ctx, expr, "cannot deref pointer to incomplete type.");
					else
						expr->type = expr->left->type->type;
				}
			}
		} break;
		
		case C_NodeKind_Expr1Cast:
		{
			expr->expr = C_DecayExpr(ctx, C_ResolveExpr(ctx, expr->expr));
			if (expr->expr->flags & C_NodeFlags_Poisoned)
				break;
			
			int32 cmp = C_CompareTypes(ctx, expr->expr->type, expr->type, 0, NULL);
			
			if (cmp == C_INVALID)
			{
				C_NodeError(ctx, expr, "cannot convert from type '%s' to '%s'.",
							C_CStringFromType(ctx, expr->expr->type),
							C_CStringFromType(ctx, expr->type));
			}
		} break;
		
		case C_NodeKind_Expr3Condition:
		{
			expr->left = C_DecayExpr(ctx, C_ResolveExpr(ctx, expr->left));
			expr->middle = C_DecayExpr(ctx, C_ResolveExpr(ctx, expr->middle));
			expr->right = C_DecayExpr(ctx, C_ResolveExpr(ctx, expr->right));
			
			if ((expr->left->flags | expr->middle->flags | expr->right->flags) & C_NodeFlags_Poisoned)
				break;
			
			C_Node* c = C_PromoteToAtLeast(ctx, expr->left, C_INT);
			if (!c)
				C_NodeError(ctx, expr->left, "condition is not of numeric type.");
			
			int32 cmp = C_CompareTypes(ctx, expr->middle->type, expr->right->type, 0, NULL);
			if (cmp == C_INVALID)
				C_NodeError(ctx, expr, "both sides of ternary operator are incompatible.");
			else if (cmp < 0)
				expr->type = expr->middle->type;
			else // if (cmp > 0 || cmp == 0)
				expr->type = expr->right->type;
		} break;
		
		case C_NodeKind_ExprCompoundLiteral:
		{
			bool32 c = false;
			C_ResolveType(ctx, expr->type, &c, false);
			
			if (!c)
				C_NodeError(ctx, expr->type, "type '%s' is incomplete.", C_CStringFromType(ctx, expr->type));
			else
				C_ResolveInitializerOfType(ctx, expr->init, expr->type);
		} break;
		
		case C_NodeKind_ExprInitializer:
		{
			for (C_Node** it = &expr->init; *it; *it = (*it)->next)
			{
				C_Node* node = *it;
				
				if (node->kind != C_NodeKind_ExprInitializerMember && node->kind != C_NodeKind_ExprInitializerIndex)
					*it = C_DecayExpr(ctx, C_ResolveExpr(ctx, *it));
				else
				{
					if (node->right)
						node->right = C_ResolveExpr(ctx, node->right);
					
					loop:;
					switch (node->kind)
					{
						case C_NodeKind_ExprInitializerMember:
						{
							if (node->middle)
							{
								node = node->middle;
								goto loop;
							}
						} break;
						
						case C_NodeKind_ExprInitializerIndex:
						{
							node->left = C_ResolveExpr(ctx, node->left);
							C_Node* promoted = C_PromoteToAtLeast(ctx, node->left, C_INT);
							if (!promoted)
								C_NodeError(ctx, node->left, "array initializer index needs to be of integer type.");
							else
							{
								node->left = promoted;
								C_TryToEval(ctx, node->left);
								
								if (node->middle)
								{
									node = node->middle;
									goto loop;
								}
							}
						} break;
					}
				}
			}
		} break;
		
		case C_NodeKind_Expr2Comma:
		{
			expr->left = C_ResolveExpr(ctx, expr->left);
			expr->right = C_ResolveExpr(ctx, expr->right);
			
			if ((expr->left->flags | expr->right->flags) & C_NodeFlags_Poisoned)
				break;
			
			expr->type = expr->right->type;
		} break;
		
		case C_NodeKind_Expr2Access:
		case C_NodeKind_Expr2DerefAccess:
		{
			expr->left = C_ResolveExpr(ctx, expr->left);
			
			if (expr->left->flags & C_NodeFlags_Poisoned)
				break;
			
			C_Node* type = expr->type;
			if (expr->kind == C_NodeKind_Expr2DerefAccess)
			{
				if (type->kind != C_NodeKind_TypePointer)
				{
					C_NodeError(ctx, expr, "cannot deref non-pointer type.");
					break;
				}
				
				type = type->next;
			}
			
			type = C_TypeFromTypename(ctx, type);
			
			if (type->kind == C_NodeKind_TypeBaseUnion || type->kind == C_NodeKind_TypeBaseStruct)
			{
				C_Symbol* type_symbol = type->symbol;
				Assert(type_symbol->kind == C_SymbolKind_Struct || type_symbol->kind == C_SymbolKind_Union);
				
				C_Symbol* field = C_FindSymbolInNamespace(ctx, expr->name, C_SymbolKind_Field, type_symbol->fields);
				if (!field)
					C_NodeError(ctx, expr, "field '%S' does not exist in type '%s'.", StrFmt(expr->name),
								C_CStringFromType(ctx, type));
				else
				{
					expr->symbol = field;
					expr->type = field->type;
				}
			}
			else
				C_NodeError(ctx, expr->left, "trying to access non-union-or-struct type.");
		} break;
		
		case C_NodeKind_Expr2LThan:
		case C_NodeKind_Expr2GThan:
		case C_NodeKind_Expr2LEqual:
		case C_NodeKind_Expr2GEqual:
		case C_NodeKind_Expr2Equals:
		case C_NodeKind_Expr2NotEquals:
		case C_NodeKind_Expr2LogicalAnd:
		case C_NodeKind_Expr2LogicalOr:
		{
			expr->left = C_DecayExpr(ctx, C_ResolveExpr(ctx, expr->left));
			expr->right = C_DecayExpr(ctx, C_ResolveExpr(ctx, expr->right));
			
			expr->type = C_INT;
		} break;
		
		{
			// 0: it's just an assignment
			// 1: + and -
			// 2: anything else
			int32 arithmetic_level;
			
			case C_NodeKind_Expr2Assign:
			arithmetic_level = 0;
			
			if (0)
			{
				case C_NodeKind_Expr2AssignAdd:
				case C_NodeKind_Expr2AssignSub:
				arithmetic_level = 1;
			}
			if (0)
			{
				case C_NodeKind_Expr2AssignMul:
				case C_NodeKind_Expr2AssignDiv:
				case C_NodeKind_Expr2AssignMod:
				case C_NodeKind_Expr2AssignLeftShift:
				case C_NodeKind_Expr2AssignRightShift:
				case C_NodeKind_Expr2AssignAnd:
				case C_NodeKind_Expr2AssignOr:
				case C_NodeKind_Expr2AssignXor:
				arithmetic_level = 2;
			}
			
			expr->left = C_DecayExpr(ctx, C_ResolveExpr(ctx, expr->left));
			
			expr->right = C_DecayExpr(ctx, C_ResolveExpr(ctx, expr->right));
			expr->right = C_PromoteToAtLeast(ctx, expr->right, C_INT);
			
			if ((expr->left->flags | expr->right->flags) & C_NodeFlags_Poisoned)
				break;
			
			if (!C_IsExprLValue(ctx, expr->left))
				C_NodeError(ctx, expr->left, "left-side of assignment needs to be a lvalue.");
			
			expr->type = expr->left->type;
			
			bool32 error = false;
			int32 cmp = C_CompareTypes(ctx, expr->left->type, expr->right->type, 1 << arithmetic_level, &error);
			if (!error)
			{
				if (cmp == C_INVALID)
				{
					if (arithmetic_level > 0)
						C_NodeError(ctx, expr, "invalid operation '%s' with operands of type '%s' and '%s'.",
									C_CStringFromNodeKind(expr->kind),
									C_CStringFromType(ctx, expr->left->type),
									C_CStringFromType(ctx, expr->right->type));
					else
						C_NodeError(ctx, expr, "cannot assign object of type '%s' to object of type '%s'.",
									C_CStringFromType(ctx, expr->right->type),
									C_CStringFromType(ctx, expr->left->type));
				}
				else if (cmp < 0)
				{
					expr->type = expr->right->type;
				}
			}
		} break;
		
		{
			case C_NodeKind_Expr2Add:
			case C_NodeKind_Expr2Sub:;
			bool32 is_simple_arithmetic = true;
			
			if (0)
			{
				case C_NodeKind_Expr2Mul:
				case C_NodeKind_Expr2Div:
				case C_NodeKind_Expr2Mod:
				case C_NodeKind_Expr2And:
				case C_NodeKind_Expr2Or:
				case C_NodeKind_Expr2Xor:
				case C_NodeKind_Expr2LeftShift:
				case C_NodeKind_Expr2RightShift:
				is_simple_arithmetic = false;
			}
			
			expr->left = C_DecayExpr(ctx, C_ResolveExpr(ctx, expr->left));
			expr->left = C_PromoteToAtLeast(ctx, expr->left, C_INT);
			
			expr->right = C_DecayExpr(ctx, C_ResolveExpr(ctx, expr->right));
			expr->right = C_PromoteToAtLeast(ctx, expr->right, C_INT);
			
			if ((expr->left->flags | expr->right->flags) & C_NodeFlags_Poisoned)
				break;
			
			expr->type = expr->left->type;
			
			bool32 error = false;
			int32 cmp = C_CompareTypes(ctx, expr->left->type, expr->right->type,
									   is_simple_arithmetic ? 2 : 4, &error);
			if (!error)
			{
				if (cmp == C_INVALID)
				{
					C_NodeError(ctx, expr, "invalid operation '%s' with operands of type '%s' and '%s'.",
								C_CStringFromNodeKind(expr->kind),
								C_CStringFromType(ctx, expr->left->type),
								C_CStringFromType(ctx, expr->right->type));
				}
				else if (cmp < 0)
				{
					expr->type = expr->right->type;
				}
			}
		} break;
		
		case C_NodeKind_Expr1Ref:
		{
			// NOTE(ljre): The unary '&' operator does *not* decay the expression.
			expr->expr = C_ResolveExpr(ctx, expr->expr);
			expr->type = C_CreatePointerType(ctx, expr->expr->type);
			
			if (expr->expr->flags & C_NodeFlags_Poisoned)
				break;
			
			if (!C_IsExprLValue(ctx, expr->expr))
				C_NodeError(ctx, expr->expr, "operand is not a lvalue.");
		} break;
		
		case C_NodeKind_Expr1PrefixInc:
		case C_NodeKind_Expr1PrefixDec:
		case C_NodeKind_Expr1PostfixInc:
		case C_NodeKind_Expr1PostfixDec:
		{
			expr->expr = C_ResolveExpr(ctx, expr->expr);
			expr->type = expr->expr->type;
			
			if (expr->expr->flags & C_NodeFlags_Poisoned)
				break;
			
			if (!C_IsExprLValue(ctx, expr->expr))
				C_NodeError(ctx, expr->expr, "operand is not a lvalue.");
			
			if (expr->type->kind == C_NodeKind_TypePointer &&
				expr->type->type->kind == C_NodeKind_TypeFunction)
			{
				C_NodeError(ctx, expr, "cannot use ++ or -- operators on pointer-to-function object.");
			}
			else if (!C_IsNumericType(ctx, expr))
			{
				C_NodeError(ctx, expr, "++ and -- operators expects operand of numeric value.");
			}
			
			// TODO(ljre): Check if this is needed.
			//expr = C_PromoteToAtLeast(expr, C_INT);
		} break;
		
		case C_NodeKind_Expr1Sizeof:
		{
			// NOTE(ljre): The 'sizeof' operator does *not* decay the expression.
			expr->expr = C_ResolveExpr(ctx, expr->expr);
			expr->type = C_SIZE_T;
			
			if (expr->expr->flags & C_NodeFlags_Poisoned)
				break;
			
			C_Node* type = expr->expr->type;
			if (0) case C_NodeKind_Expr1SizeofType:
			{
				type = expr->type;
			}
			
			if (type->kind == C_NodeKind_TypeFunction)
				C_NodeError(ctx, expr, "cannot take sizeof of function.");
			else if (C_IsIncompleteType(ctx, expr->type))
				C_NodeError(ctx, expr, "cannot take sizeof of incomplete type.");
			else
				expr->value_uint = expr->type->size;
		} break;
		
		case C_NodeKind_Expr1Deref:
		{
			expr->expr = C_ResolveExpr(ctx, expr->expr);
			
			if (expr->expr->flags & C_NodeFlags_Poisoned)
				break;
			
			if (expr->type->kind != C_NodeKind_TypePointer)
				C_NodeError(ctx, expr, "cannot dereference a non-pointer object.");
			else if (C_IsIncompleteType(ctx, expr->type))
				C_NodeError(ctx, expr, "cannot dereference pointer to incomplete object.");
			else
				expr->type = expr->expr->type->type;
		} break;
		
		case C_NodeKind_Expr1Plus:
		case C_NodeKind_Expr1Negative:
		case C_NodeKind_Expr1Not:
		case C_NodeKind_Expr1LogicalNot:
		{
			expr->expr = C_ResolveExpr(ctx, expr->expr);
			expr->type = C_PromoteToAtLeast(ctx, expr->expr->type, C_INT);
		} break;
		
		case C_NodeKind_ExprIdent:
		{
			C_Symbol* sym = C_FindSymbolOfIdent(ctx, expr, 0);
			
			if (!sym)
				C_NodeError(ctx, expr, "unknown identifier '%S'.", StrFmt(expr->name));
			else
				expr->type = sym->type;
		} break;
		
		case C_NodeKind_ExprInt: expr->type = C_INT; break;
		case C_NodeKind_ExprLInt: expr->type = C_LINT; break;
		case C_NodeKind_ExprLLInt: expr->type = C_LLINT; break;
		case C_NodeKind_ExprUInt: expr->type = C_UINT; break;
		case C_NodeKind_ExprULInt: expr->type = C_LUINT; break;
		case C_NodeKind_ExprULLInt: expr->type = C_LLUINT; break;
		case C_NodeKind_ExprFloat: expr->type = C_FLOAT; break;
		case C_NodeKind_ExprDouble: expr->type = C_DOUBLE; break;
		case C_NodeKind_ExprString:
		case C_NodeKind_ExprWideString:
		{
			expr->type = C_CreateArrayType(ctx,
										   (expr->kind == C_NodeKind_ExprWideString) ? C_USHORT : C_CHAR,
										   expr->value_str.size + 1); // NOTE(ljre): null-terminator
		} break;
		
		default: Unreachable(); break;
	}
	
	if (!expr->type)
	{
		expr->type = C_CreateNodeFrom(ctx, expr, C_NodeKind_Type);
		expr->flags |= C_NodeFlags_Poisoned;
	}
	
	return expr;
}

internal void
C_ResolveInitializerOfType(C_Context* ctx, C_Node* init, C_Node* type)
{
	C_Node* expr = C_ResolveExpr(ctx, init);
	
	// TODO(ljre):
	
	expr->type = type;
}

internal void
C_ResolveDecl(C_Context* ctx, C_Node* decl)
{
	C_SymbolKind sym_kind = C_SymbolKind_Var;
	bool32 is_complete;
	C_ResolveType(ctx, decl->type, &is_complete, false);
	
	// TODO(ljre): Refactor this function -- remove unecessary C_FindSymbol-s
	
	C_Node* type = C_TypeFromTypename(ctx, decl->type);
	
	if (type->flags & C_NodeFlags_Poisoned)
	{
		decl->flags |= C_NodeFlags_Poisoned;
	}
	else switch (type->kind)
	{
		case C_NodeKind_TypeArray:
		{
			if (decl->type->expr)
			{
				if (decl->type->expr->cannot_be_evaluated_at_compile_time)
				{
					if (decl->expr)
						C_NodeError(ctx, decl->type, "cannot initialize variable-length array.");
					
					break;
				}
			}
			else if (!decl->expr)
			{
				C_NodeError(ctx, decl, "array declaration with implicit size needs initializer.");
			}
			
			if (decl->expr)
			{
				decl->expr = C_ResolveExpr(ctx, decl->expr);
				
				if (decl->expr->kind != C_NodeKind_ExprInitializer &&
					decl->expr->kind != C_NodeKind_ExprString)
				{
					C_NodeError(ctx, decl->expr, "this is not a valid array initializer.");
				}
				else if (decl->type->length == 0)
				{
					uint64 length = decl->expr->length;
					
					decl->type->length = length;
					decl->type->size = decl->type->type->size * length;
				}
				else if (decl->expr->length > decl->type->length)
				{
					C_NodeError(ctx, decl->expr, "excessive number of items in array initializer");
				}
			}
		} break;
		
		case C_NodeKind_TypeFunction:
		{
			// TODO(ljre)
			
			sym_kind = C_SymbolKind_FunctionDecl;
		} break;
		
		case C_NodeKind_TypeBaseChar:
		case C_NodeKind_TypeBaseInt:
		case C_NodeKind_TypeBaseFloat:
		case C_NodeKind_TypeBaseDouble:
		case C_NodeKind_TypeBaseVoid:
		case C_NodeKind_TypeBaseBool:
		case C_NodeKind_TypeBaseTypename:
		case C_NodeKind_TypeBaseStruct:
		case C_NodeKind_TypeBaseUnion:
		case C_NodeKind_TypeBaseEnum:
		{
			if (decl->expr && decl->expr->kind == C_NodeKind_ExprInitializer)
			{
				C_Node* resolved;
				bool32 is_struct = false;
				C_Symbol* sym = NULL;
				
				if (decl->type->kind == C_NodeKind_TypeBaseStruct)
				{
					sym = C_FindSymbol(ctx, decl->type->name, C_SymbolKind_Struct);
					is_struct = true;
					
					if (!sym)
						C_NodeError(ctx, decl->type, "'struct %.*s' is an incomplete type.", StrFmt(decl->type->name));
					else
						resolved = sym->type;
				}
				else if (decl->type->kind == C_NodeKind_TypeBaseTypename)
				{
					sym = C_FindSymbol(ctx, decl->type->name, C_SymbolKind_Typename);
					resolved = C_TypeFromTypename(ctx, sym->type);
					
					if (C_IsStructType(resolved->type))
						is_struct = true;
				}
				
				if (sym)
					decl->symbol = sym;
				
				if (is_struct)
				{
					C_ResolveInitializerOfType(ctx, decl->expr, resolved);
					break;
				}
			}
		} break;
		
		case C_NodeKind_TypePointer:
		default:
		{
			if (decl->expr)
			{
				decl->expr = C_ResolveExpr(ctx, decl->expr);
				
				C_Node* newone = C_AddCastToExprIfNeeded(ctx, decl->expr, decl->type);
				if (!newone)
				{
					C_NodeError(ctx, decl->expr, "cannot assign value of type '%s' to '%s'.",
								C_CStringFromType(ctx, decl->expr->type),
								C_CStringFromType(ctx, decl->type));
				}
				else
				{
					decl->expr = newone;
				}
			}
		} break;
	}
	
	if (decl->kind == C_NodeKind_DeclTypedef)
		sym_kind = C_SymbolKind_Typename;
	
	if (decl->name.size > 0 && !decl->symbol)
	{
		C_Symbol* sym = C_CreateSymbol(ctx, decl->name, sym_kind, decl->type, decl);
		decl->symbol = sym;
	}
}

internal void
C_ResolveStmt(C_Context* ctx, C_Node* stmt)
{
	C_Node* cast;
	
	switch (stmt->kind)
	{
		case C_NodeKind_StmtEmpty: break;
		
		case C_NodeKind_StmtExpr:
		case C_NodeKind_StmtReturn:
		{
			stmt->expr = C_ResolveExpr(ctx, stmt->expr);
		} break;
		
		case C_NodeKind_StmtFor:
		{
			if (stmt->init)
			{
				if (stmt->init->kind == C_NodeKind_Decl)
					C_ResolveDecl(ctx, stmt->init);
				else
					stmt->init = C_ResolveExpr(ctx, stmt->init);
			}
			
			// NOTE(ljre): If 'stmt->expr' is null, then the code gen shall generate an 1 constant
			if (stmt->expr)
				stmt->expr = C_ResolveExpr(ctx, stmt->expr);
			
			cast = C_PromoteToAtLeast(ctx, stmt->expr, C_INT);
			if (!cast)
				C_NodeError(ctx, stmt->expr, "condition should be of numeric type.");
			else
				stmt->expr = cast;
			
			if (stmt->iter)
				stmt->iter = C_ResolveExpr(ctx, stmt->iter);
			
			C_ResolveStmt(ctx, stmt->stmt);
		} break;
		
		case C_NodeKind_StmtSwitch:
		{
			stmt->expr = C_ResolveExpr(ctx, stmt->expr);
			
			cast = C_PromoteToAtLeast(ctx, stmt->expr, C_INT);
			if (!cast)
				C_NodeError(ctx, stmt->expr, "switch expression should be of numeric type.");
			else
				stmt->expr = cast;
			
			C_Node* saved_host_switch = ctx->host_switch;
			C_Node* saved_host_break = ctx->host_break;
			
			ctx->host_switch = stmt;
			ctx->host_break = stmt;
			C_ResolveStmt(ctx, stmt->stmt);
			ctx->host_switch = saved_host_switch;
			ctx->host_break = saved_host_break;
		} break;
		
		case C_NodeKind_StmtDoWhile:
		case C_NodeKind_StmtWhile:
		{
			stmt->expr = C_ResolveExpr(ctx, stmt->expr);
			
			cast = C_PromoteToAtLeast(ctx, stmt->expr, C_INT);
			if (!cast)
				C_NodeError(ctx, stmt->expr, "condition should be of numeric type.");
			else
				stmt->expr = cast;
			
			C_Node* saved_host_break = ctx->host_break;
			C_Node* saved_host_continue = ctx->host_continue;
			
			ctx->host_break = stmt;
			ctx->host_continue = stmt;
			C_ResolveStmt(ctx, stmt->stmt);
			ctx->host_break = saved_host_break;
			ctx->host_continue = saved_host_continue;
		} break;
		
		case C_NodeKind_StmtIf:
		{
			stmt->expr = C_ResolveExpr(ctx, stmt->expr);
			
			cast = C_PromoteToAtLeast(ctx, stmt->expr, C_INT);
			if (!cast)
				C_NodeError(ctx, stmt->expr, "condition should be of numeric type.");
			else
				stmt->expr = cast;
			
			C_ResolveStmt(ctx, stmt->stmt);
			if (stmt->stmt2)
				C_ResolveStmt(ctx, stmt->stmt2);
		} break;
		
		case C_NodeKind_StmtLabel:
		{
			C_ResolveStmt(ctx, stmt->stmt);
			
			// TODO(ljre): Add to the "labels table"
		} break;
		
		case C_NodeKind_StmtCase:
		{
			stmt->expr = C_ResolveExpr(ctx, stmt->expr);
			
			cast = C_PromoteToAtLeast(ctx, stmt->expr, C_INT);
			if (!cast)
				C_NodeError(ctx, stmt->expr, "case expression should be of integer type.");
			else
				stmt->expr = cast;
			
			C_TryToEval(ctx, stmt->expr);
			if (stmt->expr->cannot_be_evaluated_at_compile_time)
				C_NodeError(ctx, stmt->expr, "case expression needs to be a constant expression.");
			
			C_ResolveStmt(ctx, stmt->stmt);
			
			// TODO(ljre): Find the host switch statement
		} break;
		
		case C_NodeKind_StmtCompound:
		{
			C_ResolveBlock(ctx, stmt->stmt);
		} break;
		
		case C_NodeKind_StmtGccAsm:
		{
			// TODO(ljre):
		} break;
		
		default: Unreachable(); break;
	}
}

internal C_SymbolStack*
C_ResolveBlock(C_Context* ctx, C_Node* block)
{
	Assert(block->kind == C_NodeKind_StmtCompound);
	
	C_SymbolStack* result = C_PushSymbolStack(ctx);
	
	for (C_Node* stmt = block->stmt; stmt; stmt = stmt->next)
	{
		if ((stmt->kind & C_NodeKind__CategoryMask) == C_NodeKind_Decl)
			C_ResolveDecl(ctx, stmt);
		else
			C_ResolveStmt(ctx, stmt);
	}
	
	C_PopSymbolStack(ctx);
	return result;
}

internal void
C_ResolveGlobalDecl(C_Context* ctx, C_Node* decl)
{
	C_ResolveType(ctx, decl->type, NULL, false);
	
	// TODO(ljre): Merging of multiple declarations specifics and attributes.
	switch (decl->kind)
	{
		case C_NodeKind_Decl:
		{
			if (decl->type->kind == C_NodeKind_TypeFunction)
			{
				decl->kind = C_NodeKind_DeclExtern;
				func_decl:;
				
				if (decl->name.size == 0)
				{
					C_NodeError(ctx, decl, "functions without a name are not allowed.");
					break;
				}
				
				C_Symbol* old_decl = C_FindSymbol(ctx, decl->name, 0);
				C_Symbol* sym;
				
				if (old_decl)
				{
					if (0 != C_CompareTypes(ctx, decl->type, old_decl->type, 0, NULL))
					{
						C_NodeError(ctx, decl, "type of previous function declaration is incompatible.");
					}
					else if (decl->kind != old_decl->decl->kind ||
							 (decl->flags & C_NodeFlags_Inline) != (old_decl->decl->flags & C_NodeFlags_Inline))
					{
						C_NodeError(ctx, decl, "linking mismatch with previous declaration of function.");
					}
					
					sym = old_decl;
					C_UpdateSymbol(sym, C_SymbolKind_FunctionDecl, decl->type, decl);
				}
				else
				{
					sym = C_CreateSymbol(ctx, decl->name, C_SymbolKind_FunctionDecl, decl->type, decl);
				}
				
				if (decl->body)
				{
					sym->kind = C_SymbolKind_Function;
					
					C_PushSymbolStack(ctx);
					for (C_Node* param = decl->type->params; param; param = param->next)
					{
						if (param->name.size > 0)
							C_CreateSymbol(ctx, param->name, C_SymbolKind_Parameter, param->type, param);
					}
					
					sym->locals = C_ResolveBlock(ctx, decl->body);
					C_PopSymbolStack(ctx);
				}
			}
			else if (decl->name.size > 0)
			{
				C_SymbolKind kind = C_SymbolKind_Var;
				C_Symbol* old_sym = C_FindSymbol(ctx, decl->name, 0);
				C_Symbol* sym;
				
				if (old_sym)
				{
					// TODO(ljre): Array semantics
					if (0 != C_CompareTypes(ctx, old_sym->type, decl->type, 0, NULL))
					{
						C_NodeError(ctx, decl, "type of previous function declaration is incompatible.");
					}
					
					sym = old_sym;
					sym->kind = kind;
				}
				else
				{
					sym = C_CreateSymbol(ctx, decl->name, kind, decl->type, decl);
				}
			}
		} break;
		
		case C_NodeKind_DeclStatic:
		{
			if (decl->type->kind == C_NodeKind_TypeFunction)
				goto func_decl;
			
			if (decl->name.size > 0)
			{
				C_SymbolKind kind = C_SymbolKind_StaticVar;
				C_Symbol* old_sym = C_FindSymbol(ctx, decl->name, 0);
				C_Symbol* sym;
				
				if (old_sym)
				{
					// TODO(ljre): Array semantics
					if (0 != C_CompareTypes(ctx, old_sym->type, decl->type, 0, NULL))
					{
						C_NodeError(ctx, decl, "type of previous function declaration is incompatible.");
					}
					
					if (old_sym->decl->kind != C_NodeKind_DeclStatic)
					{
						C_NodeError(ctx, decl, "linking mismatch with previous declaration of function.");
					}
					
					sym = old_sym;
				}
				else
				{
					sym = C_CreateSymbol(ctx, decl->name, kind, decl->type, decl);
				}
				
				(void)sym;
			}
		} break;
		
		case C_NodeKind_DeclExtern:
		{
			// NOTE(ljre): 'extern' keywords makes no difference for functions... Unless it's inline.
			if (decl->type->kind == C_NodeKind_TypeFunction)
			{
				if (decl->flags & C_NodeFlags_Inline)
				{
					// TODO(ljre): inline semantics
				}
				else
				{
					goto func_decl;
				}
			}
			else if (decl->name.size > 0)
			{
				C_SymbolKind kind = C_SymbolKind_VarDecl;
				C_Symbol* old_sym = C_FindSymbol(ctx, decl->name, 0);
				C_Symbol* sym;
				
				if (old_sym)
				{
					// TODO(ljre): Array semantics
					if (0 != C_CompareTypes(ctx, old_sym->type, decl->type, 0, NULL))
					{
						C_NodeError(ctx, decl, "type of previous function declaration is incompatible.");
					}
					
					sym = old_sym;
				}
				else
				{
					sym = C_CreateSymbol(ctx, decl->name, kind, decl->type, decl);
				}
				
				(void)sym;
			}
		} break;
		
		case C_NodeKind_DeclAuto:
		{
			C_NodeError(ctx, decl, "cannot use 'auto' in global scope.");
		} break;
		
		case C_NodeKind_DeclTypedef:
		{
			if (C_SymbolAlreadyDefinedInThisScope(ctx, decl->name, C_SymbolKind_Typename))
				C_NodeError(ctx, decl, "redefinition of type '%S'.", StrFmt(decl->name));
			
			if (decl->name.size > 0)
				C_CreateSymbol(ctx, decl->name, C_SymbolKind_Typename, decl->type, decl);
		} break;
		
		case C_NodeKind_DeclRegister:
		{
			C_NodeError(ctx, decl, "cannot use 'register' in global scope.");
		} break;
		
		default: Unreachable(); break;
	}
}

internal void
C_FinishSymbolsResolution(C_Context* ctx, C_SymbolStack* scope)
{
	// TODO(ljre): Make arrays definitions with no specified length have length of 1.
	// TODO(ljre): Make 'static' global variables symbols.
}

internal bool32
C_ResolveAst(C_Context* ctx)
{
	Trace();
	
	// NOTE(ljre): Resolve stuff
	C_PushSymbolStack(ctx);
	
	C_Node* global_decl = ctx->ast;
	while (global_decl)
	{
		C_ResolveGlobalDecl(ctx, global_decl);
		global_decl = global_decl->next;
	}
	
	// NOTE(ljre): Don't pop the global stack, we want the global symbols.
	//C_PopSymbolStack(ctx);
	
	C_FinishSymbolsResolution(ctx, ctx->symbol_stack);
	
	return C_error_count == 0;
}
