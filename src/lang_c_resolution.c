#define LangC_INVALID (-2)
#define LangC_CHAR (&LangC_basic_types_table[0])
#define LangC_SCHAR (&LangC_basic_types_table[1])
#define LangC_UCHAR (&LangC_basic_types_table[2])
#define LangC_SHORT (&LangC_basic_types_table[3])
#define LangC_USHORT (&LangC_basic_types_table[4])
#define LangC_INT (&LangC_basic_types_table[5])
#define LangC_UINT (&LangC_basic_types_table[6])
#define LangC_LINT (&LangC_basic_types_table[7])
#define LangC_LUINT (&LangC_basic_types_table[8])
#define LangC_LLINT (&LangC_basic_types_table[9])
#define LangC_LLUINT (&LangC_basic_types_table[10])
#define LangC_FLOAT (&LangC_basic_types_table[11])
#define LangC_DOUBLE (&LangC_basic_types_table[12])
#define LangC_PTR (&LangC_basic_types_table[13])
#define LangC_BOOL (&LangC_basic_types_table[14])
#define LangC_VOID (&LangC_basic_types_table[15])
#define LangC_SIZE_T (&LangC_basic_types_table[ctx->abi->index_sizet])
#define LangC_PTRDIFF_T (&LangC_basic_types_table[ctx->abi->index_ptrdifft])
#define LangC_IsSimpleType(node) ((node) && (node) >= LangC_CHAR && (node) <= LangC_VOID)
#define LangC_IsStructType(node) ((node) && (node)->kind == LangC_NodeKind_TypeBaseStruct)
#define LangC_IsUnionType(node) ((node) && (node)->kind == LangC_NodeKind_TypeBaseUnion)

// NOTE(ljre): DO NOT MODIFY THESE OBJECTS. ACT AS IF THERE WAS CONST HERE.
internal /* const */ LangC_Node LangC_basic_types_table[] = {
	{ .kind = LangC_NodeKind_TypeBaseChar, },
	{ .kind = LangC_NodeKind_TypeBaseChar, .flags = LangC_NodeFlags_Signed, },
	{ .kind = LangC_NodeKind_TypeBaseChar, .flags = LangC_NodeFlags_Unsigned, },
	{ .kind = LangC_NodeKind_TypeBaseInt, .flags = LangC_NodeFlags_Short, },
	{ .kind = LangC_NodeKind_TypeBaseInt, .flags = LangC_NodeFlags_Short | LangC_NodeFlags_Unsigned, },
	{ .kind = LangC_NodeKind_TypeBaseInt, },
	{ .kind = LangC_NodeKind_TypeBaseInt, .flags = LangC_NodeFlags_Unsigned, },
	{ .kind = LangC_NodeKind_TypeBaseInt, .flags = LangC_NodeFlags_Long, },
	{ .kind = LangC_NodeKind_TypeBaseInt, .flags = LangC_NodeFlags_Long | LangC_NodeFlags_Unsigned, },
	{ .kind = LangC_NodeKind_TypeBaseInt, .flags = LangC_NodeFlags_LongLong, },
	{ .kind = LangC_NodeKind_TypeBaseInt, .flags = LangC_NodeFlags_LongLong | LangC_NodeFlags_Unsigned, },
	{ .kind = LangC_NodeKind_TypeBaseFloat, },
	{ .kind = LangC_NodeKind_TypeBaseDouble, },
	{ .kind = LangC_NodeKind_TypePointer, .type = LangC_VOID, },
	{ .kind = LangC_NodeKind_TypeBaseBool, },
	{ .kind = LangC_NodeKind_TypeBaseVoid, },
};

internal LangC_SymbolStack* LangC_ResolveBlock(LangC_Context* ctx, LangC_Node* block);
internal LangC_Node* LangC_ResolveExpr(LangC_Context* ctx, LangC_Node* expr);
internal LangC_Node* LangC_AddCastToExprIfNeeded(LangC_Context* ctx, LangC_Node* expr, LangC_Node* type);

internal LangC_Node*
LangC_CreateNodeFrom(LangC_Context* ctx, LangC_Node* other, LangC_NodeKind kind)
{
	LangC_Node* result = Arena_Push(ctx->persistent_arena, sizeof *result);
	
	result->kind = kind;
	result->flags |= LangC_NodeFlags_Implicit;
	result->line = other->line;
	result->col = other->col;
	result->lexfile = other->lexfile;
	result->leading_spaces = Str("");
	
	return result;
}

internal LangC_Node*
LangC_CreateArrayType(LangC_Context* ctx, LangC_Node* of, uintsize len)
{
	LangC_Node* result = LangC_CreateNodeFrom(ctx, of, LangC_NodeKind_TypeArray);
	result->type = of;
	result->length = len;
	result->size = len * of->size;
	result->alignment_mask = of->alignment_mask;
	
	return result;
}

internal LangC_Node*
LangC_CreatePointerType(LangC_Context* ctx, LangC_Node* of)
{
	LangC_Node* result = LangC_CreateNodeFrom(ctx, of, LangC_NodeKind_TypePointer);
	result->type = of;
	result->size = ctx->abi->t_ptr.size;
	result->alignment_mask = ctx->abi->t_ptr.alignment_mask;
	
	return result;
}

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

internal inline bool32
LangC_IsSymbolType(LangC_Symbol* sym)
{
	return (sym->kind == LangC_SymbolKind_Typename ||
			sym->kind == LangC_SymbolKind_Struct ||
			sym->kind == LangC_SymbolKind_Union ||
			sym->kind == LangC_SymbolKind_Enum);
}

internal LangC_Symbol*
LangC_FindSymbol(LangC_Context* ctx, String name, LangC_SymbolKind specific)
{
	uint64 hash = SimpleHash(name);
	LangC_SymbolStack* stack = ctx->symbol_stack;
	
	while (stack)
	{
		LangC_Symbol* it = stack->symbols;
		
		for (; it; it = it->next)
		{
			if (it->name_hash == hash && (specific ? specific == it->kind : it->kind < LangC_SymbolKind__OwnNamespace))
				return it;
		}
		
		stack = stack->up;
	}
	
	return NULL;
}

