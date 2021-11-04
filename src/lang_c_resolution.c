// NOTE(ljre): DO NOT MODIFY THESE OBJECTS. ACT AS IF THERE WAS CONST HERE.
internal /* const */ LangC_Node LangC_basic_types_table[] = {
	{ .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_Char, },
	{ .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_Char | LangC_Node_BaseType_Signed, },
	{ .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_Char | LangC_Node_BaseType_Unsigned, },
	{ .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_Short, },
	{ .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_Short | LangC_Node_BaseType_Unsigned, },
	{ .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_Int, },
	{ .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_Int | LangC_Node_BaseType_Unsigned, },
	{ .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_Long, },
	{ .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_Long | LangC_Node_BaseType_Unsigned, },
	{ .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_LongLong, },
	{ .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_LongLong | LangC_Node_BaseType_Unsigned, },
	{ .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_Float, },
	{ .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_Double, },
	{ .kind = LangC_NodeKind_PointerType, .type = &(LangC_Node) { .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_Void, }, },
	{ .kind = LangC_NodeKind_BaseType, .flags = LangC_Node_BaseType_Void, },
};

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
#define LangC_VOID (&LangC_basic_types_table[14])
#define LangC_SIZE_T (&LangC_basic_types_table[ctx->abi->index_sizet])
#define LangC_PTRDIFF_T (&LangC_basic_types_table[ctx->abi->index_ptrdifft])
#define LangC_IsSimpleType(node) ((node) && (node) >= LangC_CHAR && (node) <= LangC_VOID)
#define LangC_IsStructType(node) ((node) && (node)->kind == LangC_NodeKind_BaseType && ((node)->flags & LangC_Node_BaseType_Struct))
#define LangC_IsUnionType(node) ((node) && (node)->kind == LangC_NodeKind_BaseType && ((node)->flags & LangC_Node_BaseType_Union))

internal LangC_SymbolStack* LangC_ResolveBlock(LangC_Context* ctx, LangC_Node* block);

internal LangC_Node*
LangC_CreateNodeFrom(LangC_Context* ctx, LangC_Node* other, LangC_NodeKind kind)
{
	LangC_Node* result = Arena_Push(ctx->persistent_arena, sizeof *result);
	
	result->kind = kind;
	result->flags |= LangC_Node_Implicit;
	result->line = other->line;
	result->col = other->col;
	result->lexfile = other->lexfile;
	result->leading_spaces = Str("");
	
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
			if (it->name_hash == hash && ((!specific && !LangC_IsSymbolType(it)) || specific == it->kind))
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
		if (it->name_hash == hash && (!specific || specific == it->kind))
			return it;
	}
	
	return NULL;
}

internal LangC_Node*
LangC_ResolveType(LangC_Context* ctx, LangC_Node* type)
{
	while (!LangC_IsSimpleType(type) &&
		   type->kind == LangC_NodeKind_BaseType &&
		   (type->flags & LangC_Node_LowerBits) == LangC_Node_BaseType_Typename)
	{
		LangC_Symbol* sym = LangC_FindSymbol(ctx, type->name, LangC_SymbolKind_Typename);
		Assert(sym);
		
		type = sym->type;
	}
	
	return type;
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
	result->type = LangC_ResolveType(ctx, type);
	result->name_hash = SimpleHash(name);
	
	return result;
}