internal LangC_Symbol*
LangC_FindSymbolOfIdent(LangC_Context* ctx, LangC_Node* ident, LangC_SymbolKind specific)
{
	if (!ident->symbol)
		ident->symbol = LangC_FindSymbol(ctx, ident->name, specific);
	
	return ident->symbol;
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
LangC_SymbolAlreadyDefinedInThisScope(LangC_Context* ctx, String name, LangC_SymbolKind specific)
{
	uint64 hash = SimpleHash(name);
	LangC_Symbol* it = ctx->symbol_stack->symbols;
	
	for (; it; it = it->next)
	{
		if (it->name_hash == hash && (specific ? specific == it->kind : it->kind < LangC_SymbolKind__OwnNamespace))
			return it;
	}
	
	return NULL;
}

internal LangC_Node*
LangC_TypeFromTypename(LangC_Context* ctx, LangC_Node* type)
{
	while (!LangC_IsSimpleType(type) &&
		   type->kind == LangC_NodeKind_TypeBaseTypename)
	{
		LangC_Symbol* sym = type->symbol;
		Assert(sym);
		
		type = sym->type;
	}
	
	return type;
}

internal LangC_Symbol*
LangC_UpdateSymbol(LangC_Symbol* sym, LangC_SymbolKind kind, LangC_Node* type, LangC_Node* decl)
{
	sym->kind = kind;
	sym->type = type;
	sym->decl = decl;
	
	return sym;
}

internal LangC_Symbol*
LangC_CreateSymbol(LangC_Context* ctx, String name, LangC_SymbolKind kind, LangC_Node* type, LangC_Node* decl)
{
	LangC_Symbol* result = Arena_Push(ctx->persistent_arena, sizeof *result);
	LangC_SymbolStack* stack = ctx->symbol_stack;
	
	result->next = stack->symbols;
	stack->symbols = result;
	
	result->kind = kind;
	result->name = name;
	result->type = type;
	result->name_hash = SimpleHash(name);
	result->decl = decl;
	
	return result;
}

internal const char*
LangC_CStringFromType(LangC_Context* ctx, LangC_Node* type)
{
	// TODO(ljre): this is for warning and error messages.
	
	return "";
}

internal const char*
LangC_CStringFromNodeKind(LangC_NodeKind kind)
{
	static const char* const table[LangC_NodeKind__CategoryCount][33] = {
		// TODO(ljre): Rest of the table.
		//             Search doesn't need to be that fast since this function should only
		//             be called when we are reporting warnings or errors.
		
		[(LangC_NodeKind_Expr2>>LangC_NodeKind__Category) - 1] = {
			"+", "-", "*", "/", "%", "<", ">", "<=", ">=", "==", "!=",
			"<<", ">>", "&", "|", "^", "&&", "||", "=", "+=", "-=", "*=",
			"/=", "<<=", ">>=", "&=", "|=", "^=", ",", "function call()",
			"indexing[]", ".", "->",
		},
	};
	
	uintsize cat = (kind>>LangC_NodeKind__Category) - 1;
	uintsize index = kind & ~LangC_NodeKind__CategoryMask;
	
	Assert(cat < ArrayLength(table));
	Assert(index < ArrayLength(table[cat]));
	
	return table[cat][index];
}

internal int32
LangC_NodeCount(LangC_Node* node)
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
LangC_RankOfType(LangC_Context* ctx, LangC_Node* type)
{
	Assert(ArrayLength(LangC_basic_types_table) >= ArrayLength(ctx->abi->t));
	
	for (int32 i = 0; i < ArrayLength(ctx->abi->t); ++i)
	{
		const LangC_Node* node = &LangC_basic_types_table[i];
		if (type->kind == node->kind && (type->flags & node->flags) == node->flags)
			return i;
	}
	
	return -1;
}

internal bool32
LangC_IsNumericType(LangC_Context* ctx, LangC_Node* type)
{
	// TODO(ljre): is this enough?
	return LangC_RankOfType(ctx, type) != -1;
}

internal bool32
LangC_IsIncompleteType(LangC_Context* ctx, LangC_Node* type)
{
	return type->size == 0;
}

internal bool32
LangC_IsVoidType(LangC_Context* ctx, LangC_Node* type)
{
	type = LangC_TypeFromTypename(ctx, type);
	
	return type->kind == LangC_NodeKind_TypeBaseVoid;
}

// NOTE(ljre): Compares two types.
//             Returns 0 if both are the same type, 1 if 'left' is "bigger" than 'right', -1 if
//             'right' is "bigger" than 'left', and -2 (aka LangC_INVALID) if both types are incompatible.
internal int32
LangC_CompareTypes(LangC_Context* ctx, LangC_Node* left, LangC_Node* right)
{
	left = LangC_TypeFromTypename(ctx, left);
	right = LangC_TypeFromTypename(ctx, right);
	
	// NOTE(ljre): Same definitions are from the same pointers.
	if (left == right)
		return 0;
	
	switch (left->kind)
	{
		case LangC_NodeKind_TypeBaseStruct:
		case LangC_NodeKind_TypeBaseUnion:
		{
			// NOTE(ljre): Just return invalid since they are not the same definition.
			return LangC_INVALID;
		} break;
		
		case LangC_NodeKind_TypeBaseVoid:
		{
			if (right->kind == LangC_NodeKind_TypeBaseVoid)
				return 0;
		} /* fallthrough */
		
		case LangC_NodeKind_TypeBaseChar:
		case LangC_NodeKind_TypeBaseInt:
		case LangC_NodeKind_TypeBaseFloat:
		case LangC_NodeKind_TypeBaseDouble:
		case LangC_NodeKind_TypeBaseBool:
		case LangC_NodeKind_TypeBaseEnum:
		{
			int32 left_rank = LangC_RankOfType(ctx, left);
			int32 right_rank = LangC_RankOfType(ctx, right);
			
			if (left_rank == -1 || right_rank == -1)
				return LangC_INVALID;
			
			return (left_rank > right_rank) ? 1 : (left_rank < right_rank) ? -1 : 0;
		} break;
		
		case LangC_NodeKind_TypePointer:
		{
			if (right->kind == LangC_NodeKind_TypePointer)
			{
				if (right->type->kind == LangC_NodeKind_TypeBaseVoid || left->type->kind == LangC_NodeKind_TypeBaseVoid)
					return 1;
				
				if (LangC_CompareTypes(ctx, left->type, right->type) == 0)
					return 0;
				
				return LangC_INVALID;
			}
			else if (LangC_IsBaseType(right->kind))
			{
				int32 right_rank = LangC_RankOfType(ctx, right);
				
				if (right_rank == -1)
					return LangC_INVALID;
				
				return 1;
			}
			else
			{
				return LangC_INVALID;
			}
		} break;
		
		case LangC_NodeKind_TypeFunction:
		{
			if (right->kind != LangC_NodeKind_TypeFunction)
				return LangC_INVALID;
			
			LangC_Node* lparam = left->params;
			LangC_Node* rparam = right->params;
			
			while (lparam && rparam)
			{
				if (LangC_CompareTypes(ctx, lparam->type, rparam->type) != 0)
					return LangC_INVALID;
				
				lparam = lparam->next;
				rparam = rparam->next;
			}
			
			if (lparam != rparam) // NOTE(ljre): If one of them isn't NULL
				return LangC_INVALID;
			
			return 0;
		} break;
		
		case LangC_NodeKind_TypeArray:
		{
			if (right->kind != LangC_NodeKind_TypeArray ||
				right->length != left->length ||
				LangC_CompareTypes(ctx, left->type, right->type) != 0)
				return LangC_INVALID;
			
			return 0;
		} break;
	}
	
	return LangC_INVALID;
}

internal bool32
LangC_IsExprLValue(LangC_Context* ctx, LangC_Node* expr)
{
	switch (expr->kind)
	{
		case LangC_NodeKind_Expr1Deref:
		case LangC_NodeKind_Expr2Assign:
		case LangC_NodeKind_Expr2AssignAdd:
		case LangC_NodeKind_Expr2AssignSub:
		case LangC_NodeKind_Expr2AssignMul:
		case LangC_NodeKind_Expr2AssignDiv:
		case LangC_NodeKind_Expr2AssignMod:
		case LangC_NodeKind_Expr2AssignLeftShift:
		case LangC_NodeKind_Expr2AssignRightShift:
		case LangC_NodeKind_Expr2AssignAnd:
		case LangC_NodeKind_Expr2AssignOr:
		case LangC_NodeKind_Expr2AssignXor:
		case LangC_NodeKind_Expr2Index:
		case LangC_NodeKind_ExprCompoundLiteral:
		case LangC_NodeKind_Expr2Access:
		case LangC_NodeKind_Expr2DerefAccess:
		{
			return true;
		}
		
		case LangC_NodeKind_ExprIdent:
		{
			LangC_Symbol* sym = LangC_FindSymbol(ctx, expr->name, 0);
			return !(sym->kind == LangC_SymbolKind_Function ||
					 sym->kind == LangC_SymbolKind_FunctionDecl ||
					 sym->kind == LangC_SymbolKind_FunctionDecl ||
					 sym->kind == LangC_SymbolKind_EnumConstant);
		}
	}
	
	return false;
}

internal void
LangC_TryToEval(LangC_Context* ctx, LangC_Node* expr)
{
	if (expr->cannot_be_evaluated_at_compile_time)
		return;
	
	expr->cannot_be_evaluated_at_compile_time = true;
	// TODO
}

internal void
LangC_ResolveType(LangC_Context* ctx, LangC_Node* type, bool32* out_is_complete, bool32 inlined_symbols)
{
	bool32 is_complete = false;
	
	LangC_SymbolKind kind = LangC_SymbolKind_Struct;
	LangC_ABIType* abitype = NULL;
	
	switch (type->kind)
	{
		case LangC_NodeKind_TypeBaseTypename:
		{
			type->symbol = LangC_FindSymbol(ctx, type->name, LangC_SymbolKind_Typename);
			is_complete = !LangC_IsIncompleteType(ctx, type->symbol->type);
		} break;
		
		case LangC_NodeKind_TypeBaseUnion: kind = LangC_SymbolKind_Union;
		case LangC_NodeKind_TypeBaseStruct:
		{
			if (type->name.size > 0)
			{
				LangC_Symbol* sym = LangC_FindSymbol(ctx, type->name, kind);
				
				if (sym)
				{
					type->symbol = sym;
					
					if (type->body)
						LangC_NodeError(ctx, type->body, "redefinition of '%s %.*s'.",
										kind == LangC_SymbolKind_Struct ? "struct" : "union",
										StrFmt(type->name));
					else
						is_complete = true;
				}
			}
			
			if (type->body)
			{
				is_complete = true;
				LangC_Symbol* sym;
				
				if (type->symbol)
				{
					sym = LangC_CreateSymbol(ctx, StrNull, kind, type, NULL);
				}
				else
				{
					sym = LangC_CreateSymbol(ctx, type->name, kind, type, NULL);
					type->symbol = sym;
				}
				
				if (inlined_symbols)
					sym->fields = ctx->symbol_stack;
				else
					sym->fields = LangC_PushSymbolStack(ctx);
				
				LangC_Node* needs_to_be_the_last = NULL;
				
				for (LangC_Node* member = type->body; member; member = member->next)
				{
					if (member->kind != LangC_NodeKind_Decl)
						continue;
					
					if (needs_to_be_the_last)
					{
						LangC_NodeError(ctx, needs_to_be_the_last, "incomplete flexible array needs to be at the end of struct");
					}
					
					bool32 c;
					LangC_ResolveType(ctx, member->type, &c, member->name.size == 0);
					
					LangC_Symbol* membersym = LangC_CreateSymbol(ctx, member->name, LangC_SymbolKind_Field, member->type, member);
					
					sym->size = AlignUp(sym->size, member->type->alignment_mask);
					
					if (!c)
					{
						if (member->type->kind == LangC_NodeKind_TypeArray)
							needs_to_be_the_last = member;
						else
							LangC_NodeError(ctx, member->type, "incomplete type not allowed in struct.");
					}
					else
					{
						membersym->size = member->type->size;
						membersym->offset = sym->size;
						
						if (kind == LangC_SymbolKind_Struct)
							sym->size += member->type->size;
						sym->alignment_mask = Max(sym->alignment_mask, member->type->alignment_mask);
					}
				}
				
				sym->size = AlignUp(sym->size, sym->alignment_mask);
				
				type->size = sym->size;
				type->alignment_mask = sym->alignment_mask;
				
				if (!inlined_symbols)
					LangC_PopSymbolStack(ctx);
			}
		} break;
		
		case LangC_NodeKind_TypeBaseVoid:
		{
			type->size = 0;
			type->alignment_mask = 0;
		} break;
		
		case LangC_NodeKind_TypeBaseChar:
		{
			if (type->flags & LangC_NodeFlags_Unsigned)
				abitype = &ctx->abi->t_uchar;
			else if (type->flags & LangC_NodeFlags_Signed)
				abitype = &ctx->abi->t_schar;
			else
				abitype = &ctx->abi->t_char;
		} goto set_abi_type;
		
		case LangC_NodeKind_TypeBaseInt:
		{
			if (type->flags & LangC_NodeFlags_LongLong)
				abitype = &ctx->abi->t_longlong;
			else if (type->flags & LangC_NodeFlags_Long)
				abitype = &ctx->abi->t_long;
			else if (type->flags & LangC_NodeFlags_Short)
				abitype = &ctx->abi->t_short;
			else
				abitype = &ctx->abi->t_int;
			
			if (type->flags & LangC_NodeFlags_Unsigned)
				abitype = abitype+1; // NOTE(ljre): 'abitype+1' is the same type, but unsigned.
		} goto set_abi_type;
		
		case LangC_NodeKind_TypeBaseFloat:  abitype = &ctx->abi->t_float;  goto set_abi_type;
		case LangC_NodeKind_TypeBaseDouble: abitype = &ctx->abi->t_double; goto set_abi_type;
		case LangC_NodeKind_TypeBaseBool:   abitype = &ctx->abi->t_bool;   goto set_abi_type;
		
		case LangC_NodeKind_TypePointer:
		{
			is_complete = true;
			
			type->size = ctx->abi->t_ptr.size;
			type->alignment_mask = ctx->abi->t_ptr.alignment_mask;
			LangC_ResolveType(ctx, type->type, NULL, false);
		} break;
		
		case LangC_NodeKind_TypeArray:
		{
			bool32 c;
			LangC_ResolveType(ctx, type->type, &c, false);
			
			if (!c)
			{
				LangC_NodeError(ctx, type->type, "cannot have array of incomplete type.");
			}
			else
			{
				type->alignment_mask = type->type->alignment_mask;
				type->length = 0;
				
				if (type->expr)
				{
					type->expr = LangC_ResolveExpr(ctx, type->expr);
					LangC_Node* casted = LangC_AddCastToExprIfNeeded(ctx, type->expr, LangC_SIZE_T);
					if (!casted)
					{
						LangC_NodeError(ctx, type->expr, "array length needs to be of numeric type.");
					}
					else
					{
						type->expr = casted;
						LangC_TryToEval(ctx, type->expr);
						
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
		
		case LangC_NodeKind_TypeFunction:
		{
			bool32 c;
			LangC_ResolveType(ctx, type->type, &c, false);
			
			if (!c)
			{
				if (LangC_IsVoidType(ctx, type->type))
				{
					type->type = NULL;
				}
				else
				{
					LangC_NodeError(ctx, type->type, "function cannot return incomplete type.");
				}
			}
			else
			{
				LangC_Node* param = type->params;
				LangC_ResolveType(ctx, param->type, &c, false);
				if (!c)
				{
					if (LangC_IsVoidType(ctx, param->type))
					{
						type->params = NULL; // NOTE(ljre): Function takes in no arguments.
						break;
					}
					
					LangC_NodeError(ctx, param->type, "function parameter cannot be of incomplete type.");
				}
				
				param = param->next;
				
				for (; param; param = param->next)
				{
					LangC_ResolveType(ctx, param->type, &c, false);
					
					if (!c)
						LangC_NodeError(ctx, param->type, "function parameter cannot be of incomplete type.");
				}
			}
		} break;
	}
	
	if (0) set_abi_type:
	{
		type->size = abitype->size;
		type->alignment_mask = abitype->alignment_mask;
		
		is_complete = true;
	}
	
	if (out_is_complete)
		*out_is_complete = is_complete;
}

// NOTE(ljre): Returns NULL if types are incompatible.
internal LangC_Node*
LangC_AddCastToExprIfNeeded(LangC_Context* ctx, LangC_Node* expr, LangC_Node* type)
{
	LangC_Node* result;
	
	int32 cmp = LangC_CompareTypes(ctx, type, expr->type);
	if (cmp == LangC_INVALID)
	{
		result = NULL;
	}
	else if (cmp != 0)
	{
		result = LangC_CreateNodeFrom(ctx, expr, LangC_NodeKind_Expr1Cast);
		result->type = type;
		result->expr = expr;
	}
	else
	{
		// NOTE(ljre): No cast needed
		result = expr;
	}
	
	return result;
}

internal LangC_Node*
LangC_DecayExpr(LangC_Context* ctx, LangC_Node* expr)
{
	LangC_Node* result = expr;
	
	if (expr->type->kind == LangC_NodeKind_TypeArray || expr->type->kind == LangC_NodeKind_TypeFunction)
	{
		result = LangC_CreateNodeFrom(ctx, expr, LangC_NodeKind_Expr1Ref);
		result->expr = expr;
		result->type = LangC_CreatePointerType(ctx,
											   (expr->type->kind == LangC_NodeKind_TypeArray) ? expr->type->type : expr->type);
	}
	
	return result;
}

internal LangC_Node*
LangC_PromoteToAtLeast(LangC_Context* ctx, LangC_Node* expr, LangC_Node* type)
{
	if (!expr)
		return NULL;
	
	LangC_Node* result;
	int32 cmp = LangC_CompareTypes(ctx, expr->type, type);
	
	if (cmp == LangC_INVALID)
	{
		result = NULL;
	}
	else if (cmp < 0)
	{
		result = LangC_CreateNodeFrom(ctx, expr, LangC_NodeKind_Expr1Cast);
		result->type = type;
		result->expr = expr;
	}
	else
	{
		result = expr;
	}
	
	return result;
}

internal LangC_Node*
LangC_ResolveExpr(LangC_Context* ctx, LangC_Node* expr)
{
	switch (expr->kind)
	{
		case LangC_NodeKind_Expr2Call:
		{
			expr->left = LangC_ResolveExpr(ctx, expr->left);
			LangC_Node* functype = expr->left->type;
			
			if (functype->kind == LangC_NodeKind_TypePointer)
				functype = functype->type;
			
			if (functype->kind != LangC_NodeKind_TypeFunction)
			{
				LangC_NodeError(ctx, expr->left, "trying to call non-function object.");
			}
			else
			{
				LangC_Node* funcargs = functype->params;
				
				for (LangC_Node** arg = &expr->right; *arg; arg = &(*arg)->next, funcargs = funcargs->next)
				{
					if (!funcargs)
					{
						LangC_NodeError(ctx, *arg, "excessive number of arguments being passed to function.");
						break;
					}
					
					*arg = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, *arg));
					
					LangC_Node* c = LangC_AddCastToExprIfNeeded(ctx, *arg, funcargs->type);
					if (!c)
					{
						LangC_NodeError(ctx, *arg, "cannot convert from type '%s' to '%s' when passing argument to function.",
										LangC_CStringFromType(ctx, (*arg)->type),
										LangC_CStringFromType(ctx, funcargs->type));
					}
					else
					{
						*arg = c;
					}
				}
				
				if (funcargs)
				{
					LangC_NodeError(ctx, expr, "missing %i arguments when calling function.", LangC_NodeCount(expr->right));
				}
				
				expr->type = functype->type;
			}
		} break;
		
		case LangC_NodeKind_Expr2Index:
		{
			expr->left = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->left));
			expr->right = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->right));
			
			if (!LangC_IsNumericType(ctx, expr->left->type))
			{
				LangC_NodeError(ctx, expr->left, "expected numeric type when indexing.");
			}
			else if (!LangC_IsNumericType(ctx, expr->right->type))
			{
				LangC_NodeError(ctx, expr->right, "expected numeric type when indexing.");
			}
			else
			{
				int32 cmp = LangC_CompareTypes(ctx, expr->left->type, expr->right->type);
				
				// NOTE(ljre): Swap, make sure pointer is 'expr->left'.
				if (cmp <= 0)
				{
					LangC_Node* temp = expr->left;
					expr->left = expr->right;
					expr->right = temp;
				}
				
				if (expr->left->type->kind != LangC_NodeKind_TypePointer)
				{
					LangC_NodeError(ctx, expr, "expected a pointer type when indexing.");
				}
				else if (expr->right->type->kind == LangC_NodeKind_TypePointer)
				{
					// TODO(ljre): maybe void* + T*?
					LangC_NodeError(ctx, expr, "expected a numeric type when indexing.");
				}
				else if (LangC_IsIncompleteType(ctx, expr->left->type->type))
				{
					LangC_NodeError(ctx, expr, "cannot deref pointer to incomplete type.");
				}
				else
				{
					expr->type = expr->left->type->type;
				}
			}
		} break;
		
		case LangC_NodeKind_Expr1Cast:
		{
			expr->expr = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->expr));
			int32 cmp = LangC_CompareTypes(ctx, expr->expr->type, expr->type);
			
			if (cmp == LangC_INVALID)
			{
				LangC_NodeError(ctx, expr, "cannot convert from type '%s' to '%s'.",
								LangC_CStringFromType(ctx, expr->expr->type),
								LangC_CStringFromType(ctx, expr->type));
			}
		} break;
		
		case LangC_NodeKind_Expr3Condition:
		{
			expr->left = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->left));
			expr->middle = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->middle));
			expr->right = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->right));
			
			LangC_Node* c = LangC_PromoteToAtLeast(ctx, expr->left, LangC_INT);
			if (!c)
			{
				LangC_NodeError(ctx, expr->left, "condition is not of numeric type.");
			}
			
			int32 cmp = LangC_CompareTypes(ctx, expr->middle->type, expr->right->type);
			if (cmp == LangC_INVALID)
			{
				LangC_NodeError(ctx, expr, "both sides of ternary operator are incompatible.");
			}
			else if (cmp < 0)
			{
				expr->type = expr->middle->type;
			}
			else // if (cmp > 0 || cmp == 0)
			{
				expr->type = expr->right->type;
			}
		} break;
		
		case LangC_NodeKind_ExprCompoundLiteral:
		{
			// TODO(ljre)
		} break;
		
		case LangC_NodeKind_ExprInitializer:
		{
			// TODO(ljre)
		} break;
		
		case LangC_NodeKind_Expr2Comma:
		{
			expr->left = LangC_ResolveExpr(ctx, expr->left);
			expr->right = LangC_ResolveExpr(ctx, expr->right);
			
			expr->type = expr->right->type;
		} break;
		
		case LangC_NodeKind_Expr2Access:
		{
			// TODO(ljre)
		} break;
		
		case LangC_NodeKind_Expr2DerefAccess:
		{
			// TODO(ljre)
		} break;
		
		case LangC_NodeKind_Expr2LThan:
		case LangC_NodeKind_Expr2GThan:
		case LangC_NodeKind_Expr2LEqual:
		case LangC_NodeKind_Expr2GEqual:
		case LangC_NodeKind_Expr2Equals:
		case LangC_NodeKind_Expr2NotEquals:
		case LangC_NodeKind_Expr2LogicalAnd:
		case LangC_NodeKind_Expr2LogicalOr:
		{
			expr->left = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->left));
			expr->right = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->right));
			
			expr->type = LangC_INT;
		} break;
		
		{
			bool32 needs_to_be_numeric;
			
			case LangC_NodeKind_Expr2AssignAdd:
			case LangC_NodeKind_Expr2AssignSub:
			case LangC_NodeKind_Expr2AssignMul:
			case LangC_NodeKind_Expr2AssignDiv:
			case LangC_NodeKind_Expr2AssignMod:
			case LangC_NodeKind_Expr2AssignLeftShift:
			case LangC_NodeKind_Expr2AssignRightShift:
			case LangC_NodeKind_Expr2AssignAnd:
			case LangC_NodeKind_Expr2AssignOr:
			case LangC_NodeKind_Expr2AssignXor: needs_to_be_numeric = true;
			if (0) case LangC_NodeKind_Expr2Assign: needs_to_be_numeric = false;
			
			expr->left = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->left));
			
			expr->right = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->right));
			expr->right = LangC_PromoteToAtLeast(ctx, expr->right, LangC_INT);
			
			if (!LangC_IsExprLValue(ctx, expr->left))
			{
				LangC_NodeError(ctx, expr->left, "left-side of assignment needs to be a lvalue.");
			}
			
			int32 cmp = LangC_CompareTypes(ctx, expr->left->type, expr->right->type);
			if (cmp == LangC_INVALID)
			{
				LangC_NodeError(ctx, expr, "invalid operation '%s' with operands of type '%s' and '%s'.",
								LangC_CStringFromNodeKind(expr->kind),
								LangC_CStringFromType(ctx, expr->left->type),
								LangC_CStringFromType(ctx, expr->right->type));
			}
			else if (cmp < 0)
			{
				expr->type = expr->right->type;
			}
			else
			{
				expr->type = expr->left->type;
			}
		} break;
		
		case LangC_NodeKind_Expr2Add:
		case LangC_NodeKind_Expr2Sub:
		case LangC_NodeKind_Expr2Mul:
		case LangC_NodeKind_Expr2Div:
		case LangC_NodeKind_Expr2Mod:
		case LangC_NodeKind_Expr2And:
		case LangC_NodeKind_Expr2Or:
		case LangC_NodeKind_Expr2Xor:
		case LangC_NodeKind_Expr2LeftShift:
		case LangC_NodeKind_Expr2RightShift:
		{
			expr->left = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->left));
			expr->left = LangC_PromoteToAtLeast(ctx, expr->left, LangC_INT);
			
			expr->right = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->right));
			expr->right = LangC_PromoteToAtLeast(ctx, expr->right, LangC_INT);
			
			int32 cmp = LangC_CompareTypes(ctx, expr->left->type, expr->right->type);
			if (cmp == LangC_INVALID)
			{
				LangC_NodeError(ctx, expr, "invalid operation '%s' with operands of type '%s' and '%s'.",
								LangC_CStringFromNodeKind(expr->kind),
								LangC_CStringFromType(ctx, expr->left->type),
								LangC_CStringFromType(ctx, expr->right->type));
			}
			else if (cmp < 0)
			{
				expr->type = expr->right->type;
			}
			else
			{
				expr->type = expr->left->type;
			}
		} break;
		
		case LangC_NodeKind_Expr1Ref:
		{
			expr->expr = LangC_ResolveExpr(ctx, expr->expr);
			expr->type = LangC_CreatePointerType(ctx, expr->expr->type);
			
			if (0)
			{
				case LangC_NodeKind_Expr1PrefixInc:
				case LangC_NodeKind_Expr1PrefixDec:
				case LangC_NodeKind_Expr1PostfixInc:
				case LangC_NodeKind_Expr1PostfixDec:
				
				expr->type = expr->expr->type;
				
				// TODO(ljre): Check if this is needed.
				//LangC_PromoteToAtLeast(expr, LangC_INT);
				
				if (!LangC_IsNumericType(ctx, expr))
				{
					LangC_NodeError(ctx, expr, "++ and -- operators expects operand of numeric value.");
				}
			}
			
			if (!LangC_IsExprLValue(ctx, expr->expr))
			{
				LangC_NodeError(ctx, expr->expr, "operand is not a lvalue.");
			}
		} break;
		
		case LangC_NodeKind_Expr1Sizeof:
		{
			expr->expr = LangC_ResolveExpr(ctx, expr->expr);
			expr->type = LangC_SIZE_T;
			
			LangC_Node* type = expr->expr->type;
			if (0) case LangC_NodeKind_Expr1SizeofType:
			{
				type = expr->type;
			}
			
			if (type->kind == LangC_NodeKind_TypeFunction)
			{
				LangC_NodeError(ctx, expr, "cannot take sizeof of function.");
			}
			else if (LangC_IsIncompleteType(ctx, expr->type))
			{
				LangC_NodeError(ctx, expr, "cannot take sizeof of incomplete type.");
			}
			else
			{
				expr->value_uint = expr->type->size;
			}
		} break;
		
		case LangC_NodeKind_Expr1Deref:
		{
			expr->expr = LangC_ResolveExpr(ctx, expr->expr);
			
			if (expr->type->kind != LangC_NodeKind_TypePointer)
			{
				LangC_NodeError(ctx, expr, "cannot dereference a non-pointer object.");
			}
			else if (LangC_IsIncompleteType(ctx, expr->type))
			{
				LangC_NodeError(ctx, expr, "cannot dereference pointer to incomplete object.");
			}
			else
			{
				expr->type = expr->expr->type->type;
			}
		} break;
		
		case LangC_NodeKind_Expr1Plus:
		case LangC_NodeKind_Expr1Negative:
		case LangC_NodeKind_Expr1Not:
		case LangC_NodeKind_Expr1LogicalNot:
		{
			expr->expr = LangC_ResolveExpr(ctx, expr->expr);
			expr->type = LangC_PromoteToAtLeast(ctx, expr->expr->type, LangC_INT);
		} break;
		
		case LangC_NodeKind_ExprIdent:
		{
			LangC_Symbol* sym = LangC_FindSymbolOfIdent(ctx, expr, 0);
			
			if (!sym)
			{
				LangC_NodeError(ctx, expr, "unknown identifier '%*s'.", StrFmt(expr->name));
			}
			else
			{
				expr->type = sym->type;
			}
		} break;
		
		case LangC_NodeKind_ExprInt: expr->type = LangC_INT; break;
		case LangC_NodeKind_ExprLInt: expr->type = LangC_LINT; break;
		case LangC_NodeKind_ExprLLInt: expr->type = LangC_LLINT; break;
		case LangC_NodeKind_ExprUInt: expr->type = LangC_UINT; break;
		case LangC_NodeKind_ExprULInt: expr->type = LangC_LUINT; break;
		case LangC_NodeKind_ExprULLInt: expr->type = LangC_LLUINT; break;
		case LangC_NodeKind_ExprFloat: expr->type = LangC_FLOAT; break;
		case LangC_NodeKind_ExprDouble: expr->type = LangC_DOUBLE; break;
		case LangC_NodeKind_ExprString:
		case LangC_NodeKind_ExprWideString:
		{
			expr->type = LangC_CreateArrayType(ctx,
											   (expr->kind == LangC_NodeKind_ExprWideString) ? LangC_USHORT : LangC_CHAR,
											   expr->value_str.size + 1); // NOTE(ljre): null-terminator
		} break;
		
		default: Unreachable(); break;
	}
	
	return expr;
}

internal LangC_Node*
LangC_ResolveExprOfType(LangC_Context* ctx, LangC_Node* expr, LangC_Node* type)
{
	expr = LangC_ResolveExpr(ctx, expr);
	expr = LangC_DecayExpr(ctx, expr);
	LangC_Node* result = LangC_AddCastToExprIfNeeded(ctx, expr, type);
	if (!result)
	{
		result = expr;
		LangC_NodeError(ctx, expr, "expression of type '%s' is incompatible with '%s'.",
						LangC_CStringFromType(ctx, expr->type),
						LangC_CStringFromType(ctx, type));
	}
	
	return result;
}

internal void
LangC_ResolveDecl(LangC_Context* ctx, LangC_Node* decl)
{
	LangC_SymbolKind sym_kind = LangC_SymbolKind_Var;
	bool32 is_complete;
	LangC_ResolveType(ctx, decl->type, &is_complete, false);
	
	// TODO(ljre): Refactor this function -- remove unecessary LangC_FindSymbol-s
	
	LangC_Node* type = LangC_TypeFromTypename(ctx, decl->type);
	
	if (type->flags & LangC_NodeFlags_Poisoned)
	{
		decl->flags |= LangC_NodeFlags_Poisoned;
	}
	else switch (type->kind)
	{
		case LangC_NodeKind_TypeArray:
		{
			if (decl->type->expr)
			{
				if (decl->type->expr->cannot_be_evaluated_at_compile_time)
				{
					if (decl->expr)
						LangC_NodeError(ctx, decl->type, "cannot initialize variable-length array.");
					
					break;
				}
			}
			else if (!decl->expr)
			{
				LangC_NodeError(ctx, decl, "array declaration with implicit size needs initializer.");
			}
			
			if (decl->expr)
			{
				decl->expr = LangC_ResolveExpr(ctx, decl->expr);
				
				if (decl->expr->kind != LangC_NodeKind_ExprInitializer &&
					decl->expr->kind != LangC_NodeKind_ExprString)
				{
					LangC_NodeError(ctx, decl->expr, "this is not a valid array initializer.");
				}
				else if (decl->type->length == 0)
				{
					uint64 length = decl->expr->length;
					
					decl->type->length = length;
					decl->type->size = decl->type->type->size * length;
				}
				else if (decl->expr->length > decl->type->length)
				{
					LangC_NodeError(ctx, decl->expr, "excessive number of items in array initializer");
				}
			}
		} break;
		
		case LangC_NodeKind_TypeFunction:
		{
			// TODO(ljre)
			
			sym_kind = LangC_SymbolKind_FunctionDecl;
		} break;
		
		case LangC_NodeKind_TypeBaseChar:
		case LangC_NodeKind_TypeBaseInt:
		case LangC_NodeKind_TypeBaseFloat:
		case LangC_NodeKind_TypeBaseDouble:
		case LangC_NodeKind_TypeBaseVoid:
		case LangC_NodeKind_TypeBaseBool:
		case LangC_NodeKind_TypeBaseTypename:
		case LangC_NodeKind_TypeBaseStruct:
		case LangC_NodeKind_TypeBaseUnion:
		case LangC_NodeKind_TypeBaseEnum:
		{
			if (decl->expr && decl->expr->kind == LangC_NodeKind_ExprInitializer)
			{
				LangC_Node* resolved;
				bool32 is_struct = false;
				LangC_Symbol* sym = NULL;
				
				if (decl->type->kind == LangC_NodeKind_TypeBaseStruct)
				{
					sym = LangC_FindSymbol(ctx, decl->type->name, LangC_SymbolKind_Struct);
					is_struct = true;
					
					if (!sym)
					{
						LangC_NodeError(ctx, decl->type, "'struct %.*s' is an incomplete type.", StrFmt(decl->type->name));
					}
					else
					{
						resolved = sym->type;
					}
				}
				else if (decl->type->kind == LangC_NodeKind_TypeBaseTypename)
				{
					sym = LangC_FindSymbol(ctx, decl->type->name, LangC_SymbolKind_Typename);
					resolved = LangC_TypeFromTypename(ctx, sym->type);
					
					if (LangC_IsStructType(resolved->type))
					{
						is_struct = true;
					}
				}
				
				if (sym)
				{
					decl->symbol = sym;
				}
				
				if (is_struct)
				{
					decl->expr = LangC_ResolveExprOfType(ctx, decl->expr, resolved);
					break;
				}
			}
		} break;
		
		case LangC_NodeKind_TypePointer:
		default:
		{
			if (decl->expr)
			{
				decl->expr = LangC_ResolveExpr(ctx, decl->expr);
				
				LangC_Node* newone = LangC_AddCastToExprIfNeeded(ctx, decl->expr, decl->type);
				if (!newone)
				{
					LangC_NodeError(ctx, decl->expr, "cannot assign value of type '%s' to '%s'.",
									LangC_CStringFromType(ctx, decl->expr->type),
									LangC_CStringFromType(ctx, decl->type));
				}
				else
				{
					decl->expr = newone;
				}
			}
		} break;
	}
	
	if (decl->kind == LangC_NodeKind_DeclTypedef)
		sym_kind = LangC_SymbolKind_Typename;
	
	if (decl->name.size > 0 && !decl->symbol)
	{
		LangC_Symbol* sym = LangC_CreateSymbol(ctx, decl->name, sym_kind, decl->type, decl);
		decl->symbol = sym;
	}
}