internal const char*
LangC_CStringFromType(LangC_Context* ctx, LangC_Node* type)
{
	// TODO(ljre): this is for warning and error messages.
	
	return "";
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
LangC_RankOfType(LangC_Context* ctx, uint64 flags)
{
	Assert(ArrayLength(LangC_basic_types_table) >= ArrayLength(ctx->abi->t));
	
	for (int32 i = 0; i < ArrayLength(ctx->abi->t); ++i)
	{
		const LangC_Node* node = &LangC_basic_types_table[i];
		if ((node->flags & flags) == node->flags)
			return i;
	}
	
	return -1;
}

internal LangC_ABIType*
LangC_GetABIType(LangC_Context* ctx, uint64 flags)
{
	int32 rank = LangC_RankOfType(ctx, flags);
	if (rank == -1)
		return NULL;
	
	return &ctx->abi->t[rank];
}

internal bool32
LangC_IsNumericType(LangC_Context* ctx, LangC_Node* type)
{
	// TODO(ljre): is this enough?
	return LangC_RankOfType(ctx, type->flags) != -1;
}

internal bool32
LangC_TypeInfo(LangC_Context* ctx, const LangC_Node* type, uint64* out_size, uint64* out_alignment_mask)
{
	beginning:;
	
	switch (type->kind)
	{
		case LangC_NodeKind_BaseType:
		{
			LangC_SymbolKind k = LangC_SymbolKind_Struct;
			
			if (type->flags & LangC_Node_BaseType_Bool)
			{
				*out_size = 1;
				*out_alignment_mask = 0;
				return true;
			}
			else if (type->flags & LangC_Node_BaseType_Enum)
			{
				*out_size = ctx->abi->t_int.size;
				*out_alignment_mask = ctx->abi->t_int.alignment_mask;
				return true;
			}
			else if (type->flags & LangC_Node_BaseType_Typename)
			{
				type = LangC_ResolveType(ctx, (LangC_Node*)type);
				if (!type)
					return false;
				
				goto beginning;
			}
			else if (type->flags & LangC_Node_BaseType_Struct ||
					 type->flags & LangC_Node_BaseType_Union && (k = LangC_SymbolKind_Union))
			{
				LangC_Symbol* sym = LangC_FindSymbol(ctx, type->name, k);
				if (!type)
					return false;
				
				type = sym->type;
				*out_size = type->size;
				*out_alignment_mask = type->alignment_mask;
				return true;
			}
			else if (type->flags & LangC_Node_BaseType_Void)
			{
				return false;
			}
			
			LangC_ABIType* atype = LangC_GetABIType(ctx, type->flags);
			if (atype)
			{
				*out_size = atype->size;
				*out_alignment_mask = atype->alignment_mask;
				return true;
			}
			
			Unreachable();
		} break;
		
		case LangC_NodeKind_ArrayType:
		{
			if (type->length > 0 && LangC_TypeInfo(ctx, type->type, out_size, out_alignment_mask))
			{
				*out_size *= type->length;
				return true;
			}
			else
			{
				return false;
			}
		} break;
		
		case LangC_NodeKind_FunctionType:
		{
			return false;
		} break;
		
		case LangC_NodeKind_PointerType:
		{
			*out_size = ctx->abi->t_ptr.size;
			*out_alignment_mask = ctx->abi->t_ptr.alignment_mask;
			return true;
		} break;
	}
	
	return false;
}

internal bool32
LangC_IsIncompleteType(LangC_Context* ctx, LangC_Node* type)
{
	type = LangC_ResolveType(ctx, type);
	
	if (type->kind == LangC_NodeKind_BaseType)
	{
		if (type->flags & LangC_Node_BaseType_Void)
			return true;
		
		if (type->flags & LangC_Node_BaseType_Struct)
			return LangC_FindSymbol(ctx, type->name, LangC_SymbolKind_Struct) != NULL;
		
		if (type->flags & LangC_Node_BaseType_Union)
			return LangC_FindSymbol(ctx, type->name, LangC_SymbolKind_Union) != NULL;
	}
	else if (type->kind == LangC_NodeKind_FunctionType)
		return true;
	
	return false;
}

// NOTE(ljre): Compares two types.
//             Returns 0 if both are the same type, 1 if 'left' is "bigger" than 'right', -1 if
//             'right' is "bigger" than 'left', and -2 (aka LangC_INVALID) if both types are incompatible.
internal int32
LangC_CompareTypes(LangC_Context* ctx, LangC_Node* left, LangC_Node* right)
{
	left = LangC_ResolveType(ctx, left);
	right = LangC_ResolveType(ctx, right);
	
	if (left == right)
		return 0;
	
	switch (left->kind)
	{
		case LangC_NodeKind_BaseType:
		{
			if (left->flags & LangC_Node_BaseType_Void && right->flags & LangC_Node_BaseType_Void)
				return 0;
			
			// NOTE(ljre): Just return invalid since they are not the same definition.
			if (left->flags & LangC_Node_BaseType_Struct ||
				left->flags & LangC_Node_BaseType_Union)
				return LangC_INVALID;
			
			int32 left_rank = LangC_RankOfType(ctx, left->flags);
			int32 right_rank = LangC_RankOfType(ctx, left->flags);
			
			if (left_rank == -1 || right_rank == -1)
				return LangC_INVALID;
			
			return (left_rank > right_rank) - (left_rank < right_rank);
		} break;
		
		case LangC_NodeKind_PointerType:
		{
			if (right->kind == LangC_NodeKind_PointerType)
			{
				if ((right->type->kind == LangC_NodeKind_BaseType &&
					 right->type->flags & LangC_Node_BaseType_Void) ||
					(left->type->flags & LangC_Node_BaseType_Void))
					return 1;
				
				if (LangC_CompareTypes(ctx, left->type, right->type) == 0)
					return 0;
				
				return LangC_INVALID;
			}
			else if (right->kind == LangC_NodeKind_BaseType)
			{
				int32 right_rank = LangC_RankOfType(ctx, right->flags);
				
				if (right_rank == -1)
					return LangC_INVALID;
				
				return 1;
			}
			else
			{
				return LangC_INVALID;
			}
		} break;
		
		case LangC_NodeKind_FunctionType:
		{
			if (right->kind != LangC_NodeKind_FunctionType)
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
		
		case LangC_NodeKind_ArrayType:
		{
			if (right->kind != LangC_NodeKind_ArrayType ||
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
	if (expr->kind == LangC_NodeKind_Expr)
	{
		switch (expr->flags & LangC_Node_LowerBits)
		{
			case LangC_Node_Expr_Deref:
			case LangC_Node_Expr_Assign:
			case LangC_Node_Expr_AssignAdd:
			case LangC_Node_Expr_AssignSub:
			case LangC_Node_Expr_AssignMul:
			case LangC_Node_Expr_AssignDiv:
			case LangC_Node_Expr_AssignMod:
			case LangC_Node_Expr_AssignLeftShift:
			case LangC_Node_Expr_AssignRightShift:
			case LangC_Node_Expr_AssignAnd:
			case LangC_Node_Expr_AssignOr:
			case LangC_Node_Expr_AssignXor:
			case LangC_Node_Expr_Index:
			case LangC_Node_Expr_CompoundLiteral:
			case LangC_Node_Expr_Access:
			case LangC_Node_Expr_DerefAccess:
			{
				return true;
			}
		}
	}
	else if (expr->kind == LangC_NodeKind_Ident)
	{
		LangC_Symbol* sym = LangC_FindSymbol(ctx, expr->name, false);
		return !(sym->kind == LangC_SymbolKind_GlobalFunction ||
				 sym->kind == LangC_SymbolKind_GlobalFunctionDecl ||
				 sym->kind == LangC_SymbolKind_LocalFunctionDecl ||
				 sym->kind == LangC_SymbolKind_EnumConstant);
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
		result = LangC_CreateNodeFrom(ctx, expr, LangC_NodeKind_Expr);
		result->flags |= LangC_Node_Expr_Cast;
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
	
	if (expr->type->kind == LangC_NodeKind_ArrayType || expr->type->kind == LangC_NodeKind_FunctionType)
	{
		result = LangC_CreateNodeFrom(ctx, expr, LangC_NodeKind_Expr);
		result->flags |= LangC_Node_Expr_Ref;
		result->expr = expr;
		result->type = LangC_CreateNodeFrom(ctx, result, LangC_NodeKind_PointerType);
		result->type->type = (expr->type->kind == LangC_NodeKind_ArrayType) ? expr->type->type : expr->type;
	}
	
	return result;
}

internal LangC_Node*
LangC_ResolveExpr(LangC_Context* ctx, LangC_Node* expr)
{
	switch (expr->kind)
	{
		case LangC_NodeKind_Expr:
		{
			uint64 low = (expr->flags & LangC_Node_LowerBits);
			
			if (low >= LangC_Node_Expr__FirstSpecial)
			{
				switch (low)
				{
					case LangC_Node_Expr_Call:
					{
						expr->left = LangC_ResolveExpr(ctx, expr->left);
						LangC_Node* functype = expr->left->type;
						
						if (functype->kind == LangC_NodeKind_PointerType)
							functype = functype->type;
						
						if (functype->kind != LangC_NodeKind_FunctionType)
						{
							LangC_NodeError(ctx, expr->left, "this is not a function.");
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
								
								LangC_Node* c = LangC_AddCastToExprIfNeeded(ctx, *arg, funcargs);
								if (!c)
								{
									LangC_NodeError(ctx, *arg, "cannot convert from type '%s' to '%s' when passing argument to function.",
													LangC_CStringFromType(ctx, (*arg)->type),
													LangC_CStringFromType(ctx, funcargs));
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
					
					case LangC_Node_Expr_Index:
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
							
							if (expr->left->type->kind != LangC_NodeKind_PointerType)
							{
								LangC_NodeError(ctx, expr, "expected a pointer type when indexing.");
							}
							else if (expr->right->type->kind == LangC_NodeKind_PointerType)
							{
								// TODO(ljre): maybe void* + T*?
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
					
					case LangC_Node_Expr_Cast:
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
					
					case LangC_Node_Expr_Ternary:
					{
						expr->left = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->left));
						expr->middle = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->middle));
						expr->right = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->right));
						
						LangC_Node* c = LangC_AddCastToExprIfNeeded(ctx, expr->left, LangC_INT);
						if (!c)
						{
							LangC_NodeError(ctx, expr->left, "condition cannot be converted to an int.");
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
					
					case LangC_Node_Expr_CompoundLiteral:
					{
						// TODO(ljre)
					} break;
					
					case LangC_Node_Expr_Initializer:
					{
						// TODO(ljre)
					} break;
					
					case LangC_Node_Expr_Access:
					{
						// TODO(ljre)
					} break;
					
					case LangC_Node_Expr_DerefAccess:
					{
						// TODO(ljre)
					} break;
					
					default: Unreachable(); break;
				}
			}
			else if (low >= LangC_Node_Expr__FirstBinary)
			{
				expr->left = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->left));
				expr->right = LangC_DecayExpr(ctx, LangC_ResolveExpr(ctx, expr->right));
				
				switch (low)
				{
					case LangC_Node_Expr_Comma:
					{
						expr->type = expr->right->type;
					} break;
					
					// TODO(ljre): All other binary exprs
					
					default: Unreachable(); break;
				}
			}
			else if (low >= LangC_Node_Expr__FirstUnary)
			{
				switch (low)
				{
					case LangC_Node_Expr_Ref:
					{
						expr->expr = LangC_ResolveExpr(ctx, expr->expr);
						
						expr->type = LangC_CreateNodeFrom(ctx, expr, LangC_NodeKind_PointerType);
						expr->type->type = expr->expr->type;
						
						if (0)
						{
							case LangC_Node_Expr_PrefixInc:
							case LangC_Node_Expr_PrefixDec:
							case LangC_Node_Expr_PostfixInc:
							case LangC_Node_Expr_PostfixDec:
							
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
					
					case LangC_Node_Expr_Sizeof:
					{
						LangC_Node* type;
						
						if (!expr->expr->type)
						{
							expr->expr = LangC_ResolveExpr(ctx, expr->expr);
							type = expr->expr->type;
						}
						else
						{
							type = expr->expr->type;
						}
						
						if (type->kind == LangC_NodeKind_FunctionType)
						{
							LangC_NodeError(ctx, expr, "cannot take sizeof of function.");
						}
						else
						{
							uint64 size, ignored;
							
							if (LangC_TypeInfo(ctx, type, &size, &ignored))
							{
								expr->type = LangC_SIZE_T;
								expr->value_uint = size;
							}
							else
							{
								LangC_NodeError(ctx, expr, "cannot take sizeof of incomplete type.");
							}
						}
					} break;
					
					default:
					{
						expr->expr = LangC_ResolveExpr(ctx, expr->expr);
					} break;
				}
			}
		} break;
		
		case LangC_NodeKind_Ident:
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
		
		case LangC_NodeKind_IntConstant: expr->type = LangC_INT; break;
		case LangC_NodeKind_LIntConstant: expr->type = LangC_LINT; break;
		case LangC_NodeKind_LLIntConstant: expr->type = LangC_LLINT; break;
		case LangC_NodeKind_UintConstant: expr->type = LangC_UINT; break;
		case LangC_NodeKind_LUintConstant: expr->type = LangC_LUINT; break;
		case LangC_NodeKind_LLUintConstant: expr->type = LangC_LLUINT; break;
		case LangC_NodeKind_FloatConstant: expr->type = LangC_FLOAT; break;
		case LangC_NodeKind_DoubleConstant: expr->type = LangC_DOUBLE; break;
		case LangC_NodeKind_StringConstant:
		{
			expr->type = LangC_CreateNodeFrom(ctx, expr, LangC_NodeKind_ArrayType);
			expr->type->type = LangC_CHAR;
			expr->type->length = expr->value_str.size + 1; // NOTE(ljre): null terminator
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
	Assert(decl->kind == LangC_NodeKind_Decl);
	
	LangC_SymbolKind sym_kind = LangC_SymbolKind_LocalVar;
	LangC_Node* type = LangC_ResolveType(ctx, decl->type);
	bool32 aliased = (type != decl->type);
	
	if (type->flags & LangC_Node_Poisoned)
	{
		decl->flags |= LangC_Node_Poisoned;
	}
	else switch (type->kind)
	{
		case LangC_NodeKind_ArrayType:
		{
			if (decl->type->expr)
			{
				if (!aliased)
				{
					decl->type->expr = LangC_ResolveExpr(ctx, decl->type->expr);
					LangC_TryToEval(ctx, decl->type->expr);
				}
				
				if (decl->type->expr->cannot_be_evaluated_at_compile_time)
					decl->type->kind = LangC_NodeKind_VariableLengthArrayType;
			}
			else if (!decl->expr)
			{
				LangC_NodeError(ctx, decl, "array declaration with implicit size needs initializer.");
			}
			
			if (decl->expr)
			{
				decl->expr = LangC_ResolveExpr(ctx, decl->expr);
				
				if (!decl->type->expr)
				{
					uint64 length = decl->expr->length;
					
					decl->type->expr = LangC_CreateNodeFrom(ctx, decl->expr, LangC_NodeKind_LLUintConstant);
					decl->type->expr->value_uint = length;
					decl->type->length = length;
				}
			}
		} break;
		
		case LangC_NodeKind_FunctionType:
		{
			// TODO(ljre)
			
			sym_kind = LangC_SymbolKind_LocalFunctionDecl;
		} break;
		
		case LangC_NodeKind_BaseType:
		{
			if (decl->expr && decl->expr->kind == LangC_NodeKind_Expr && (decl->expr->flags & LangC_Node_Expr_Initializer))
			{
				LangC_Node* resolved;
				bool32 is_struct = false;
				uint64 low = decl->type->flags & LangC_Node_LowerBits;
				LangC_Symbol* sym = NULL;
				
				if (low == LangC_Node_BaseType_Struct)
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
				else if (low == LangC_Node_BaseType_Typename)
				{
					sym = LangC_FindSymbol(ctx, decl->type->name, LangC_SymbolKind_Typename);
					resolved = LangC_ResolveType(ctx, sym->type);
					
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
		
		case LangC_NodeKind_PointerType:
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
	
	if (decl->flags & LangC_Node_Typedef)
		sym_kind = LangC_SymbolKind_Typename;
	
	if (decl->name.size > 0)
	{
		LangC_Symbol* sym = LangC_CreateSymbol(ctx, decl->name, sym_kind, decl->type);
		decl->symbol = sym;
	}
}

internal void
LangC_ResolveStmt(LangC_Context* ctx, LangC_Node* stmt)
{
	LangC_Node* cast;
	
	switch (stmt->kind)
	{
		case LangC_NodeKind_EmptyStmt: break;
		
		case LangC_NodeKind_IntConstant:
		case LangC_NodeKind_LIntConstant:
		case LangC_NodeKind_LLIntConstant:
		case LangC_NodeKind_UintConstant:
		case LangC_NodeKind_LUintConstant:
		case LangC_NodeKind_LLUintConstant:
		case LangC_NodeKind_FloatConstant:
		case LangC_NodeKind_DoubleConstant:
		case LangC_NodeKind_StringConstant:
		case LangC_NodeKind_WideStringConstant:
		case LangC_NodeKind_ReturnStmt:
		case LangC_NodeKind_Expr:
		{
			stmt->expr = LangC_ResolveExpr(ctx, stmt->expr);
		} break;
		
		case LangC_NodeKind_ForStmt:
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
		
		case LangC_NodeKind_SwitchStmt:
		case LangC_NodeKind_DoWhileStmt:
		case LangC_NodeKind_WhileStmt:
		case LangC_NodeKind_IfStmt:
		{
			// NOTE(ljre): If 'stmt->expr' is null, then the code gen shall generate an 1 constant
			if (stmt->expr)
				stmt->expr = LangC_ResolveExpr(ctx, stmt->expr);
			
			cast = LangC_AddCastToExprIfNeeded(ctx, stmt->expr, LangC_INT);
			if (!cast)
			{
				LangC_NodeError(ctx, stmt->expr, (stmt->kind == LangC_NodeKind_SwitchStmt) ?
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
		
		case LangC_NodeKind_Label:
		{
			LangC_ResolveStmt(ctx, stmt->stmt);
			
			// TODO(ljre): Add to the "labels table"
		} break;
		
		case LangC_NodeKind_CaseLabel:
		{
			stmt->expr = LangC_ResolveExpr(ctx, stmt->expr);
			
			cast = LangC_AddCastToExprIfNeeded(ctx, stmt->expr, LangC_INT);
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
		
		case LangC_NodeKind_CompoundStmt:
		{
			LangC_ResolveBlock(ctx, stmt->stmt);
		} break;
		
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
		if (LangC_SymbolAlreadyDefinedInThisScope(ctx, decl->name, LangC_SymbolKind_Typename))
			LangC_NodeError(ctx, decl, "redefinition of type '%.*s'.", StrFmt(decl->name));
		
		LangC_CreateSymbol(ctx, decl->name, LangC_SymbolKind_Typename, decl->type);
		return;
	}
	
	if (decl->type->kind == LangC_NodeKind_FunctionType)
	{
		// NOTE(ljre): "extern" is useless for functions.
		
		LangC_SymbolKind kind = LangC_SymbolKind_GlobalFunctionDecl;
		if (decl->body)
			kind = LangC_SymbolKind_GlobalFunction;
		
		LangC_Symbol* sym = LangC_CreateSymbol(ctx, decl->name, kind, decl->type);
		
		LangC_ResolveType(ctx, decl->type);
		
		if (decl->body)
			sym->locals = LangC_ResolveBlock(ctx, decl->body);
	}
	else
	{
		LangC_SymbolKind kind = LangC_SymbolKind_GlobalVar;
		
		if (decl->flags & LangC_Node_Extern)
		{
			kind = LangC_SymbolKind_GlobalVarDecl;
		}
		else
		{
			if (decl->flags & LangC_Node_Static)
			{
				kind = LangC_SymbolKind_GlobalStaticVar;
			}
			// NOTE(ljre): We have an "else" here because every static global variable declaration is a
			//             "potential declaration". This should be checked after the entire thing is resolved.
			else if (decl->type->kind == LangC_NodeKind_ArrayType && decl->type->expr == NULL && decl->type->length == 0)
			{
				// NOTE(ljre): Global arrays definitions with implicit length shall have length of 1.
				decl->type->length = 1;
				
				LangC_NodeWarning(ctx, decl->type, LangC_Warning_ImplicitLengthOf1, "global arrays definitions with implicit length have length of 1.");
			}
		}
		
		LangC_Symbol* sym = LangC_CreateSymbol(ctx, decl->name, kind, decl->type);
		(void)sym;
		
		LangC_ResolveType(ctx, decl->type);
	}
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
	
	LangC_PopSymbolStack(ctx);
	
	return LangC_error_count == 0;
}