internal void
LangC_ResolveStmt(LangC_Context* ctx, LangC_Node* stmt)
{
	LangC_Node* cast;
	
	switch (stmt->kind)
	{
		case LangC_NodeKind_StmtEmpty: break;
		
		case LangC_NodeKind_StmtExpr:
		case LangC_NodeKind_StmtReturn:
		{
			stmt->expr = LangC_ResolveExpr(ctx, stmt->expr);
		} break;
		
		case LangC_NodeKind_StmtFor:
		{
			if (stmt->init)
			{
				if (stmt->init->kind == LangC_NodeKind_Decl)
					LangC_ResolveDecl(ctx, stmt->init);
				else
					stmt->init = LangC_ResolveExpr(ctx, stmt->init);
			}
			
			if (stmt->iter)
				stmt->iter = LangC_ResolveExpr(ctx, stmt->iter);
		} /* fallthrough */
		
		case LangC_NodeKind_StmtSwitch:
		case LangC_NodeKind_StmtDoWhile:
		case LangC_NodeKind_StmtWhile:
		case LangC_NodeKind_StmtIf:
		{
			// NOTE(ljre): If 'stmt->expr' is null, then the code gen shall generate an 1 constant
			if (stmt->expr)
				stmt->expr = LangC_ResolveExpr(ctx, stmt->expr);
			
			cast = LangC_PromoteToAtLeast(ctx, stmt->expr, LangC_INT);
			if (!cast)
			{
				LangC_NodeError(ctx, stmt->expr, (stmt->kind == LangC_NodeKind_StmtSwitch) ?
								"switch expression should be of numeric type." :
								"condition should be of numeric type.");
			}
			else
			{
				stmt->expr = cast;
			}
			
			LangC_ResolveStmt(ctx, stmt->stmt);
			if (stmt->stmt2)
				LangC_ResolveStmt(ctx, stmt->stmt2);
		} break;
		
		case LangC_NodeKind_StmtLabel:
		{
			LangC_ResolveStmt(ctx, stmt->stmt);
			
			// TODO(ljre): Add to the "labels table"
		} break;
		
		case LangC_NodeKind_StmtCase:
		{
			stmt->expr = LangC_ResolveExpr(ctx, stmt->expr);
			
			cast = LangC_PromoteToAtLeast(ctx, stmt->expr, LangC_INT);
			if (!cast)
			{
				LangC_NodeError(ctx, stmt->expr, "case expression should be of integer type.");
			}
			else
			{
				stmt->expr = cast;
			}
			
			LangC_ResolveStmt(ctx, stmt->stmt);
			
			// TODO(ljre): Find the host switch statement
		} break;
		
		case LangC_NodeKind_StmtCompound:
		{
			LangC_ResolveBlock(ctx, stmt->stmt);
		} break;
		
		default: Unreachable(); break;
	}
}

internal LangC_SymbolStack*
LangC_ResolveBlock(LangC_Context* ctx, LangC_Node* block)
{
	Assert(block->kind == LangC_NodeKind_StmtCompound);
	
	LangC_SymbolStack* result = LangC_PushSymbolStack(ctx);
	
	for (LangC_Node* stmt = block->stmt; stmt; stmt = stmt->next)
	{
		if ((stmt->kind & LangC_NodeKind__CategoryMask) == LangC_NodeKind_Decl)
			LangC_ResolveDecl(ctx, stmt);
		else
			LangC_ResolveStmt(ctx, stmt);
	}
	
	LangC_PopSymbolStack(ctx);
	return result;
}

internal uintsize
LangC_CalculateFunctionStackSize(LangC_Context* ctx, LangC_SymbolStack* scope)
{
	// TODO(ljre):
	return 0;
}

internal void
LangC_ResolveGlobalDecl(LangC_Context* ctx, LangC_Node* decl)
{
	LangC_ResolveType(ctx, decl->type, NULL, false);
	
	// TODO(ljre): Merging of multiple declarations specifics and attributes.
	switch (decl->kind)
	{
		case LangC_NodeKind_Decl:
		{
			if (decl->type->kind == LangC_NodeKind_TypeFunction)
			{
				decl->kind = LangC_NodeKind_DeclExtern;
				func_decl:;
				
				if (decl->name.size == 0)
				{
					LangC_NodeError(ctx, decl, "functions without a name are not allowed.");
					break;
				}
				
				LangC_Symbol* old_decl = LangC_FindSymbol(ctx, decl->name, 0);
				LangC_Symbol* sym;
				
				if (old_decl)
				{
					if (0 != LangC_CompareTypes(ctx, decl->type, old_decl->type))
					{
						LangC_NodeError(ctx, decl, "type of previous function declaration is incompatible.");
					}
					else if (decl->kind != old_decl->decl->kind ||
							 (decl->flags & LangC_NodeFlags_Inline) != (old_decl->decl->flags & LangC_NodeFlags_Inline))
					{
						LangC_NodeError(ctx, decl, "linking mismatch with previous declaration of function.");
					}
					
					sym = old_decl;
					LangC_UpdateSymbol(sym, LangC_SymbolKind_FunctionDecl, decl->type, decl);
				}
				else
				{
					sym = LangC_CreateSymbol(ctx, decl->name, LangC_SymbolKind_FunctionDecl, decl->type, decl);
				}
				
				if (decl->body)
				{
					sym->kind = LangC_SymbolKind_Function;
					sym->locals = LangC_ResolveBlock(ctx, decl->body);
					sym->stack_needed = LangC_CalculateFunctionStackSize(ctx, sym->locals);
				}
			}
			else
			{
				LangC_SymbolKind kind = LangC_SymbolKind_Var;
				LangC_Symbol* old_sym = LangC_FindSymbol(ctx, decl->name, 0);
				LangC_Symbol* sym;
				
				if (old_sym)
				{
					// TODO(ljre): Array semantics
					if (0 != LangC_CompareTypes(ctx, old_sym->type, decl->type))
					{
						LangC_NodeError(ctx, decl, "type of previous function declaration is incompatible.");
					}
					
					sym = old_sym;
					sym->kind = kind;
				}
				else
				{
					sym = LangC_CreateSymbol(ctx, decl->name, kind, decl->type, decl);
				}
			}
		} break;
		
		case LangC_NodeKind_DeclStatic:
		{
			if (decl->type->kind == LangC_NodeKind_TypeFunction)
				goto func_decl;
			
			LangC_SymbolKind kind = LangC_SymbolKind_StaticVar;
			LangC_Symbol* old_sym = LangC_FindSymbol(ctx, decl->name, 0);
			LangC_Symbol* sym;
			
			if (old_sym)
			{
				// TODO(ljre): Array semantics
				if (0 != LangC_CompareTypes(ctx, old_sym->type, decl->type))
				{
					LangC_NodeError(ctx, decl, "type of previous function declaration is incompatible.");
				}
				
				if (old_sym->decl->kind != LangC_NodeKind_DeclStatic)
				{
					LangC_NodeError(ctx, decl, "linking mismatch with previous declaration of function.");
				}
				
				sym = old_sym;
			}
			else
			{
				sym = LangC_CreateSymbol(ctx, decl->name, kind, decl->type, decl);
			}
		} break;
		
		case LangC_NodeKind_DeclExtern:
		{
			// NOTE(ljre): 'extern' keywords makes no difference for functions... Unless it's inline.
			if (decl->type->kind == LangC_NodeKind_TypeFunction)
			{
				if (decl->flags & LangC_NodeFlags_Inline)
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
				LangC_SymbolKind kind = LangC_SymbolKind_VarDecl;
				LangC_Symbol* old_sym = LangC_FindSymbol(ctx, decl->name, 0);
				LangC_Symbol* sym;
				
				if (old_sym)
				{
					// TODO(ljre): Array semantics
					if (0 != LangC_CompareTypes(ctx, old_sym->type, decl->type))
					{
						LangC_NodeError(ctx, decl, "type of previous function declaration is incompatible.");
					}
					
					sym = old_sym;
				}
				else
				{
					sym = LangC_CreateSymbol(ctx, decl->name, kind, decl->type, decl);
				}
			}
		} break;
		
		case LangC_NodeKind_DeclAuto:
		{
			LangC_NodeError(ctx, decl, "cannot use 'auto' in global scope.");
		} break;
		
		case LangC_NodeKind_DeclTypedef:
		{
			if (LangC_SymbolAlreadyDefinedInThisScope(ctx, decl->name, LangC_SymbolKind_Typename))
				LangC_NodeError(ctx, decl, "redefinition of type '%.*s'.", StrFmt(decl->name));
			
			if (decl->name.size > 0)
				LangC_CreateSymbol(ctx, decl->name, LangC_SymbolKind_Typename, decl->type, decl);
		} break;
		
		case LangC_NodeKind_DeclRegister:
		{
			LangC_NodeError(ctx, decl, "cannot use 'register' in global scope.");
		} break;
		
		default: Unreachable(); break;
	}
}

internal void
LangC_FinishSymbolsResolution(LangC_Context* ctx, LangC_SymbolStack* scope)
{
	// TODO(ljre): Make arrays definitions with no specified length have length of 1.
	// TODO(ljre): Make 'static' global variables symbols.
}

internal bool32
LangC_ResolveAst(LangC_Context* ctx)
{
	Trace();
	
	LangC_PushSymbolStack(ctx);
	
	LangC_Node* global_decl = ctx->ast;
	while (global_decl)
	{
		LangC_ResolveGlobalDecl(ctx, global_decl);
		global_decl = global_decl->next;
	}
	
	// NOTE(ljre): Don't pop the global stack, we want the global symbols.
	//LangC_PopSymbolStack(ctx);
	
	LangC_FinishSymbolsResolution(ctx, ctx->symbol_stack);
	
	return LangC_error_count == 0;
}
